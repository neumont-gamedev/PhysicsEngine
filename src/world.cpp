#include "world.h"
#include "integrator.h"
#include "effector.h"

Vector2 World::gravity = { 0, 9.8f };

void World::Step(float dt)
{
	// reset acceleration
	for (auto& body : bodies) body.acceleration = gravity * body.gravityScale * 100.0f;

	// force effector
	for (auto& effector : effectors) effector->Apply(bodies);

	/*
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		Vector2 position = GetMousePosition();
		for (auto& body : bodies)
		{
			Vector2 direction = body.position - position;
			//Vector2 direction = position - body.position;
			if (Vector2Length(direction) <= 100.0f)
			{
				Vector2 force = Vector2Normalize(direction) * 10000.0f;
				body.AddForce(force);
			}
		}

		DrawCircleLinesV(position, 100, WHITE);
	}
	*/
	// integrator
	for (auto& body : bodies) SemiImplicitEuler(body, dt);
	UpdateCollision();
}

void World::Draw()
{
	for (const auto& body : bodies) body.Draw();
	for (auto& effector : effectors) effector->Draw();
}

void World::UpdateCollision()
{
	// collision
	for (auto& body : bodies)
	{
		if (body.position.x + body.size > GetScreenWidth())
		{
			body.position.x = GetScreenWidth() - body.size;
			body.velocity.x *= -body.restitution;
		}
		if (body.position.x - body.size < 0)
		{
			body.position.x = body.size;
			body.velocity.x *= -body.restitution;
		}
		if (body.position.y + body.size > GetScreenHeight())
		{
			body.position.y = GetScreenHeight() - body.size;
			body.velocity.y *= -body.restitution;
		}
		//if (body.position.y - body.size < 0)
		//{
		//	body.position.y = body.size;
		//	body.velocity.y *= -body.restitution;
		//}
	}
}

void World::AddBody(const Body& body)
{
	bodies.push_back(body);
}

void World::AddEffector(Effector* effector)
{
	effectors.push_back(effector);
}
