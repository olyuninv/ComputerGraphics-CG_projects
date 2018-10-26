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

		// Initial transform
		vec3 initialTranslateVector = vec3(0, 0, 0);
		vec3 initialScaleVector = vec3(0.0f, 0.0f, 0.0f);
		vec3 initialRotateAngle = vec3(0, 0, 0);

		// Transform vectors
		vec3 translateVector = vec3(0, 0, 0);
		vec3 scaleVector = vec3(0.0f, 0.0f, 0.0f);
		vec3 rotateAngles = vec3(0, 0, 0);

		// Color
		vec3 color = vec3(1.0f, 1.0f, 1.0f); // White

		// Current position
		mat4 globalTransform = identity_mat4();
		
		CGObject *Parent = nullptr;

		void Draw();
		mat4 createTransform();
	};
}
