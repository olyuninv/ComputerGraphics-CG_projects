#include "maths_funcs.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtc/constants.hpp> 

#include <iostream>

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

enum class Direction
{
	// Here are the enumerators
	// These define all the possible values this type can hold
	// Each enumerator is separated by a comma, not a semicolon
	None,
	X,
	Y,
	Z,
	Uniform,
};

enum class Operation
{
	// Here are the enumerators
	// These define all the possible values this type can hold
	// Each enumerator is separated by a comma, not a semicolon
	None,
	Translate,
	Rotate,
	Scale,
};

using namespace std;

// Vertex Shader (for convenience, it is defined in the main here, but we will be using text files for shaders in future)
// Note: Input to this shader is the vertex positions that we specified for the triangle. 
// Note: gl_Position is a special built-in variable that is supposed to contain the vertex position (in X, Y, Z, W)
// Since our triangle vertices were specified as vec3, we just set W to 1.0.

static const char* pVS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
in vec3 vPosition;															  \n\
in vec4 vColor;															  \n\
																			\n\
out vec4 color;																 \n\
                                                                              \n\
uniform mat4 gTransformMatrix;													\n\
                                                                               \n\
void main()                                                                     \n\
{                                                                                \n\
    gl_Position =  gTransformMatrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);  \n\
	color = vColor;							\n\
}";

// Fragment Shader
// Note: no input in this shader, it just outputs the colour of all fragments, in this case set to red (format: R, G, B, A).
static const char* pFS = "                                              \n\
#version 330                                                            \n\
                                                                        \n\
in vec4 color;                                                      \n\
out vec4 FragColor;                                                      \n\
                                                                          \n\
void main()                                                               \n\
{                                                                          \n\
	FragColor = color;									 \n\
}";

GLuint shaderProgramID;
GLuint VBO;
GLuint IBO;
GLuint triangle1ArrayObjectID;
GLuint triangle2ArrayObjectID;

GLuint gTransformMatrixLocation;
int xIncrement = 0;
bool increment = true;

GLfloat const step = 0.01f;
glm::ivec3 translateVector1 = glm::ivec3(0, 0, 0);
glm::ivec3 scaleVector1 = glm::ivec3(0, 0, 0);
glm::ivec3 rotateAngles1 = glm::ivec3(0, 0, 0);

glm::ivec3 translateVector2 = glm::ivec3(-50, 0, 0);
glm::ivec3 scaleVector2 = glm::ivec3(0, 0, 0);
glm::ivec3 rotateAngles2 = glm::ivec3(0, 0, 0);

int meshes[2];

GLfloat screenRatio = 0.0f;

bool printInfo = false;
bool printVectorInfo = false;

Operation operation = Operation::None;
Direction direction = Direction::None;

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderText, NULL);
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
	AddShader(shaderProgramID, pVS, GL_VERTEX_SHADER);
	AddShader(shaderProgramID, pFS, GL_FRAGMENT_SHADER);

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

	// Provide uniform variables	
	gTransformMatrixLocation = glGetUniformLocation(shaderProgramID, "gTransformMatrix");

	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
GLuint generateObjectBuffer(int numVertices1, GLfloat vertices1[], GLfloat colors1[], int numVertices2, GLfloat vertices2[], GLfloat colors2[]) {

	// Genderate 1 generic buffer object, called VBO	
	glGenBuffers(1, &VBO);
	// In OpenGL, we bind (make active) the handle to a target name and then execute commands on that target
	// Buffer will contain an array of vertices 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// After binding, we now fill our object with data, everything in "Vertices" goes to the GPU
	glBufferData(GL_ARRAY_BUFFER, (numVertices1 + numVertices2) * 7 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	// if you have more data besides vertices (e.g., vertex colours or normals), use glBufferSubData to tell the buffer when the vertices array ends and when the colors start
	// 1st triangle
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices1 * 3 * sizeof(GLfloat), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, numVertices1 * 3 * sizeof(GLfloat), numVertices1 * 4 * sizeof(GLfloat), colors1);

	// 2nd triangle
	glBufferSubData(GL_ARRAY_BUFFER, numVertices1 * 7 * sizeof(GLfloat), numVertices2 * 3 * sizeof(GLfloat), vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, numVertices1 * 7 * sizeof(GLfloat) + numVertices2 * 3 * sizeof(GLfloat), numVertices2 * 4 * sizeof(GLfloat), colors2);
	
	return VBO;
}

static void CreateIndexBuffer()
{
	unsigned int Indices1[] = { 0, 1, 2 };
	unsigned int Indices2[] = { 0, 1, 2 };

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * (sizeof(Indices1) + sizeof(Indices2)), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Indices1) * 3, Indices1);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices1) * 3, sizeof(Indices2) * 3, Indices2);
}

