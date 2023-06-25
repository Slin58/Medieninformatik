#define _USE_MATH_DEFINES
#define FREEGLUT_STATIC
#include "ObjectGenerator.h"
#include <cmath>
#include <math.h>
#include <iostream>

#include <Eigen/Dense>

// COORDINATE FORMAT: x,y,z,w,nx,ny,nz,tx,ty
namespace ObjectGenerator {
	static Vertex getNormedVertex(GLfloat x, GLfloat y, GLfloat z) {
		GLfloat length = std::sqrtf(std::powf(x, 2) + std::powf(y, 2) + std::powf(z, 2));
		x /= length;
		y /= length;
		z /= length;
		return { x,y,z };
	}

	static void norm(Vertex& v) {
		GLfloat length = std::sqrtf(std::powf(v.x, 2) + std::powf(v.y, 2) + std::powf(v.z, 2));
		v.x /= length;
		v.y /= length;
		v.z /= length;
	}

	static void normVertices(Face& f) {
		norm(f.a);
		norm(f.b);
		norm(f.c);
	}

	static void normAllFaces(std::vector<Face>& faces) {
		for (Face f : faces) {
			normVertices(f);
		}
	}

	static Vertex getMiddleBetweenVertices(Vertex& const a, Vertex& const b) {
		return { a.x + (b.x - a.x) / 2, a.y + (b.y - a.y) / 2, a.z + (b.z - a.z) / 2 };
	}

	static void getFaceNormal(Face& const f, GLfloat& vx, GLfloat& vy, GLfloat& vz) {
		Eigen::Vector3f start;
		Eigen::Vector3f second;
		Eigen::Vector3f end;
		// calculate normal
		start = { f.a.x, f.a.y, f.a.z };
		second = { f.b.x, f.b.y, f.b.z };
		end = { f.c.x, f.c.y, f.c.z };
		second -= start;
		end -= start;
		start = second.cross(end);
		vx = start[0];
		vy = start[1];
		vz = start[2];
	}

	static Vertex addTex(Vertex& vx, GLfloat u, GLfloat v) {
		vx.u = u;
		vx.v = v;
		return vx;
	}

	static 	std::vector<Face> icosahedron() {
		std::vector<Face> result;

		float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
		float a = 1.0f;
		float b = 1.0f / phi;

		// add vertices
		Vertex v1 = getNormedVertex(0, b, -a);
		Vertex v2 = getNormedVertex(b, a, 0);
		Vertex v3 = getNormedVertex(-b, a, 0);
		Vertex v4 = getNormedVertex(0, b, a);
		Vertex v5 = getNormedVertex(0, -b, a);
		Vertex v6 = getNormedVertex(-a, 0, b);
		Vertex v7 = getNormedVertex(0, -b, -a);
		Vertex v8 = getNormedVertex(a, 0, -b);
		Vertex v9 = getNormedVertex(a, 0, b);
		Vertex v10 = getNormedVertex(-a, 0, -b);
		Vertex v11 = getNormedVertex(b, -a, 0);
		Vertex v12 = getNormedVertex(-b, -a, 0);

		// add triangles
		result.push_back(Face(v3, v2, v1));
		result.push_back(Face(v2, v3, v4));
		result.push_back(Face(v6, v5, v4));
		result.push_back(Face(v5, v9, v4));
		result.push_back(Face(v8, v7, v1));
		result.push_back(Face(v7, v10, v1));
		result.push_back(Face(v12, v11, v5));
		result.push_back(Face(v11, v12, v7));
		result.push_back(Face(v10, v6, v3));
		result.push_back(Face(v6, v10, v12));
		result.push_back(Face(v9, v8, v2));
		result.push_back(Face(v8, v9, v11));
		result.push_back(Face(v3, v6, v4));
		result.push_back(Face(v9, v2, v4));
		result.push_back(Face(v10, v3, v1));
		result.push_back(Face(v2, v8, v1));
		result.push_back(Face(v12, v10, v7));
		result.push_back(Face(v8, v11, v7));
		result.push_back(Face(v6, v12, v5));
		result.push_back(Face(v11, v9, v5));

		return result;
	}

	static void getIcoVertexUV(Vertex const& vx, GLfloat& u, GLfloat& v) {
		u = (std::atan2f(vx.z, vx.x) / (2.f * M_PI)) + 0.5f;
		v = (std::asin(vx.y) / M_PI) + 0.5f;
	}

	std::vector<GLfloat> ObjectGenerator::generateSphere(GLfloat radius, GLint detail)
	{
		std::vector<Face> curr = icosahedron();
		std::vector<Face> subdivided;
		for (GLint i = 0; i < detail; i++) {
			subdivided = std::vector<Face>();
			for (Face f : curr) {
				Vertex aToB = getMiddleBetweenVertices(f.a, f.b);
				Vertex bToC = getMiddleBetweenVertices(f.b, f.c);
				Vertex cToA = getMiddleBetweenVertices(f.c, f.a);
				norm(aToB); norm(bToC); norm(cToA);
				subdivided.push_back({ f.a, aToB, cToA });
				subdivided.push_back({ aToB, bToC, cToA });
				subdivided.push_back({ aToB, f.b, bToC });
				subdivided.push_back({ bToC, f.c, cToA });
			}
			curr = subdivided;
		}
		std::vector<GLfloat> result;
		GLfloat nx, ny, nz, u, v;
		for (Face f : curr) {
			getFaceNormal(f, nx, ny, nz);
			getIcoVertexUV(f.a, u, v);
			result.insert(result.end(), { f.a.x * radius, f.a.y * radius, f.a.z * radius, 1.0, nx, ny, nz, u, v });
			getIcoVertexUV(f.b, u, v);
			result.insert(result.end(), { f.b.x * radius, f.b.y * radius, f.b.z * radius, 1.0, nx, ny, nz, u, v });
			getIcoVertexUV(f.c, u, v);
			result.insert(result.end(), { f.c.x * radius, f.c.y * radius, f.c.z * radius, 1.0, nx, ny, nz, u, v });
		}
		return result;
	}

	std::vector<GLfloat> ObjectGenerator::generateTorus(GLfloat inner, GLfloat  outer, GLint detail)
	{
		return std::vector<GLfloat>();
	}
}
