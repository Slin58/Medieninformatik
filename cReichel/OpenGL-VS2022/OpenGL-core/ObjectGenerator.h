#pragma once
#define FREEGLUT_STATIC
#include <vector>
#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)
namespace ObjectGenerator {
	struct Vertex {
		GLfloat x, y, z, u, v;
		Vertex() {
			this->x = 0; this->y = 0; this->z = 0; this->u = 0; this->v = 0;
		}
		Vertex(GLfloat x, GLfloat y, GLfloat z) {
			this->x = x; this->y = y; this->z = z; this->u = 0; this->v = 0;
		}
	};

	struct Face {
		Vertex a, b, c;
		Face(Vertex a, Vertex b, Vertex c) {
			this->a = a, this->b = b; this->c = c;
		}
	};

	class ObjectGenerator
	{
	public:
		// std::vector<GLfloat> generateCube(GLfloat scale);
		static std::vector<GLfloat> generateSphere(GLfloat radius, GLint detail);
		static std::vector<GLfloat> generateTorus(GLfloat inner, GLfloat outer, GLint detail);
	};
}
