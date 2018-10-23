#include "CGObject.h"
namespace Assignment1
{
	CGObject::CGObject()
	{
	}

	CGObject::~CGObject()
	{
	}

	void CGObject::Draw()
	{
		glDrawElements(GL_TRIANGLES, this->Mesh.Indices.size(), GL_UNSIGNED_INT, (void*)((this->startIBO) * sizeof(unsigned int))); 
	}
}