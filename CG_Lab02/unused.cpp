//// Print current parameters on the screen
////unsigned char string[] = "Operation is T";
////int w = glutBitmapLength(GLUT_BITMAP_8_BY_13, string);
////glRasterPos2f(0., 0.);
//////glColor(1., 0., 0.);
////int len = strlen(string);
////for (int i = 0; i < len; i++) {
////	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, string);
////}
//
///*
//if (xIncrement < 100)
//{
//xIncrement ++;
//}*/
//
//
////std::vector<vec4> verticesGround;
////std::vector<vec3> normalsGround;
////std::vector<GLushort> elementsGround;
////std::vector<GLushort> elementNormalsGround;
////Obj_parser::load_obj(groundFileName, verticesGround, normalsGround, elementsGround);
////Obj_parser::load_obj2(groundFileName, verticesGround, normalsGround);
////Mesh groundMesh = Mesh();
////groundMesh.VertexPoints = verticesGround;
//
//#define _CRT_SECURE_NO_WARNINGS
//
//#pragma once
//
////Some Windows Headers (For Time, IO, etc.)
//#include <windows.h>
//#include <mmsystem.h>
//#include <GL/glew.h>
//#include <GL/freeglut.h>
//#include <iostream>
//#include <vector>
//#include <string>
//#include "maths_funcs.h"
//#include "Obj_parser.h"
//
////#include "Mesh.h"
//#include "CGobject.h"
//#include "OBJ_Loader.h"
//
//
//#include "../CG_Utility/stdafx.h"
//
//// Macro for indexing vertex buffer
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//
//using namespace std;
//GLuint shaderProgramID;
//
//unsigned int teapot_vao = 0;
//int width = 800;
//int height = 600;
//
//GLuint vbo = 0;
//GLuint ibo = 0;
//int vboMeshes[4];
//int iboMeshes[4];
//Assignment1::CGObject plane;
//Assignment1::CGObject knight;
//Assignment1::CGObject root;
//Assignment1::CGObject cylinder;
//
//GLuint loc1;
//GLuint loc2;
//GLuint loc3;
//GLfloat rotatey = 0.0f;
//GLfloat rotatez = 0.0f;
//GLfloat movex = 0.0f;
//GLfloat movey = 5.0f;
//
//int matrix_location = 0;
//int view_mat_location = 0;
//int proj_mat_location = 0;
//int normals_location = 0;
//
//unsigned int plane_vao = 0;
//
//Operation operation = Operation::None;
//Direction direction = Direction::None;
//bool printInfo = false;
//
//GLfloat const step = 0.1f;
//vec3 translateVector1 = vec3(0, -30, 0);
//vec3 scaleVector1 = vec3(0.5f, 0.5f, 0.5f);
//vec3 rotateAngles1 = vec3(0, 0, 0);
//
//vec3 cameraTranslateVector = vec3(0, 0, 0);
//vec3 cameraRotateVector = vec3(0, 0, 0);
//
//#pragma region CUSTOM_FUNCTIONS
//
//void updateUniformVariables(mat4 model)
//{
//	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
//
//	mat4 normalsTransform = transpose(inverse(model));
//	glUniformMatrix4fv(normals_location, 1, GL_FALSE, normalsTransform.m);
//}
//
//void updateUniformVariables(mat4 model, mat4 view, mat4 persp_proj)
//{
//	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
//	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
//	updateUniformVariables(model);
//}
//
//void drawTeapot()
//{
//	//	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);
//}
//
//// Shader Functions- click on + to expand
//#pragma region SHADER_FUNCTIONS
//
//// Create a NULL-terminated string by reading the provided file
//char* readShaderSource(const char* shaderFile) {
//	FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?
//
//	if (fp == NULL) { return NULL; }
//
//	fseek(fp, 0L, SEEK_END);
//	long size = ftell(fp);
//
//	fseek(fp, 0L, SEEK_SET);
//	char* buf = new char[size + 1];
//	fread(buf, 1, size, fp);
//	buf[size] = '\0';
//
//	fclose(fp);
//
//	return buf;
//}
//
//
//static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
//{
//	// create a shader object
//	GLuint ShaderObj = glCreateShader(ShaderType);
//
//	if (ShaderObj == 0) {
//		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
//		exit(0);
//	}
//	const char* pShaderSource = readShaderSource(pShaderText);
//
//	// Bind the source code to the shader, this happens before compilation
//	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
//	// compile the shader and check for errors
//	glCompileShader(ShaderObj);
//	GLint success;
//	// check for shader related errors using glGetShaderiv
//	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		GLchar InfoLog[1024];
//		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
//		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
//		exit(1);
//	}
//	// Attach the compiled shader object to the program object
//	glAttachShader(ShaderProgram, ShaderObj);
//}
//
//GLuint CompileShaders()
//{
//	//Start the process of setting up our shaders by creating a program ID
//	//Note: we will link all the shaders together into this ID
//	shaderProgramID = glCreateProgram();
//	if (shaderProgramID == 0) {
//		fprintf(stderr, "Error creating shader program\n");
//		exit(1);
//	}
//
//	// Create two shader objects, one for the vertex, and one for the fragment shader
//	AddShader(shaderProgramID, "../Assignment1/Shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
//	AddShader(shaderProgramID, "../Assignment1/Shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);
//
//	GLint Success = 0;
//	GLchar ErrorLog[1024] = { 0 };
//	// After compiling all shader objects and attaching them to the program, we can finally link it
//	glLinkProgram(shaderProgramID);
//	// check for program related errors using glGetProgramiv
//	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
//	if (Success == 0) {
//		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
//		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
//		exit(1);
//	}
//
//	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
//	glValidateProgram(shaderProgramID);
//	// check for program related errors using glGetProgramiv
//	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
//	if (!Success) {
//		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
//		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
//		exit(1);
//	}
//	// Finally, use the linked shader program
//	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
//	glUseProgram(shaderProgramID);
//	return shaderProgramID;
//}
//#pragma endregion SHADER_FUNCTIONS
//
//// VBO Functions - click on + to expand
//#pragma region VBO_FUNCTIONS
//
//void addToObjectBuffer(int start, int n_vertices, float *vertices, int n_indices, unsigned int *indices)
//{
//	//glBufferSubData(GL_ARRAY_BUFFER, 0, n_vertices * 8 * sizeof(GLfloat), vertices);
//
//	// Vertex Attribute array	
//	glGenVertexArrays(1, &plane_vao);
//	glBindVertexArray(plane_vao);
//
//	glEnableVertexAttribArray(loc1);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), NULL);
//
//	glEnableVertexAttribArray(loc2);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(GLfloat)));
//
//	glEnableVertexAttribArray(loc3);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_TRUE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(GLfloat)));
//
//	// Create index buffer	
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
//
//}
//
//
//#pragma endregion VBO_FUNCTIONS
//
//
//void display() {
//
//	if (printInfo)
//	{
//		printf("Operation is %s\r\n", operation == Operation::Translate ? "Translate" : operation == Operation::Rotate ? "Rotate" : operation == Operation::Scale ? "Scale" : "None");
//		printf("Direction is %s\r\n", direction == Direction::X ? "X" : direction == Direction::Y ? "Y" : direction == Direction::Z ? "Z" : direction == Direction::Uniform ? "Uniform" : "None");
//		printInfo = false;
//	}
//
//	HWND hwnd = GetActiveWindow();
//	RECT rect;
//	if (GetWindowRect(hwnd, &rect))
//	{
//		width = rect.right - rect.left;
//		height = rect.bottom - rect.top;
//	}
//
//	float aspectRatio = (float)width / (float)height;
//
//	// tell GL to only draw onto a pixel if the shape is closer to the viewer
//	glEnable(GL_DEPTH_TEST); // enable depth-testing
//	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
//	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glUseProgram(shaderProgramID);
//
//	// Position the camera
//	vec3 cameraPosition = vec3(0.0 + cameraTranslateVector.v[0], 40.0 + cameraTranslateVector.v[1], 0.0 + cameraTranslateVector.v[2]);
//	mat4 view = look_at(cameraPosition, vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
//	mat4 persp_proj = perspective(40.0, aspectRatio, 0.1, 100.0);
//
//	mat4 local1 = identity_mat4();
//
//	updateUniformVariables(local1, view, persp_proj);
//
//	// GROUND
//	glBindVertexArray(plane_vao);
//	//glDrawArrays(GL_TRIANGLES, 0, ground.Mesh.Vertices.size()/3);
//	//plane.Draw();
//	cylinder.Draw();
//
//	glutSwapBuffers();
//}
//
//void updateScene() {
//
//	// Placeholder code, if you want to work with framerate
//	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
//	static DWORD  last_time = 0;
//	DWORD  curr_time = timeGetTime();
//	float  delta = (curr_time - last_time) * 0.001f;
//	if (delta > 0.03f)
//		delta = 0.03f;
//	last_time = curr_time;
//
//	rotatey += 0.2f;
//	rotatez += 0.2f;
//	// Draw the next frame
//	glutPostRedisplay();
//}
//
//
//void init()
//{
//	// Set up the shaders
//	GLuint shaderProgramID = CompileShaders();
//
//	int n_vbovertices = 0;
//	int n_ibovertices = 0;
//
//	//// Create plane where knight is standing
//	//plane = Assignment1::CGObject();
//	//objl::Vertex point1, point2, point3, point4 = objl::Vertex();
//	//point1.Position = objl::Vector3(-1.0f, 0.0f, -1.0f);			
//	//point2.Position = objl::Vector3(1.0f, 0.0f, -1.0f);			
//	//point3.Position = objl::Vector3(1.0f, 0.0f, 1.0f);			
//	//point4.Position = objl::Vector3(-1.0f, 0.0f, 1.0f);
//
//	//point1.Normal = point2.Normal = point3.Normal = point4.Normal = objl::Vector3(0.0f, 1.0f, 0.0f);
//	//point1.TextureCoordinate = point2.TextureCoordinate = point3.TextureCoordinate = point4.TextureCoordinate = objl::Vector2(0.013400, 0.997700);  //green
//	//
//	//std::vector<objl::Vertex> vertices = std::vector<objl::Vertex> { point1, point2, point3, point4 };
//	//std::vector<unsigned int> indices = std::vector<unsigned int>{ 0, 1, 2,
//	//															0, 2, 3 };
//	//plane.Mesh = objl::Mesh(vertices, indices);
//	//float *vertices_ptr_plane = &plane.Mesh.Vertices[0].Position.X;
//	//unsigned int *indices_ptr_plane = &plane.Mesh.Indices[0];	
//	//vboMeshes[0] = 0;  // keep track of where mesh starts in the buffer
//	//iboMeshes[0] = 0;  // keep track of where mesh starts in the buffer
//	//n_vbovertices += plane.Mesh.Vertices.size();
//	//n_ibovertices += plane.Mesh.Indices.size();
//
//	objl::Loader obj_loader;
//
//	// load meshes
//	const char* knightFileName = "../Assignment1/Mesh/Knight/Knight.obj";
//	const char* rootFileName = "../Assignment1/Mesh/Palm/palm.obj";
//	const char* cylinderFileName = "../Assignment1/Mesh/Cylinder/cylinder.obj";
//
//	//// Knight
//	//bool result = obj_loader.LoadFile(knightFileName);
//	//if (result)
//	//{	
//	//	float *vertices_ptr_knight = &obj_loader.LoadedMeshes[0].Vertices[0].Position.X;
//	//	unsigned int *indices_ptr_knight = &obj_loader.LoadedMeshes[0].Indices[0];		
//	//	knight = Assignment1::CGObject();
//	//	knight.Mesh = obj_loader.LoadedMeshes[0];
//	//	vboMeshes[1] = n_vbovertices;
//	//	iboMeshes[1] = n_ibovertices;
//	//	n_vbovertices += knight.Mesh.Vertices.size();
//	//	n_ibovertices += knight.Mesh.Indices.size();
//	//}
//
//	//// Root 	
//	//result = obj_loader.LoadFile(rootFileName);
//	//if (result)
//	//{		
//	//	float *vertices_ptr_root = &obj_loader.LoadedMeshes[0].Vertices[0].Position.X;
//	//	unsigned int *indices_ptr_root = &obj_loader.LoadedMeshes[0].Indices[0];		
//	//	root = Assignment1::CGObject();
//	//	root.Mesh = obj_loader.LoadedMeshes[0];
//	//	vboMeshes[2] = n_vbovertices;
//	//	iboMeshes[2] = n_ibovertices;
//	//	n_vbovertices += root.Mesh.Vertices.size();
//	//	n_ibovertices += root.Mesh.Indices.size();
//	//}
//
//	// 8 Cylinders
//	float *vertices_ptr_cylinder = NULL;
//	unsigned int *indices_ptr_cylinder = NULL;
//	bool result = obj_loader.LoadFile(cylinderFileName);
//	if (result)
//	{
//		vertices_ptr_cylinder = &obj_loader.LoadedMeshes[0].Vertices[0].Position.X;
//		indices_ptr_cylinder = &obj_loader.LoadedMeshes[0].Indices[0];
//		cylinder = Assignment1::CGObject();
//		cylinder.Mesh = obj_loader.LoadedMeshes[0];
//		vboMeshes[3] = n_vbovertices;
//		iboMeshes[3] = n_ibovertices;
//		n_vbovertices += cylinder.Mesh.Vertices.size();
//		n_ibovertices += cylinder.Mesh.Indices.size();
//	}
//
//	//Declare your uniform variables that will be used in your shader
//	matrix_location = glGetUniformLocation(shaderProgramID, "model");
//	view_mat_location = glGetUniformLocation(shaderProgramID, "view");
//	proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");
//	normals_location = glGetUniformLocation(shaderProgramID, "normals");
//
//	// Create vertex buffer & locations
//	glGenBuffers(1, &vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glGenBuffers(1, &ibo);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//
//	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
//	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normals");
//	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");
//
//	glBufferData(GL_ARRAY_BUFFER, n_vbovertices * 8 * sizeof(float), vertices_ptr_cylinder, GL_STATIC_DRAW);  // Vertex contains 8 floats: position (vec3), normal (vec3), texture (vec2)
//	GLenum error = glGetError();
//	if (error != -1)
//		return;
//
//	// Add all 4 objects to buffers
//	//addToObjectBuffer(0, plane.Mesh.Vertices.size(), vertices_ptr_plane, plane.Mesh.Indices.size(), indices_ptr_plane);
//	//addToObjectBuffer(obj_loader.LoadedMeshes[0].Vertices.size(), vertices_ptr, obj_loader.LoadedMeshes[0].Indices.size(), indices_ptr);
//	addToObjectBuffer(0, cylinder.Mesh.Vertices.size(), vertices_ptr_cylinder, cylinder.Mesh.Indices.size(), indices_ptr_cylinder);
//
//	operation = Operation::Translate;
//	direction = Direction::X;
//}
//
//// Placeholder code for the keypress
//void keypress(unsigned char key, int x, int y) {
//
//	switch (key) {
//	case 'r':
//		operation = Operation::Rotate;
//		direction = Direction::X;
//		printInfo = true;
//		break;
//	case 't':
//		operation = Operation::Translate;
//		direction = Direction::X;
//		printInfo = true;
//		break;
//	case 's':
//		operation = Operation::Scale;
//		direction = Direction::X;
//		printInfo = true;
//		break;
//	case 'u':
//		operation = Operation::Scale;
//		direction = Direction::Uniform;
//		printInfo = true;
//		break;
//	case 'x':
//		direction = Direction::X;
//		printInfo = true;
//		break;
//	case 'y':
//		direction = Direction::Y;
//		printInfo = true;
//		break;
//	case 'z':
//		direction = Direction::Z;
//		printInfo = true;
//		break;
//	default:
//		break;
//	}
//
//}
//
//void specialKeys(int key, int x, int y)
//{
//	switch (key)
//	{
//	case GLUT_KEY_UP:
//		switch (operation)
//		{
//		case (Operation::Translate):
//			switch (direction)
//			{
//			case (Direction::Z):
//				cameraTranslateVector.v[2]++;
//				break;
//			default:
//				// Assume Y- axis unless Z is selected
//				cameraTranslateVector.v[1]++;
//				break;
//			}
//			break;
//		case (Operation::Scale):
//			switch (direction)
//			{
//			case (Direction::Z):
//				scaleVector1.v[2] += step;
//				break;
//			case (Direction::Uniform):
//				scaleVector1.v[0] += step;
//				scaleVector1.v[1] += step;
//				scaleVector1.v[2] += step;
//				break;
//			default:
//				// Assume Y-scaling unless  Z or Uniform are specified
//				scaleVector1.v[1] += step;
//				break;
//			}
//			break;
//		case (Operation::Rotate):
//			switch (direction)
//			{
//			case (Direction::Z):
//				rotateAngles1.v[2]++;
//				break;
//			default:
//				// Assume rotation around Y-axis unless Z is specified
//				rotateAngles1.v[1]++;
//				break;
//			}
//			break;
//		case(Operation::None):
//		default:
//			break;
//		}
//		break;
//	case GLUT_KEY_DOWN:
//		switch (operation)
//		{
//		case (Operation::Translate):
//			switch (direction)
//			{
//			case (Direction::Z):
//				cameraTranslateVector.v[2]--;
//				break;
//			default:
//				// Assume Y-axis
//				cameraTranslateVector.v[1]--;
//				break;
//			}
//			break;
//		case (Operation::Scale):
//			switch (direction)
//			{
//			case (Direction::Z):
//				scaleVector1.v[2] -= step;
//				break;
//			case (Direction::Uniform):
//				scaleVector1.v[0] -= step;
//				scaleVector1.v[1] -= step;
//				scaleVector1.v[2] -= step;
//				break;
//			default:
//				// Assume y-axis
//				scaleVector1.v[1] -= step;
//				break;
//			}
//			break;
//		case (Operation::Rotate):
//			switch (direction)
//			{
//			case (Direction::Z):
//				rotateAngles1.v[2]--;
//				break;
//			default:
//				// Assume Y axis
//				rotateAngles1.v[1]--;
//				break;
//			}
//			break;
//		default:
//			break;
//		}
//		break;
//	case GLUT_KEY_LEFT:
//		// Assume operation is for X-axis
//		switch (operation)
//		{
//		case (Operation::Scale):
//			scaleVector1.v[0] -= step;
//			break;
//		case (Operation::Rotate):
//			rotateAngles1.v[0]--;
//			break;
//		default:
//			cameraTranslateVector.v[0]--;
//			break;
//		}
//		break;
//	case GLUT_KEY_RIGHT:
//		// Assume operation is for X-axis
//		switch (operation)
//		{
//		case (Operation::Scale):
//			scaleVector1.v[0] += step;
//			break;
//		case (Operation::Rotate):
//			rotateAngles1.v[0]++;
//			break;
//		default:
//			// Assume translate
//			cameraTranslateVector.v[0]++;
//			break;
//		}
//		break;
//	default:
//		break;
//	}
//}
//
//int main(int argc, char** argv) {
//
//	// Set up the window
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//	glutInitWindowSize(width, height);
//	glutCreateWindow("Knight");
//
//	// Tell glut where the display function is
//	glutDisplayFunc(display);
//	glutIdleFunc(updateScene);
//	glutKeyboardFunc(keypress);
//	glutSpecialFunc(specialKeys);
//
//	// A call to glewInit() must be done after glut is initialized!
//	GLenum res = glewInit();
//	// Check for any errors
//	if (res != GLEW_OK) {
//		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
//		return 1;
//	}
//
//	// Set up your objects and shaders
//	init();
//
//	// Begin infinite event loop
//	glutMainLoop();
//	return 0;
//}
//
//
//
//
//
//
//
//
//
//
//
