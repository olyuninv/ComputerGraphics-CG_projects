#define _CRT_SECURE_NO_WARNINGS

#pragma once

//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <string>
#include "maths_funcs.h"
#include "Obj_parser.h"

//#include "Mesh.h"
#include "CGobject.h"
#include "OBJ_Loader.h"


#include "../CG_Utility/stdafx.h"

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;

unsigned int teapot_vao = 0;
int width = 1600;
int height = 600;
const float planeSize = 15.0f;

GLuint vbo = 0;
GLuint ibo = 0;

Assignment1::CGObject plane;
Assignment1::CGObject knight;
Assignment1::CGObject root;
Assignment1::CGObject cylinderBottom1;
Assignment1::CGObject cylinderBottom2;
Assignment1::CGObject cylinderBottom3;
Assignment1::CGObject cylinderBottom4;
Assignment1::CGObject cylinderBottom5;
Assignment1::CGObject cylinderTop1;
Assignment1::CGObject cylinderTop2;
Assignment1::CGObject cylinderTop3;
Assignment1::CGObject cylinderTop4;
Assignment1::CGObject cylinderTop5;

bool loadKinght = false;

GLuint loc1;
GLuint loc2;
GLuint loc3;
GLfloat rotatey = 0.0f;
GLfloat rotatez = 0.0f;
GLfloat movex = 0.0f;
GLfloat movey = 5.0f;

int matrix_location = 0;
int view_mat_location = 0;
int proj_mat_location = 0;
int normals_location = 0;

unsigned int plane_vao = 0;
unsigned int knight_vao = 0;
unsigned int root_vao = 0;
unsigned int cylinder_vao = 0;

Operation operation = Operation::None;
Direction direction = Direction::None;
bool printInfo = false;

enum class Phase
{
	Phase1,
	Phase2,
	Phase3,
	Phase4,
	Phase5
};

bool grabStarted = false;
bool pickupStarted = false;
Phase grabPhase = Phase::Phase1;
bool grabComplete = false;

GLfloat const step = 0.1f;
vec3 translateVector1 = vec3(0, -30, 0);
vec3 scaleVector1 = vec3(0.5f, 0.5f, 0.5f);
vec3 rotateAngles1 = vec3(0, 0, 0);

vec3 cameraTranslateVector = vec3(0, 0, 0);
vec3 cameraRotateVector = vec3(0, 0, 0);

#pragma region CUSTOM_FUNCTIONS

void updateUniformVariables(mat4 model)
{
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);

	mat4 normalsTransform = transpose(inverse(model));
	glUniformMatrix4fv(normals_location, 1, GL_FALSE, normalsTransform.m);
}

void updateUniformVariables(mat4 model, mat4 view, mat4 persp_proj)
{
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	updateUniformVariables(model);
}

mat4 orthogonal(float left, float right, float bottom, float top, float nearz, float farz) {
	mat4 m = zero_mat4();
	m.m[0] = 2 / (right - left);
	m.m[5] = 2 / (top - bottom);
	m.m[10] = 2 / (farz - nearz);
	m.m[12] = -((right + left) / (right - left));
	m.m[13] = -((top + bottom) / (top - bottom));
	m.m[14] = -((farz + nearz) / (farz - nearz));
	m.m[15] = 1.0f;

	return m;
};

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {
	FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "../Assignment1/Shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "../Assignment1/Shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

void linkCurrentBuffertoShader(int objectIndex)
{
	if (objectIndex == 1)
	{
		glBindVertexArray(plane_vao);

		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(plane.startVBO * 8 * sizeof(float)));

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(plane.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(3 * sizeof(GLfloat))));

		glEnableVertexAttribArray(loc3);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(plane.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(6 * sizeof(GLfloat))));

		//IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
	else if (objectIndex == 2)
	{
		glBindVertexArray(knight_vao);

		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(knight.startVBO * 8 * sizeof(float)));

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(knight.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(3 * sizeof(GLfloat))));

		glEnableVertexAttribArray(loc3);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(knight.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(6 * sizeof(GLfloat))));

		//IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
	else if (objectIndex == 3)
	{
		glBindVertexArray(cylinder_vao);

		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(cylinderBottom1.startVBO * 8 * sizeof(float)));

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(cylinderBottom1.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(3 * sizeof(GLfloat))));

		glEnableVertexAttribArray(loc3);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(cylinderBottom1.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(6 * sizeof(GLfloat))));

		//IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
	else if (objectIndex == 4)
	{
		glBindVertexArray(root_vao);

		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(root.startVBO * 8 * sizeof(float)));

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(root.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(3 * sizeof(GLfloat))));

		glEnableVertexAttribArray(loc3);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(root.startVBO * 8 * sizeof(float) + BUFFER_OFFSET(6 * sizeof(GLfloat))));

		//IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}
}