void linkCurrentBuffertoShader(int numObjects, int objectIndex, int meshes[], GLuint shaderProgramID) {

	// find the location of the variables that we will be using in the shader program
	GLuint positionID = glGetAttribLocation(shaderProgramID, "vPosition");
	GLuint colorID = glGetAttribLocation(shaderProgramID, "vColor");

	if (objectIndex == 1)
	{	
		glBindVertexArray(triangle1ArrayObjectID);
		// Have to enable this
		glEnableVertexAttribArray(positionID);
		glEnableVertexAttribArray(colorID);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Tell it where to find the position data in the currently active buffer (at index positionID)
		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
		// Similarly, for the color data.
		glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(meshes[0] * 3 * sizeof(GLfloat)));
						
		//IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);	
	}
	else if (objectIndex == 2)
	{
		glBindVertexArray(triangle2ArrayObjectID);

		glEnableVertexAttribArray(positionID);
		glEnableVertexAttribArray(colorID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(meshes[0] * 7 * sizeof(GLfloat)));
		glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(meshes[0] * 7 * sizeof(GLfloat) + meshes[1] * 3 * sizeof(GLfloat)));
		
		//IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	}
}
#pragma endregion VBO_FUNCTIONS

void display() {
	if (printInfo)
	{
		printf("Operation is %s\r\n", operation == Operation::Translate ? "Translate" : operation == Operation::Rotate ? "Rotate" : operation == Operation::Scale ? "Scale" : "None");
		printf("Direction is %s\r\n", direction == Direction::X ? "X" : direction == Direction::Y ? "Y" : direction == Direction::Z ? "Z" : direction == Direction::Uniform ? "Uniform" : "None");
		printInfo = false;
	}

	glClear(GL_COLOR_BUFFER_BIT);

	// DRAW 1st TRIANGLE
	//glm::mat4x4 translateToOriginMatrix = glm::translate(glm::mat4(), glm::vec3(0.0, sqrt(5.0/ 3.0) - 1, 0.0));
	//glm::mat4x4 translateToSourceMatrix = glm::translate(glm::mat4(), glm::vec3(0.0, 1 - sqrt(5.0 / 3.0), 0.0));
	glm::mat4x4 scaleForWindowRatio = glm::scale(glm::mat4(), glm::vec3(screenRatio, 1, 1));

	glm::mat4x4 translateMatrix1 = glm::translate(glm::mat4(), glm::vec3(step * translateVector1[0], step * translateVector1[1], step * translateVector1[2]));
	glm::mat4x4 scaleMatrix1 = glm::scale(glm::mat4(), glm::vec3(1 + step * scaleVector1[0], 1 + step * scaleVector1[1], 1 + step * scaleVector1[2]));
	glm::mat4x4 rotationMatrixX1 = glm::mat4();
	glm::mat4x4 rotationMatrixY1 = glm::mat4();
	glm::mat4x4 rotationMatrixZ1 = glm::mat4();
	float rotate_x1 = (rotateAngles1[0] == 0) ? 0.0f : glm::pi<float>() / 180 * rotateAngles1[0];
	float rotate_y1 = (rotateAngles1[1] == 0) ? 0.0f : glm::pi<float>() / 180 * rotateAngles1[1];
	float rotate_z1 = (rotateAngles1[2] == 0) ? 0.0f : glm::pi<float>() / 180 * rotateAngles1[2];

	rotationMatrixX1[1][1] = rotationMatrixX1[2][2] = cos(rotate_x1);
	rotationMatrixX1[0][0] = rotationMatrixX1[3][3] = 1.0f;
	rotationMatrixX1[1][2] = sin(rotate_x1);
	rotationMatrixX1[2][1] = -rotationMatrixX1[1][2];

	rotationMatrixY1[0][0] = rotationMatrixY1[2][2] = cos(rotate_y1);
	rotationMatrixY1[1][1] = rotationMatrixY1[3][3] = 1.0f;
	rotationMatrixY1[0][2] = sin(rotate_y1);
	rotationMatrixY1[2][0] = -rotationMatrixY1[0][2];

	rotationMatrixZ1[0][0] = rotationMatrixZ1[1][1] = cos(rotate_z1);
	rotationMatrixZ1[2][2] = rotationMatrixZ1[3][3] = 1.0f;
	rotationMatrixZ1[0][1] = sin(rotate_z1);
	rotationMatrixZ1[1][0] = -rotationMatrixZ1[0][1];
		
	glm::mat4x4 transformMatrix1 = translateMatrix1 * scaleForWindowRatio * rotationMatrixX1 * rotationMatrixY1 * rotationMatrixZ1 * scaleMatrix1;

	glUniformMatrix4fv(gTransformMatrixLocation, 1, GL_FALSE, &transformMatrix1[0][0]);

	if (printVectorInfo)
	{
		printf("Translate vector is: [%.2f, %.2f, %.2f]\n", step * translateVector1[0], step * translateVector1[1], step * translateVector1[2]);
		printf("Scale vector is: [%.2f, %.2f, %.2f]\n", 1 + step * scaleVector1[0], 1 + step * scaleVector1[0], 1 + step * scaleVector1[0]);
		printf("Rotation angles are: [%i, %i, %i]\n", rotateAngles1[0], rotateAngles1[1], rotateAngles1[2]);

		printf("Transform matix is:\n");
		printf("[%.2f][%.2f][%.2f][%.2f]\n", transformMatrix1[0][0], transformMatrix1[1][0], transformMatrix1[2][0], transformMatrix1[3][0]);
		printf("[%.2f][%.2f][%.2f][%.2f]\n", transformMatrix1[0][1], transformMatrix1[1][1], transformMatrix1[2][1], transformMatrix1[3][1]);
		printf("[%.2f][%.2f][%.2f][%.2f]\n", transformMatrix1[0][2], transformMatrix1[1][2], transformMatrix1[2][2], transformMatrix1[3][2]);
		printf("[%.2f][%.2f][%.2f][%.2f]\n", transformMatrix1[0][3], transformMatrix1[1][3], transformMatrix1[2][3], transformMatrix1[3][3]);
		printVectorInfo = false;
	}
	glBindVertexArray(triangle1ArrayObjectID);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	//// TRIANGLE 2
	glm::mat4x4 translateMatrix2 = glm::translate(glm::mat4(), glm::vec3(step * translateVector2[0] + step * xIncrement, step * translateVector2[1], step * translateVector2[2]));
	glm::mat4x4 scaleMatrix2 = glm::scale(glm::mat4(), glm::vec3(screenRatio * (1 + step * scaleVector2[0]), 1 + step * scaleVector2[1], 1 + step * scaleVector2[2]));
	glm::mat4x4 rotationMatrixX2 = glm::mat4();
	glm::mat4x4 rotationMatrixY2 = glm::mat4();
	glm::mat4x4 rotationMatrixZ2 = glm::mat4();

	rotateAngles2[0] = xIncrement;

	float rotate_x2 = (rotateAngles2[0] == 0) ? 0.0f : glm::pi<float>() / 180 * rotateAngles2[0];
	float rotate_y2 = (rotateAngles2[1] == 0) ? 0.0f : glm::pi<float>() / 180 * rotateAngles2[1];
	float rotate_z2 = (rotateAngles2[2] == 0) ? 0.0f : glm::pi<float>() / 180 * rotateAngles2[2];

	rotationMatrixX2[1][1] = rotationMatrixX2[2][2] = cos(rotate_x2);
	rotationMatrixX2[0][0] = rotationMatrixX2[3][3] = 1.0f;
	rotationMatrixX2[1][2] = sin(rotate_x2);
	rotationMatrixX2[2][1] = -rotationMatrixX2[1][2];

	rotationMatrixY2[0][0] = rotationMatrixY2[2][2] = cos(rotate_y2);
	rotationMatrixY2[1][1] = rotationMatrixY2[3][3] = 1.0f;
	rotationMatrixY2[0][2] = sin(rotate_y2);
	rotationMatrixY2[2][0] = -rotationMatrixY2[0][2];

	rotationMatrixZ2[0][0] = rotationMatrixZ2[1][1] = cos(rotate_z2);
	rotationMatrixZ2[2][2] = rotationMatrixZ2[3][3] = 1.0f;
	rotationMatrixZ2[0][1] = sin(rotate_z2);
	rotationMatrixZ2[1][0] = -rotationMatrixZ2[0][1];

	glm::mat4x4 transformMatrix2 = translateMatrix2 * rotationMatrixX2 * rotationMatrixY2 * rotationMatrixZ2 * scaleMatrix2;
	//glm::mat4x4 transformMatrix2 = translateMatrix2 * rotationMatrixX2 * rotationMatrixZ2 * scaleMatrix2; 
	glUniformMatrix4fv(gTransformMatrixLocation, 1, GL_FALSE, &transformMatrix2[0][0]);

	glBindVertexArray(triangle2ArrayObjectID);
	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	linkCurrentBuffertoShader(2, 2, meshes, shaderProgramID);
		
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * 3 * sizeof(unsigned int)));
	
	glutSwapBuffers();

	if (xIncrement == 100)
	{
		increment = false;
		xIncrement--;
	}
	if (xIncrement == -100)
	{
		increment = true;
		xIncrement++;
	}
	else if (increment)
	{
		xIncrement++;		
	}
	else 
	{
		xIncrement--;
	}
}

