#pragma once

#include <GL/glew.h>
#include "maths_funcs.h"
//#include "Mesh.h"
#include "OBJ_loader.h"

namespace Assignment1
{
	class CGObject
	{
	public:
		CGObject();
		~CGObject();

		objl::Mesh Mesh;

		int startVBO = 0;
		int startIBO = 0;

		// Local translate transform
		vec3 initialTranslateVector = vec3(0, 0, 0);
		vec3 initialScaleVector = vec3(0.0f, 0.0f, 0.0f);
		vec3 initialRotateAngle = vec3(0, 0, 0);

		// Local rotate transform
		vec3 translateVector = vec3(0, 0, 0);
		vec3 scaleVector = vec3(0.0f, 0.0f, 0.0f);
		vec3 rotateAngles = vec3(0, 0, 0);

		CGObject *Parent;

		void Draw();
	};
}