void addToObjectBuffer(int objectIndex, int startVBO, int n_vertices, float *vertices)
{
	glBufferSubData(GL_ARRAY_BUFFER, startVBO * 8 * sizeof(GLfloat), n_vertices * 8 * sizeof(GLfloat), vertices);

	// Vertex Attribute array	
	switch (objectIndex)
	{
	case 1:
		glGenVertexArrays(1, &plane_vao);
		break;
	case 2:
		glGenVertexArrays(1, &knight_vao);
		break;
	case 3:
		glGenVertexArrays(1, &cylinder_vao);
		break;
	case 4:
		glGenVertexArrays(1, &root_vao);
		break;
	}

	linkCurrentBuffertoShader(objectIndex);
}

void addToIndexBuffer(int startIBO, int n_indices, unsigned int *indices)
{
	// Create index buffer
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	GLenum error = glGetError();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, startIBO, sizeof(indices) * n_indices, indices);
}


#pragma endregion VBO_FUNCTIONS


void display() {

	if (printInfo)
	{
		printf("Operation is %s\r\n", operation == Operation::Translate ? "Translate" : operation == Operation::Rotate ? "Rotate" : operation == Operation::Scale ? "Scale" : "None");
		printf("Direction is %s\r\n", direction == Direction::X ? "X" : direction == Direction::Y ? "Y" : direction == Direction::Z ? "Z" : direction == Direction::Uniform ? "Uniform" : "None");
		printInfo = false;
	}

	HWND hwnd = GetActiveWindow();
	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	float aspectRatio = (float)width / (float)height;

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// SCREEN 1 
	// perspective
	glViewport(0, 0, width * 2 / 3, height);

	// Position the camera
	vec3 cameraPosition = vec3(20.0 + cameraTranslateVector.v[0] * step, 20.0 + cameraTranslateVector.v[1] * step, 20.0 + cameraTranslateVector.v[2] * step);
	mat4 view = look_at(cameraPosition, vec3(0.0, 0.0, 0.0), vec3(0.0 + cameraRotateVector.v[0] * step, 1.0 + cameraRotateVector.v[1] * step, 0.0 + cameraRotateVector.v[2] * step));
	mat4 persp_proj = perspective(40.0, aspectRatio, 0.1, 100.0);

	mat4 local1 = identity_mat4();
	updateUniformVariables(local1, view, persp_proj);

	// DRAW PLANE
	glBindVertexArray(plane_vao);
	linkCurrentBuffertoShader(1);
	plane.Draw();

	// DRAW ROOT
	mat4 globalRootTransform = root.createTransform();// Root of the Hierarchy				
	updateUniformVariables(globalRootTransform);
	root.globalTransform = globalRootTransform; // keep current state
	glBindVertexArray(root_vao);
	linkCurrentBuffertoShader(4);
	root.Draw();

	// DRAW CYLINDER - Bottom 1
	mat4 globalTransformCylinderBottom1 = cylinderBottom1.createTransform();
	updateUniformVariables(globalTransformCylinderBottom1);
	cylinderBottom1.globalTransform = globalTransformCylinderBottom1; // keep current state
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom1.Draw();

	// DRAW CYLINDER - Bottom 2
	mat4 globalTransformCylinderBottom2 = cylinderBottom2.createTransform();
	updateUniformVariables(globalTransformCylinderBottom2);
	cylinderBottom2.globalTransform = globalTransformCylinderBottom2; // keep current state
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom2.Draw();

	// DRAW CYLINDER - Bottom 3
	mat4 globalTransformCylinderBottom3 = cylinderBottom3.createTransform();
	updateUniformVariables(globalTransformCylinderBottom3);
	cylinderBottom3.globalTransform = globalTransformCylinderBottom3; // keep current state
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom3.Draw();

	// DRAW CYLINDER - Bottom 4
	mat4 globalTransformCylinderBottom4 = cylinderBottom4.createTransform();
	updateUniformVariables(globalTransformCylinderBottom4);
	cylinderBottom4.globalTransform = globalTransformCylinderBottom4; // keep current state
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom4.Draw();

	// DRAW CYLINDER - Bottom 5
	mat4 globalTransformCylinderBottom5 = cylinderBottom5.createTransform();
	updateUniformVariables(globalTransformCylinderBottom5);
	cylinderBottom5.globalTransform = globalTransformCylinderBottom5; // keep current state
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom5.Draw();

	if (loadKinght)
	{
		// DRAW Knight
		mat4 globalTransformKnight = knight.createTransform();
		updateUniformVariables(globalTransformKnight);
		knight.globalTransform = globalTransformKnight; // keep current state
		glBindVertexArray(knight_vao);
		linkCurrentBuffertoShader(2);
		knight.Draw();
	}

	// SCREEN 2
	// orthographic	
	glViewport(width * 2 / 3, 0, width / 3, height);

	cameraPosition = vec3(0.0, -10.0, 0.0);
	view = look_at(cameraPosition, vec3(0.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0));
	mat4 ortho_proj = orthogonal(-planeSize, planeSize, -planeSize, planeSize, -planeSize, planeSize);
	updateUniformVariables(local1, view, ortho_proj);

	// DRAW PLANE	
	glBindVertexArray(plane_vao);
	linkCurrentBuffertoShader(1);
	plane.Draw();

	// DRAW ROOT	
	updateUniformVariables(root.globalTransform);   // use already calculated transform	
	glBindVertexArray(root_vao);
	linkCurrentBuffertoShader(4);
	root.Draw();

	// DRAW CYLINDER - Bottom 1	
	updateUniformVariables(cylinderBottom1.globalTransform);
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom1.Draw();

	// DRAW CYLINDER - Bottom 2
	updateUniformVariables(cylinderBottom2.globalTransform);
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom2.Draw();

	// DRAW CYLINDER - Bottom 3
	updateUniformVariables(cylinderBottom3.globalTransform);
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom3.Draw();

	// DRAW CYLINDER - Bottom 4
	updateUniformVariables(cylinderBottom4.globalTransform);
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom4.Draw();

	// DRAW CYLINDER - Bottom 5
	updateUniformVariables(cylinderBottom5.globalTransform);
	glBindVertexArray(cylinder_vao);
	linkCurrentBuffertoShader(3);
	cylinderBottom5.Draw();

	if (loadKinght)
	{
		// DRAW Knight
		updateUniformVariables(knight.globalTransform);
		glBindVertexArray(knight_vao);
		linkCurrentBuffertoShader(2);
		knight.Draw();
	}

	glutSwapBuffers();

	glDisableVertexAttribArray(loc1);
	glDisableVertexAttribArray(loc2);
	glDisableVertexAttribArray(loc3);
}

