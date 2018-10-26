#pragma once

//#include <string>
#include "maths_funcs.h"

struct DirectionalLight
{
	vec3 Color;
	float AmbientIntensity;
	vec3 Direction;
	float DiffuseIntensity;
};