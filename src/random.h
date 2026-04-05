#pragma once
#include "raylib.h"

// return 0-1
inline float GetRandomFloat()
{
	return GetRandomValue(0, 10000) / (float)10000;
}