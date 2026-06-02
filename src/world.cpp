#include "world.h"
#include "integrator.h"
#include "effector.h"
#include "spring.h"

Vector2 World::gravity = { 0, 9.8f };
float World::springMultiplier = 1.0f;

void World::Step(float dt)
{
	// update gravity
	for (auto& body : bodies) body.AddForce(gravity * body.gravityScale, ForceMode::Acceleration);

	// force effector
	for (auto& effector : effectors) effector->Apply(bodies);

	// spring
	for (auto& spring : springs) spring->Apply(springMultiplier);

	// integrator
	for (auto& body : bodies) if (body.bodyType == BodyType::Dynamic) SemiImplicitEuler(body, dt);
	for (int i = 0; i < 4; i++) UpdateCollision();

	// reset acceleration
	for (auto& body : bodies) body.acceleration = Vector2{ 0, 0 };
}

void World::Draw()
{
	// draw a grid of lines with each grid being one unit in size
	// vertical
	DrawLineV(Vector2{ 0, boundsMin.y }, Vector2{ 0, boundsMax.y }, WHITE);
	for (float x = 1; x < (boundsMax.x - boundsMin.x) * 0.5f; x += 1)
	{
		DrawLineV(Vector2{ +x, boundsMin.y }, Vector2{ +x, boundsMax.y }, GRAY);
		DrawLineV(Vector2{ -x, boundsMin.y }, Vector2{ -x, boundsMax.y }, GRAY);
	}
	// horizontal
	DrawLineV(Vector2{ boundsMin.x, 0 }, Vector2{ boundsMax.x, 0 }, WHITE);
	for (float y = 1; y < (boundsMax.y - boundsMin.y) * 0.5f; y += 1)
	{
		DrawLineV(Vector2{ boundsMin.x, +y }, Vector2{ boundsMax.x, +y }, GRAY);
		DrawLineV(Vector2{ boundsMin.x, -y }, Vector2{ boundsMax.x, -y }, GRAY);
	}

	for (auto& effector : effectors) effector->Draw();
	for (auto& spring : springs) spring->Draw();
	for (const auto& body : bodies) body.Draw();
}

void World::UpdateCollision()
{
	contacts.clear();
	CreateContacts(bodies, contacts);
	SeparateContacts(contacts);
	ResolveContacts(contacts);

	// collision
	for (auto& body : bodies)
	{
		if (body.position.x + body.size > boundsMax.x)
		{
			body.position.x = boundsMax.x - body.size;
			body.velocity.x *= -body.restitution;
		}
		if (body.position.x - body.size < boundsMin.x)
		{
			body.position.x = boundsMin.x + body.size;
			body.velocity.x *= -body.restitution;
		}
		if (body.position.y + body.size > boundsMax.y)
		{
			body.position.y = boundsMax.y - body.size;
			body.velocity.y *= -body.restitution;
		}
		if (body.position.y - body.size < boundsMin.y)
		{
			body.position.y = boundsMin.y + body.size;
			body.velocity.y *= -body.restitution;
		}
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

void World::AddSpring(Body& bodyA, Body& bodyB, float restLength, float stiffness, float damping)
{
	Spring* spring = new Spring(&bodyA, &bodyB, restLength, stiffness, damping);
	springs.push_back(spring);
}

Body* World::GetBodyIntersect(Vector2 position)
{
	for (auto& body : bodies)
	{
		if (CheckCollisionPointCircle(position, body.position, body.size))
		{
			return &body;
		}
	}

	return nullptr;
}