void updateScene() {

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	if (grabStarted & !grabComplete)
	{
		bool xcentered = false;
		bool zcentered = false;

		switch (grabPhase)
		{
		case Phase::Phase1:
			// Move hand to center
			if (abs(root.globalTransform.m[12]) > 0.1 && root.globalTransform.m[12] < 0)  // current X-position negative
			{
				root.translateVector.v[0] += 0.1f;
			}
			else if (abs(root.globalTransform.m[12]) > 0.1 && root.globalTransform.m[12] > 0)  // current X-position positive
			{
				root.translateVector.v[0] -= 0.1f;
			}
			else
			{
				// We are centered in X-coord
				xcentered = true;
			}

			if (abs(root.globalTransform.m[14]) > 0.1 && root.globalTransform.m[14] < 0)  // current Z-position negative
			{
				root.translateVector.v[2] += 0.1f;
			}
			else if (abs(root.globalTransform.m[14]) > 0.1 && root.globalTransform.m[14] > 0)  // current Z-position positive
			{
				root.translateVector.v[2] -= 0.1f;
			}
			else
			{
				// We are centered in Z-coord
				zcentered = true;
			}

			if (xcentered && zcentered)
				grabPhase = Phase::Phase2;

			break;
		case Phase::Phase2:
			// Lower hand - assume begins higher than the knight
			if (root.globalTransform.m[13] > 2.5f)
			{
				root.translateVector.v[1] -= 0.1f;
			}
			else
			{
				grabPhase = Phase::Phase3;
			}

			break;
		case Phase::Phase3:
			// Grab knight
			if (cylinderBottom1.rotateAngles.v[2] > -90.0f)
			{
				// Turn all 5 fingers
				cylinderBottom1.rotateAngles.v[2] -= 0.5f;
				cylinderBottom2.rotateAngles.v[2] -= 0.5f;
				cylinderBottom3.rotateAngles.v[2] -= 0.5f;
				cylinderBottom4.rotateAngles.v[2] -= 0.5f;
				cylinderBottom5.rotateAngles.v[1] -= 0.5f;   // Rotate around X
			}
			else
			{
				// grab complete				
				grabComplete = true;

				// re-parent knight
				knight.Parent = &root;
				knight.initialTranslateVector = vec3(0.0f, -2.0f, 0.0);
			}
			break;
		default:
			break;
		}
	}

	if (pickupStarted)
	{

		switch (grabPhase)
		{
		case Phase::Phase4:
			// Pickup hand
			if (root.globalTransform.m[13] < root.initialTranslateVector.v[1])   // go back to initial hight
			{
				root.translateVector.v[1] += 0.1f;
			}
			else
			{
				grabPhase = Phase::Phase5;
			}
			break;
		case Phase::Phase5:
			// Release findgers and drop the knight				
			if (cylinderBottom1.rotateAngles.v[2] < 0.0f)
			{
				// Turn all 5 fingers
				cylinderBottom1.rotateAngles.v[2] += 0.5f;
				cylinderBottom2.rotateAngles.v[2] += 0.5f;
				cylinderBottom3.rotateAngles.v[2] += 0.5f;
				cylinderBottom4.rotateAngles.v[2] += 0.5f;
				cylinderBottom5.rotateAngles.v[1] += 0.5f;   // Rotate around X
			}
			else
			{
				// reset all variables as move is complete			
				grabStarted = false;
				grabComplete = false;
				pickupStarted = false;

				// re-parent knight
				knight.Parent = nullptr;
				knight.initialTranslateVector = vec3(0.0f, 0.0f, 0.0);
			}
		default:
			break;
		}
	}

	// Draw the next frame
	glutPostRedisplay();
}

