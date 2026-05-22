#include "world.h"
#include "integrator.h"
#include "effector.h"
#include "spring.h"

Vector2 World::gravity = { 0, 9.8f };

void World::Step(float dt)
{
	for (auto& body : bodies) body.AddForce(gravity * body.gravityScale * 100.0f, ForceMode::Acceleration);

	// force effector
	for (auto& effector : effectors) effector->Apply(bodies);

	// spring
	for (auto& spring : springs) spring->Apply(100.0f);

	// integrator
	for (auto& body : bodies) if (body.bodyType == BodyType::Dynamic) SemiImplicitEuler(body, dt);
	for (int i = 0; i < 4; i++) UpdateCollision();

	// reset acceleration
	for (auto& body : bodies) body.acceleration = Vector2{ 0, 0 };
}

void World::Draw()
{
	for (auto& effector : effectors) effector->Draw();
	for (const auto& body : bodies) body.Draw();
	for (auto& spring : springs) spring->Draw();
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
		if (body.position.y - body.size < 0)
		{
			body.position.y = body.size;
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

void World::AddSpring(Body& bodyA, Body& bodyB, float restLength, float stiffness)
{
	Spring* spring = new Spring(&bodyA, &bodyB, restLength, stiffness);
	springs.push_back(spring);
}

Body* World::GetBodyIntersect(const Vector2& position)
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

