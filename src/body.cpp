#include "body.h"

void Body::AddForce(Vector2 force)
{
	acceleration += force / mass;
}

void Body::Draw() const
{
	DrawCircleV(position, size, RED);
}
