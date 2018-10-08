
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

// Vertex Shader (for convenience, it is defined in the main here, but we will be using text files for shaders in future)
// Note: Input to this shader is the vertex positions that we specified for the triangle. 
// Note: gl_Position is a special built-in variable that is supposed to contain the vertex position (in X, Y, Z, W)
// Since our triangle vertices were specified as vec3, we just set W to 1.0.

static const char* pVS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
in vec3 vPosition;															  \n\
in vec4 vColor;																  \n\
out vec4 color;																 \n\
                                                                              \n\
                                                                               \n\
void main()                                                                     \n\
{                                                                                \n\
    gl_Position = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);  \n\
	color = vColor;							\n\
}";

// Vertex Shader (for convenience, it is defined in the main here, but we will be using text files for shaders in future)
// Note: Input to this shader is the vertex positions that we specified for the triangle. 
// Note: gl_Position is a special built-in variable that is supposed to contain the vertex position (in X, Y, Z, W)
// Since our triangle vertices were specified as vec3, we just set W to 1.0.

static const char* pVS2 = "                                                    \n\
#version 330                                                                  \n\
																				\n\
in vec3 vPosition;															  \n\
in vec4 vColor;																  \n\
out vec4 color;																 \n\
                                                                              \n\
uniform mat4 gScaleMatrix;													\n\
uniform float gRatio;															  \n\
                                                                               \n\
void main()                                                                     \n\
{                                                                               \n\
	gl_Position = gScaleMatrix * vec4(gRatio * vPosition.x, vPosition.y, vPosition.z, 1.0);				\n\
	color = vColor;																\n\
}";


//if (gl_InstanceID = 1)															\n\
//{	
//\n\

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

GLuint VBO;
GLuint IBO;
GLuint shaderProgramID;
GLuint gScaleMatrixLocation;
GLuint gRatio;
GLfloat *vertices;

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
	AddShader(shaderProgramID, pVS2, GL_VERTEX_SHADER);
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
	gScaleMatrixLocation = glGetUniformLocation(shaderProgramID, "gScaleMatrix");
	gRatio = glGetUniformLocation(shaderProgramID, "gRatio");

	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
GLuint generateObjectBuffer(GLfloat vertices[], GLfloat colors[]) {
	GLuint numVertices = 4;
	//GLuint numVertices = 4 * 10;

	// Genderate 1 generic buffer object, called VBO
	glGenBuffers(1, &VBO);

	// In OpenGL, we bind (make active) the handle to a target name and then execute commands on that target
	// Buffer will contain an array of vertices 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//// After binding, we now fill our object with data, everything in "Vertices" goes to the GPU
	glBufferData(GL_ARRAY_BUFFER, numVertices * 7 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	//// if you have more data besides vertices (e.g., vertex colours or normals), use glBufferSubData to tell the buffer when the vertices array ends and when the colors start
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * 3 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), numVertices * 4 * sizeof(GLfloat), colors);

	return VBO;
}

static void CreateIndexBuffer(unsigned int indices[])
{	
	GLuint numTriangles = 2;
	//GLuint numTriangles = 2 * 10;

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * numTriangles * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

void linkCurrentBuffertoShader(GLuint shaderProgramID) {
	GLuint numVertices = 4;
	//GLuint numVertices = 4 * 10;

	// find the location of the variables that we will be using in the shader program
	GLuint positionID = glGetAttribLocation(shaderProgramID, "vPosition");
	GLuint colorID = glGetAttribLocation(shaderProgramID, "vColor");

	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(colorID);

	// Have to enable this
	glEnableVertexAttribArray(positionID);
	// Tell it where to find the position data in the currently active buffer (at index positionID)
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Similarly, for the color data.
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(numVertices * 3 * sizeof(GLfloat)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
}
#pragma endregion VBO_FUNCTIONS


void display() {
	
	static GLuint xIncrement = 0;
	
	glClear(GL_COLOR_BUFFER_BIT);
	// NB: Make the call to draw the geometry in the currently activated vertex buffer. This is where the GPU starts to work!	

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glutSwapBuffers();

	//if (xIncrement < 100)
	//{
	//	xIncrement ++;
	//}
}


void init(GLfloat height, GLfloat width)
{	
	// Create 4 vertices that make up a square - start with triangle 
	GLfloat verticesArray[] = { -1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f };

	vertices = verticesArray;

	// Create a color array that identfies the colors of each vertex (format R, G, B, A)
	GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f };

	unsigned int indices[] = { 0, 1, 2,
		0, 2, 3 };

	////// Add vertices to array for each increment
	////GLfloat verticesArray[120];
	////GLfloat colors[160];
	////unsigned int indices[60];
	////for (int i = 0; i < 10; i++)
	////{
	////	verticesArray[12*i] = -1.0f; verticesArray[12 * i + 1] = -1.0f; verticesArray[12 * i + 2] = 0.0f;
	////	verticesArray[12*i + 3] = 1.0f; verticesArray[12 * i + 4] = -1.0f; verticesArray[12 * i + 5] = 0.0f;
	////	verticesArray[12*i + 6] = 1.0f; verticesArray[12 * i + 7] = 1.0f; verticesArray[12 * i + 8] = 0.0f;
	////	verticesArray[12*i + 9] = -1.0f; verticesArray[12 * i + 12] = 1.0f; verticesArray[12 * i + 11] = 0.0;
	////	
	////	colors[16 * i] = 1.0f; colors[16 * i + 1] = 0.0f; colors[16 * i + 2] = 0.0f; colors[16 * i + 3] = 1.0f;
	////	colors[16 * i + 4] = 1.0f; colors[16 * i + 5] = 0.0f; colors[16 * i + 6] = 0.0f; colors[16 * i + 7] = 1.0f;
	////	colors[16 * i + 8] = 1.0f; colors[16 * i + 9] = 1.0f; colors[16 * i + 10] = 0.0f; colors[16 * i + 11] = 1.0f;
	////	colors[16 * i + 12] = 1.0f; colors[16 * i + 13] = 1.0f; colors[16 * i + 14] = 0.0f; colors[16 * i + 15] = 1.0f;

	////	indices[6 * i] = 0; indices[6 * i + 1] = 1; indices[6 * i + 2] = 2;
	////	indices[6 * i + 3] = 0; indices[6 * i + 4] = 2; indices[6 * i + 5] = 3;
	////}

	////vertices = verticesArray;
	
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();

	//create global variable for the tranformation
	float scaleMatrix[4][4] = { 0.0f };

	scaleMatrix[0][0] = 0.5f;
	scaleMatrix[1][1] = 0.5f;
	scaleMatrix[2][2] = 0.5f;
	scaleMatrix[3][3] = 1.0f;
	glUniformMatrix4fv(gScaleMatrixLocation, 1, GL_TRUE, &scaleMatrix[0][0]);

	GLfloat ratio = height / width;
	glUniform1f(gRatio, ratio);

	// Put the vertices and colors into a vertex buffer object
	generateObjectBuffer(vertices, colors);
	CreateIndexBuffer(indices);
	// Link the current buffer to the shader
	linkCurrentBuffertoShader(shaderProgramID);
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	GLfloat height = 600.0;
	GLfloat width = 800.0;
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");
	// Tell glut where the display function is
	glutDisplayFunc(display);

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











