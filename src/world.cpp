#include "world.h"
#include "integrator.h"
#include "effector.h"

Vector2 World::gravity = { 0, 9.8f };

void World::Step(float dt)
{
	// reset acceleration
	for (auto& body : bodies) body.acceleration = Vector2{ 0, 0 };
	for (auto& body : bodies) body.AddForce(gravity * body.gravityScale * 100.0f, ForceMode::Acceleration);

	// force effector
	for (auto& effector : effectors) effector->Apply(bodies);

	// integrator
	for (auto& body : bodies) if (body.bodyType == BodyType::Dynamic) SemiImplicitEuler(body, dt);
	UpdateCollision();
}

void World::Draw()
{
	for (const auto& body : bodies) body.Draw();
	for (auto& effector : effectors) effector->Draw();
}

void World::UpdateCollision()
{
	contacts.clear();
	CreateContacts(bodies, contacts);
	SeparateContacts(contacts);

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
