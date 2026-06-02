#pragma once
#include "body.h"
#include "collision.h"
#include <vector>

class World
{
public:
	World()
	{
		bodies.reserve(1000);
	}

	void Step(float dt);
	void Draw();

	void AddBody(const Body& body);
	void AddEffector(class Effector* effector);
	void AddSpring(Body& bodyA,  Body& bodyB, float restLength, float stiffness, float damping);

	std::vector<Body>& GetBodies() { return bodies; }
	const std::vector<Body>& GetBodies() const { return bodies; }

	Body* GetBodyIntersect(Vector2 position);

	static void SetGravity(Vector2 newgravity) { gravity = newgravity; }
	static void SetSpringMultiplier(float multiplier) { springMultiplier = multiplier; }

	void SetBounds(Vector2 min, Vector2 max) { boundsMin = min; boundsMax = max; }

private:
	void UpdateCollision();

private:
	Vector2 boundsMin{ -10.0f, -5.0f };
	Vector2 boundsMax{  10.0f,  5.0f };

	static Vector2 gravity;
	static float springMultiplier;

	std::vector<Body> bodies;
	std::vector<Contact> contacts;
	std::vector<class Effector*> effectors;
	std::vector<class Spring*> springs;
};