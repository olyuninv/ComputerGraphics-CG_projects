#pragma once

//#include <string>
#include "maths_funcs.h"

//class BaseLight
//{
//public:
//	std::string Name;
//	vec3 Color;
//	float AmbientIntensity;
//	float DiffuseIntensity;
//
//	BaseLight(); 
//
//	//virtual void AddToATB(TwBar *bar);
//};

struct DirectionalLight
{
	vec3 Color;
	float AmbientIntensity;
	vec3 Direction;
	float DiffuseIntensity;
};