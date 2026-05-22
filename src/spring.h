#pragma once
#include "raylib.h"

class Spring
{
public:
	struct Body* bodyA;
	struct Body* bodyB;
	float restLength;
	float k; // stiffness
	float damping;

public:
	Spring() = default;
	Spring(Body* bodyA, Body* bodyB, float restLength, float k, float damping = 0) :
		bodyA{ bodyA },
		bodyB{ bodyB },
		restLength{ restLength },
		k{ k },
		damping{ damping }
	{
	}

	void Apply(float kMultiplier = 1);
	void Draw();

	static Vector2 GetSpringForce(Vector2 positionA, Vector2 positionB, float restLength, float k);
};
