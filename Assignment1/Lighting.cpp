#pragma once
#include "Lighting.h"

	BaseLight::BaseLight()
	{
		Color = vec3(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}

	/*void BaseLight::AddToATB(TwBar *bar)
	{
		std::string s = Name + ".Color";
		TwAddVarRW(bar, s.c_str(), TW_TYPE_COLOR3F, &Color, NULL);
		s = Name + ".Ambient Intensity";
		TwAddVarRW(bar, s.c_str(), TW_TYPE_FLOAT, &AmbientIntensity, "min=0.0 max=1.0 step=0.005");
		s = Name + ".Diffuse Intensity";
		TwAddVarRW(bar, s.c_str(), TW_TYPE_FLOAT, &DiffuseIntensity, "min=0.0 max=1.0 step=0.005");
	}*/

	DirectionalLight::DirectionalLight()
	{
		Direction = vec3(0.0f, 0.0f, 0.0f);
	}

	/*void DirectionalLight::AddToATB(TwBar *bar)
	{
		BaseLight::AddToATB(bar);
		std::string s = Name + ".Direction";
		TwAddVarRW(bar, s.c_str(), TW_TYPE_DIR3F, &Direction, "axisz=-z");
	}*/