void createObjects()
{
	int n_vbovertices = 0;
	int n_ibovertices = 0;

	//// Create plane where knight is standing
	plane = Assignment1::CGObject();
	objl::Vertex point1, point2, point3, point4 = objl::Vertex();
	point1.Position = objl::Vector3(-planeSize, 0.0f, -planeSize);
	point2.Position = objl::Vector3(planeSize, 0.0f, -planeSize);
	point3.Position = objl::Vector3(planeSize, 0.0f, planeSize);
	point4.Position = objl::Vector3(-planeSize, 0.0f, planeSize);

	point1.Normal = point2.Normal = point3.Normal = point4.Normal = objl::Vector3(0.0f, 1.0f, 0.0f);
	point1.TextureCoordinate = point2.TextureCoordinate = point3.TextureCoordinate = point4.TextureCoordinate = objl::Vector2(0.013400, 0.997700);  //green

	std::vector<objl::Vertex> vertices = std::vector<objl::Vertex>{ point1, point2, point3, point4 };
	std::vector<unsigned int> indices = std::vector<unsigned int>{ 0, 1, 2,
		0, 2, 3 };
	plane.Mesh = objl::Mesh(vertices, indices);
	plane.startVBO = 0;
	plane.startIBO = 0;

	float *vertices_ptr_plane = &plane.Mesh.Vertices[0].Position.X;
	unsigned int *indices_ptr_plane = &plane.Mesh.Indices[0];

	n_vbovertices += plane.Mesh.Vertices.size();
	n_ibovertices += plane.Mesh.Indices.size();

	objl::Loader obj_loader1; // = new objl::Loader();
	objl::Loader obj_loader2; // = new objl::Loader();
	objl::Loader obj_loader3; // = new objl::Loader();

	// load meshes
	const char* knightFileName = "../Assignment1/Mesh/Knight/Knight_sm.obj";
	const char* rootFileName = "../Assignment1/Mesh/Palm/palm.obj";
	const char* cylinderFileName = "../Assignment1/Mesh/Cylinder/cylinder.obj";

	// root
	float *vertices_ptr_root = NULL;
	unsigned int *indices_ptr_root = NULL;
	bool result = obj_loader1.LoadFile(rootFileName);
	if (result)
	{
		// create separate vertex array for vertices, normals etc
		vertices_ptr_root = &obj_loader1.LoadedMeshes[0].Vertices[0].Position.X;
		indices_ptr_root = &obj_loader1.LoadedMeshes[0].Indices[0];
		root = Assignment1::CGObject();
		root.Mesh = obj_loader1.LoadedMeshes[0];
		root.startVBO = n_vbovertices;
		root.startIBO = n_ibovertices;
		root.initialTranslateVector = vec3(-10.0f, 4.0f, -10.0f);

		n_vbovertices += root.Mesh.Vertices.size();
		n_ibovertices += root.Mesh.Indices.size();
	}

	// cylinders
	float *vertices_ptr_cylinder = NULL;
	unsigned int *indices_ptr_cylinder = NULL;
	result = obj_loader2.LoadFile(cylinderFileName);
	if (result)
	{
		// create separate vertex array for vertices, normals etc
		vertices_ptr_cylinder = &obj_loader2.LoadedMeshes[0].Vertices[0].Position.X;
		indices_ptr_cylinder = &obj_loader2.LoadedMeshes[0].Indices[0];
		cylinderBottom1 = cylinderBottom2 = cylinderBottom3 = cylinderBottom4 = cylinderBottom5 = Assignment1::CGObject();
		cylinderBottom1.Mesh = cylinderBottom2.Mesh = cylinderBottom3.Mesh = cylinderBottom4.Mesh = cylinderBottom5.Mesh = obj_loader2.LoadedMeshes[0];
		cylinderBottom1.startVBO = cylinderBottom2.startVBO = cylinderBottom3.startVBO = cylinderBottom4.startVBO = cylinderBottom5.startVBO = n_vbovertices;
		cylinderBottom1.startIBO = cylinderBottom2.startIBO = cylinderBottom3.startIBO = cylinderBottom4.startIBO = cylinderBottom5.startIBO = n_ibovertices;

		// Positions
		cylinderBottom1.initialTranslateVector = vec3(1.0f, 0.0f, -1.5f);
		cylinderBottom2.initialTranslateVector = vec3(1.0f, 0.0f, -0.5f);
		cylinderBottom3.initialTranslateVector = vec3(1.0f, 0.0f, 0.5f);
		cylinderBottom4.initialTranslateVector = vec3(1.0f, 0.0f, 1.5f);
		cylinderBottom5.initialTranslateVector = vec3(0.0f, 0.0f, 1.5f);
		cylinderBottom5.initialRotateAngle = vec3(0.0f, -90.0f, 0.0f);
		cylinderBottom1.Parent = cylinderBottom2.Parent = cylinderBottom3.Parent = cylinderBottom4.Parent = cylinderBottom5.Parent = &root;

		n_vbovertices += cylinderBottom1.Mesh.Vertices.size();
		n_ibovertices += cylinderBottom1.Mesh.Indices.size();
	}

	// knight
	float *vertices_ptr_knight = NULL;
	unsigned int *indices_ptr_knight = NULL;
	if (loadKinght)
	{		
		result = obj_loader3.LoadFile(knightFileName);
		if (result)
		{
			vertices_ptr_knight = &obj_loader3.LoadedMeshes[0].Vertices[0].Position.X;
			indices_ptr_knight = &obj_loader3.LoadedMeshes[0].Indices[0];
			knight = Assignment1::CGObject();
			knight.Mesh = obj_loader3.LoadedMeshes[0];
			knight.startVBO = n_vbovertices;
			knight.startIBO = n_ibovertices;

			// Position
			knight.initialRotateAngle = vec3(0, 180, 0);

			n_vbovertices += knight.Mesh.Vertices.size();
			n_ibovertices += knight.Mesh.Indices.size();
		}
	}

	//Declare your uniform variables that will be used in your shader
	matrix_location = glGetUniformLocation(shaderProgramID, "model");
	view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
	normals_location = glGetUniformLocation(shaderProgramID, "normals");

	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normals");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	// Create VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLenum error = glGetError();
	glBufferData(GL_ARRAY_BUFFER, n_vbovertices * 8 * sizeof(float), NULL, GL_STATIC_DRAW);  // Vertex contains 8 floats: position (vec3), normal (vec3), texture (vec2)

	if (error != 0)
		throw exception("Could not initialise Vertex Buffer");

	addToObjectBuffer(1, plane.startVBO, plane.Mesh.Vertices.size(), vertices_ptr_plane);
	addToObjectBuffer(4, root.startVBO, root.Mesh.Vertices.size(), vertices_ptr_root);
	addToObjectBuffer(3, cylinderBottom1.startVBO, cylinderBottom1.Mesh.Vertices.size(), vertices_ptr_cylinder);
	if (loadKinght)
	{
		addToObjectBuffer(2, knight.startVBO, knight.Mesh.Vertices.size(), vertices_ptr_knight);
	}

	// Create IBO
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_ibovertices * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, plane.startIBO * sizeof(unsigned int), sizeof(unsigned int) * plane.Mesh.Indices.size(), indices_ptr_plane);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, root.startIBO * sizeof(unsigned int), sizeof(unsigned int) * root.Mesh.Indices.size(), indices_ptr_root);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cylinderBottom1.startIBO * sizeof(unsigned int), sizeof(unsigned int) * cylinderBottom1.Mesh.Indices.size(), indices_ptr_cylinder);
	if (loadKinght)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, knight.startIBO * sizeof(unsigned int), sizeof(unsigned int) * knight.Mesh.Indices.size(), indices_ptr_knight);
	}

	//addToIndexBuffer(iboMeshes[0], plane.Mesh.Indices.size(), indices_ptr_plane);
}

