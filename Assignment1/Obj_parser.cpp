// Initial code source: https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
// and http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// Updated to use maths_funcs objects

#include "Obj_parser.h"

Obj_parser::Obj_parser()
{
}

Obj_parser::~Obj_parser()
{
}

void Obj_parser::load_obj(const char* filename, std::vector<vec4> &vertices, std::vector<vec3> &normals, std::vector<GLushort> &elements)
{
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl; exit(1);
	}

	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream s(line.substr(2));
			vec4 v; s >> v.v[0]; s >> v.v[1]; s >> v.v[2]; v.v[3] = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream s(line.substr(2));
			std::string a, b, c;
			s >> a; s >> b; s >> c;
			//a--; b--; c--;
			//elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
		else if (line[0] == '#')
		{
			/* ignoring this line */
		}
		else
		{
			/* ignoring this line */
		}
	}

	//normals.resize(vertices.size(), vec3(0.0, 0.0, 0.0));
	//for (int i = 0; i < elements.size(); i += 3)
	//{
	//	GLushort ia = elements[i];
	//	GLushort ib = elements[i + 1];
	//	GLushort ic = elements[i + 2];
	//	vec3 normal = glm::normalize(glm::cross(
	//		glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
	//		glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
	//	normals[ia] = normals[ib] = normals[ic] = normal;
	//}
}

bool Obj_parser::load_obj2(const char * path,
	std::vector < vec3 > & out_vertices,
	//std::vector < vec2 > & out_uvs,
	std::vector < vec3 > & out_normals)
{
	std::vector< GLushort > vertexIndices;
	////std::vector< unsigned int > uvIndices;
	std::vector< GLushort > normalIndices;
	std::vector< vec3 > temp_vertices;
	//std::vector< vec2 > temp_uvs;
	std::vector< vec3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.v[0], &vertex.v[1], &vertex.v[2]);
			temp_vertices.push_back(vertex);
		}
		/*else if (strcmp(lineHeader, "vt") == 0) {
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.v[0], &uv.v[1]);
			temp_uvs.push_back(uv);
		}*/
		else if (strcmp(lineHeader, "vn") == 0) {
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.v[0], &normal.v[1], &normal.v[2]);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			unsigned int uvIndex[3];
			unsigned int normalIndex[3];

			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back((GLushort)vertexIndex[0]);
			vertexIndices.push_back((GLushort)vertexIndex[1]);
			vertexIndices.push_back((GLushort)vertexIndex[2]);
			/*uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);*/
			normalIndices.push_back((GLushort)normalIndex[0]);
			normalIndices.push_back((GLushort)normalIndex[1]);
			normalIndices.push_back((GLushort)normalIndex[2]);
		}		
	}

	// renumber the indexes - minus 1
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];

		vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];

		vec3 vertex = temp_normals[normalIndex - 1];
		out_normals.push_back(vertex);
	}
}

