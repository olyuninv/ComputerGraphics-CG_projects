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

	mat4 CGObject::createTransform()
	{
		mat4 localTransform = identity_mat4();
		localTransform = rotate_x_deg(localTransform, this->initialRotateAngle.v[0] + this->rotateAngles.v[0]);
		localTransform = rotate_y_deg(localTransform, this->initialRotateAngle.v[1] + this->rotateAngles.v[1]);
		localTransform = rotate_z_deg(localTransform, this->initialRotateAngle.v[2] + this->rotateAngles.v[2]);
		localTransform = translate(localTransform, this->initialTranslateVector + this->translateVector);

		mat4 parentTransform = Parent == nullptr ? identity_mat4() : Parent->globalTransform;
		return parentTransform * localTransform;
	}
}