void init()
{
	// Set up the shaders
	shaderProgramID = CompileShaders();

	createObjects();

	operation = Operation::Translate;
	direction = Direction::X;
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {

	switch (key) {
	case 'g':
		// Grab knight
		if (!grabStarted)
		{
			grabStarted = true;
			grabPhase = Phase::Phase1;
		}
		break;
	case 'p':
		// Pickup knight and release
		if (grabComplete)
		{
			pickupStarted = true;
			grabPhase = Phase::Phase4;
		}
		break;
	case 'r':
		operation = Operation::Rotate;
		direction = Direction::X;
		printInfo = true;
		break;
	case 't':
		operation = Operation::Translate;
		direction = Direction::X;
		printInfo = true;
		break;
		/*case 's':
			operation = Operation::Scale;
			direction = Direction::X;
			printInfo = true;
			break;*/
			/*case 'u':
				operation = Operation::Scale;
				direction = Direction::Uniform;
				printInfo = true;
				break;*/
	case 'x':
		direction = Direction::X;
		printInfo = true;
		break;
	case 'y':
		direction = Direction::Y;
		printInfo = true;
		break;
	case 'z':
		direction = Direction::Z;
		printInfo = true;
		break;
	default:
		break;
	}

}

void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		switch (operation)
		{
		case (Operation::Translate):
			switch (direction)
			{
			case (Direction::Z):
				cameraTranslateVector.v[2]++;
				break;
			default:
				// Assume Y- axis unless Z is selected
				cameraTranslateVector.v[1]++;
				break;
			}
			break;
			//case (Operation::Scale):
			//	switch (direction)
			//	{			
			//	case (Direction::Z):
			//		scaleVector1.v[2] += step;
			//		break;
			//	case (Direction::Uniform):
			//		scaleVector1.v[0] += step;
			//		scaleVector1.v[1] += step;
			//		scaleVector1.v[2] += step;
			//		break;
			//	default:
			//		// Assume Y-scaling unless  Z or Uniform are specified
			//		scaleVector1.v[1] += step;
			//		break;
			//	}
			//break;
		case (Operation::Rotate):
			switch (direction)
			{
			case (Direction::Z):
				cameraRotateVector.v[2]++;
				break;
			default:
				// Assume rotation around Y-axis unless Z is specified
				cameraRotateVector.v[1]++;
				break;
			}
			break;
		case(Operation::None):
		default:
			break;
		}
		break;
	case GLUT_KEY_DOWN:
		switch (operation)
		{
		case (Operation::Translate):
			switch (direction)
			{
			case (Direction::Z):
				cameraTranslateVector.v[2]--;
				break;
			default:
				// Assume Y-axis
				cameraTranslateVector.v[1]--;
				break;
			}
			break;
			//case (Operation::Scale):
			//	switch (direction)
			//	{					
			//	case (Direction::Z):
			//		scaleVector1.v[2] -= step;
			//		break;
			//	case (Direction::Uniform):
			//		scaleVector1.v[0] -= step;
			//		scaleVector1.v[1] -= step;
			//		scaleVector1.v[2] -= step;
			//		break;
			//	default:
			//		// Assume y-axis
			//		scaleVector1.v[1] -= step;
			//		break;
			//	}
			//	break;
		case (Operation::Rotate):
			switch (direction)
			{
			case (Direction::Z):
				cameraRotateVector.v[2]--;
				break;
			default:
				// Assume Y axis
				cameraRotateVector.v[1]--;
				break;
			}
			break;
		default:
			break;
		}
		break;
	case GLUT_KEY_LEFT:
		// Assume operation is for X-axis
		switch (operation)
		{
			/*case (Operation::Scale):
				scaleVector1.v[0] -= step;
				break;*/
		case (Operation::Rotate):
			cameraRotateVector.v[0]--;
			break;
		default:
			cameraTranslateVector.v[0]--;
			break;
		}
		break;
	case GLUT_KEY_RIGHT:
		// Assume operation is for X-axis
		switch (operation)
		{
			/*case (Operation::Scale):
				scaleVector1.v[0]+= step;
				break;*/
		case (Operation::Rotate):
			cameraRotateVector.v[0]++;
			break;
		default:
			// Assume translate
			cameraTranslateVector.v[0]++;
			break;
		}
		break;
	default:
		break;
	}
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Knight");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(specialKeys);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Set up your objects and shaders
	init();

	// Begin infinite event loop
	glutMainLoop();
	return 0;
}











