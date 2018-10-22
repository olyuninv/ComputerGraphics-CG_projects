#pragma once

// Initial code source: https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
// and http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// Updated to use maths_funcs objects

#include <ios>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "maths_funcs.h"

static class Obj_parser
{
public:	
	Obj_parser();
	~Obj_parser();
	static void load_obj(const char* filename, std::vector<vec4> &vertices, std::vector<vec3> &normals, std::vector<GLushort> &elements);

	static bool load_obj2(const char * path, std::vector < vec3 > & out_vertices, std::vector < vec3 > & out_normals);
};