/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/
*/

#include "body.h"
#include "random.h"
#include "integrator.h"
#include "world.h"
#include "point_effector.h"
#include "gravitation_effector.h"
#include "area_effector.h"
#include "drag_effector.h"

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define GUI_PHYSICS_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4576)
#include "gui_physics.h"
#pragma warning(pop)

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include <vector>
#include <string>

GuiPhysicsState state;
//GuiLoadStyle("raygui/styles/jungle/style_jungle.rgs");

void AddBody(World& world);
void AddEffector(World& world);

int main ()
{
	SetRandomSeed(5);

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 720, "Physics Engine");

	// Get GUI state
	state = InitGuiPhysics();

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	World world;


	float timeAccum = 0.0f;
	bool simulate = true;
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		float dt = fminf(GetFrameTime(), 0.1f);
		float fixedTimeStep = 1.0f / state.FPSValue; // 0.016 * 60.0 = 1.0

		// update gui input
		if (IsKeyPressed(KEY_SPACE)) state.SimulateActive = !state.SimulateActive;
		if (IsKeyPressed(KEY_TAB)) state.PhysicsPanelActive = !state.PhysicsPanelActive;

		World::SetGravity(Vector2{ 0.0f, state.GravityValue });

		bool mouseOverGui = state.PhysicsPanelActive && CheckCollisionPointRec(GetMousePosition(), Rectangle{ state.anchor02.x, state.anchor02.y, 304, 664 });
		if (!mouseOverGui)
		{
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
				(IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)))
			{
				if (IsKeyDown(KEY_LEFT_SHIFT))
				{
					AddEffector(world);
				}
				else
				{
					AddBody(world);
				}
			}
		}

		// UPDATE
		if (state.SimulateActive)
		{
			timeAccum += dt;
			while (timeAccum > fixedTimeStep)
			{
				world.Step(fixedTimeStep);
				timeAccum -= fixedTimeStep;
			}
		}

		// DRAW
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(WHITE);

		// draw some text using the default font
		std::string fpsText = "FPS: ";
		fpsText += std::to_string(GetFPS());
		DrawText(fpsText.c_str(), 40, 40, 20, WHITE);

		world.Draw();

		GuiPhysics(&state);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

void AddBody(World& world)
{
	Body body;

	body.bodyType = (BodyType)state.BodyTypeActive;

	body.position = GetMousePosition();
	// get random unit circle vector
	float angle = GetRandomFloat() * (2 * PI);
	Vector2 direction;
	direction.x = cosf(angle);
	direction.y = sinf(angle);

	body.AddForce((direction * state.BodyVelocityValue), ForceMode::VelocityChange);

	body.size = state.BodySizeValue;
	body.restitution = state.BodyRestitutionValue;
	body.mass = body.size * state.BodyMassValue;
	body.inverseMass = (body.bodyType == BodyType::Static) ? 0 : 1.0f / body.mass;
	body.gravityScale = state.BodyGravityValue;
	body.damping = state.BodyDampingValue;

	world.AddBody(body);
}

void AddEffector(World& world)
{
	Vector2 position = GetMousePosition();

	Effector* effector = nullptr;
	switch ((EffectorType)state.EffectorTypeActive)
	{
	case EffectorType::Gravitation:
		effector = new GravitationEffector(position, state.EffectorSizeValue, state.EffectorForceValue * 10000.0f);
		break;
	case EffectorType::Point:
		effector = new PointEffector(position, state.EffectorSizeValue, state.EffectorForceValue * 10000.0f);

		break;
	case EffectorType::Area:
		effector = new AreaEffector(position, state.EffectorSizeValue, state.EffectorAngleValue, state.EffectorForceValue * 10000.0f);
		break;
	case EffectorType::Drag:
		effector = new DragEffector(position, state.EffectorSizeValue, state.EffectorForceValue);
		break;
	}

	if (effector) world.AddEffector(effector);
}