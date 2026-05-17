#pragma once
#include "effector.h"

class GravitationEffector : public Effector
{
public:
	GravitationEffector(Vector2 position, float size, float strength) : 
		Effector{ position, size },
		strength{ strength } {}

	void Apply(std::vector<Body>& bodies) override;
	void Draw() override;

private:
	float strength = 10.0f;
};
