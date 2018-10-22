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

		// Local translate transform
		vec3 initialTranslateVec;
		vec3 translateVec;

		// Local rotate transform
		vec3 initialRotateVec;
		vec3 rotateVec;

		CGObject *Parent;

		void Draw();
	};
}
