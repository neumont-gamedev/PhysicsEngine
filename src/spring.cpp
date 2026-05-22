#include "spring.h"
#include "body.h"
#include "raymath.h"

void Spring::Apply(float kMultiplier)
{
	Vector2 force = GetSpringForce(bodyA->position, bodyB->position, restLength, k);
	
	// damp spring force to prevent oscillation
	//Vector2 dv = bodyA->velocity - bodyB->velocity;
	//float dampFactor = Vector2DotProduct(dv, ndirection) * damping;
	//Vector2 dampingForce = ndirection * dampFactor;
	//force -= dampingForce;

	// apply spring force
	bodyA->AddForce(force * 50.0f);
	bodyB->AddForce(Vector2Negate(force * 50.0f));
}

Vector2 Spring::GetSpringForce(Vector2 positionA, Vector2 positionB, float restLength, float k)
{
	// direction = positionB --> positionA
	Vector2 direction = positionA - positionB;
	float lengthSqr = Vector2LengthSqr(direction);
	if (lengthSqr <= EPSILON) return Vector2Zero();

	// calculate spring force
	float length = sqrtf(lengthSqr);
	float displacement = length - restLength;	// current length - resting length (>0 stretched, <0 compressed)
	float magnitude = -k * displacement;   // f = -kx <- Hooke's Law

	// calculate normalized direction (vector / length)
	direction = direction / length;	

	// return spring force
	return direction * magnitude;
}

void Spring::Draw()
{
	DrawLineV(bodyA->position, bodyB->position, WHITE);
}