void init(GLfloat height, GLfloat width)
{
	screenRatio = height / width;
	
	// Create 2 triangles
	GLfloat vertices1[] = {
		//TRIANGE 1
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f };

		//TRIANGLE 2
	GLfloat vertices2[] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f };
	
	// Create a color array that identfies the colors of each vertex (format R, G, B, A)
	GLfloat colors1[] = {
		//TRIANGE 1
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f };

		//TRIANGLE 2
	GLfloat colors2[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f 
	};
	
	// keep track of the number of vertices in the objects that we create
	meshes[0] = 3;
	meshes[1] = 3;
	
	glGenVertexArrays(1, &triangle1ArrayObjectID);
	glGenVertexArrays(1, &triangle2ArrayObjectID);

	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();

	// Put the vertices and colors into a vertex buffer object
	generateObjectBuffer(meshes[0], vertices1, colors1, meshes[1], vertices2, colors2);
	CreateIndexBuffer();

	// Link the current buffer to the shader
	linkCurrentBuffertoShader(sizeof(meshes)/sizeof(int), 1, meshes, shaderProgramID);
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(display);
	glutIdleFunc(display);
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'R':
	case 'r':
		operation = Operation::Rotate;
		printInfo = true;
		break;
	case 'T':
	case 't':
		operation = Operation::Translate;
		printInfo = true;
		break;
	case 'S':
	case 's':
		operation = Operation::Scale;
		printInfo = true;
		break;
	case 'X':
	case 'x':
		direction = Direction::X;
		printInfo = true;
		break;
	case 'Y':
	case 'y':
		direction = Direction::Y;
		printInfo = true;
		break;
	case 'Z':
	case 'z':
		direction = Direction::Z;
		printInfo = true;
		break;
	case 'U':
	case 'u':
		direction = Direction::Uniform;
		printInfo = true;
		break;
	case 'P':
	case 'p':
		printVectorInfo = true;
		break;
	default:
		break;
	}

	glutPostRedisplay(); /* this redraws the scene without waiting for the display callback so that any changes appear instantly */
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
			case (Direction::Y):
				translateVector1[1]++;
				break;
			case (Direction::Z):
				translateVector1[2]++;
				break;
			default:
				break;
			}
			break;
		case (Operation::Scale):
			switch (direction)
			{
			case (Direction::X):
				scaleVector1[0]++;
				break;
			case (Direction::Y):
				scaleVector1[1]++;
				break;
			case (Direction::Z):
				scaleVector1[2]++;
				break;
			case (Direction::Uniform):
				scaleVector1[0]++;
				scaleVector1[1]++;
				scaleVector1[2]++;
				break;
			default:
				break;
			}
			break;
		case (Operation::Rotate):
			switch (direction)
			{
			case (Direction::X):
				rotateAngles1[0]++;
				break;
			case (Direction::Y):
				rotateAngles1[1]++;
				break;
			case (Direction::Z):
				rotateAngles1[2]++;
				break;
			default:
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
			case (Direction::Y):
				translateVector1[1]--;
				break;
			case (Direction::Z):
				translateVector1[2]--;
				break;
			default:
				break;
			}
			break;
		case (Operation::Scale):
			switch (direction)
			{
			case (Direction::X):
				scaleVector1[0]--;
				break;
			case (Direction::Y):
				scaleVector1[1]--;
				break;
			case (Direction::Z):
				scaleVector1[2]--;
				break;
			case (Direction::Uniform):
				scaleVector1[0]--;
				scaleVector1[1]--;
				scaleVector1[2]--;
				break;
			default:
				break;
			}
			break;
		case (Operation::Rotate):
			switch (direction)
			{
			case (Direction::X):
				rotateAngles1[0]--;
				break;
			case (Direction::Y):
				rotateAngles1[1]--;
				break;
			case (Direction::Z):
				rotateAngles1[2]--;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
	case GLUT_KEY_LEFT:
		switch (operation)
		{
		case (Operation::Translate):
			switch (direction)
			{
			case (Direction::X):
				translateVector1[0]--;
				break;		
			default:
				break;
			}
		default:
			break;
		}
		break;
	case GLUT_KEY_RIGHT:
		switch (operation)
		{
		case (Operation::Translate):
			switch (direction)
			{
			case (Direction::X):
				translateVector1[0]++;
				break;
			default:
				break;
			}
		default:
			break;
		}
			break;
	default:
		break;
	}

	glutPostRedisplay(); /* this redraws the scene without waiting for the display callback so that any changes appear instantly */
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	
	GLfloat height = 600.0;
	GLfloat width = 800.0;
	glutInitWindowSize(width, height);

	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hello Triangle");
	// Tell glut where the display function is
	InitializeGlutCallbacks();
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init(height, width);
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}











