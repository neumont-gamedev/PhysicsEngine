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

	world.AddEffector(new PointEffector(Vector2{ 300, 200 }, 200, -30000.0f));
	world.AddEffector(new DragEffector(Vector2{ 300, 600 }, 200, 40.0f));
	world.AddEffector(new AreaEffector(Vector2{ 900, 200 }, 200, 0,30000.0f));
	world.AddEffector(new GravitationEffector(Vector2{ 900, 600 }, 200, 30000.0f));

	float timeAccum = 0.0f;
	float fixedTimeStep = 1.0f / 60.0f; // 0.016 * 60.0 = 1.0
	bool simulate = true;
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		float dt = fminf(GetFrameTime(), 0.1f);

		// update gui input
		if (IsKeyPressed(KEY_SPACE)) state.SimulateActive = !state.SimulateActive;
		World::SetGravity(Vector2{ 0.0f, state.GravityValue });

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || 
		   (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)))
		{
			Body body;

			body.bodyType = (BodyType)state.BodyTypeActive;//(IsKeyDown(KEY_LEFT_ALT)) ? BodyType::Static : BodyType::Dynamic;

			body.position = GetMousePosition();
			// get random unit circle vector
			float angle = GetRandomFloat() * (2 * PI);
			Vector2 direction;
			direction.x = cosf(angle);
			direction.y = sinf(angle);

			body.AddForce(direction * (50.0f + (GetRandomFloat() * 500)), ForceMode::VelocityChange);
			
			body.size = 5.0f + (GetRandomFloat() * 30.0f);
			body.restitution = 0.5f + (GetRandomFloat() * 0.5f);
			body.mass = body.size;
			body.inverseMass = (body.bodyType == BodyType::Static) ? 0 : 1.0f / body.mass;
			body.gravityScale = 1.0f;
			body.damping = 0.1f;

			world.AddBody(body);
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
