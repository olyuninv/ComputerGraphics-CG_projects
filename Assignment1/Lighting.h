#pragma once

#include <string>
#include "maths_funcs.h"

class BaseLight
{
public:
	std::string Name;
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight(); 

	//virtual void AddToATB(TwBar *bar);
};


class DirectionalLight : public BaseLight
{
public:
	vec3 Direction;

	DirectionalLight();

	//virtual void AddToATB(TwBar *bar);
};
