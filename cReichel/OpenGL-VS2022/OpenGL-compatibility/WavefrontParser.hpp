#pragma once
#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)
#include <string>
#include <vector>

namespace Wavefront {

	struct WavefrontVertex {
		float x, y, z, w;
	};

	struct WavefrontTextureCoordinates {
		float u, v, w;
	};

	struct WavefrontVertexNormal {
		float x, y, z;
	};

	struct WavefrontFace {
		std::vector<int> vertices;
		std::vector<int> coords;
		std::vector<int> normals;
		bool isSmooth;
		WavefrontFace() {
			this->vertices = std::vector<int>();
			this->coords = std::vector<int>();
			this->normals = std::vector<int>();
			isSmooth = false;
		}
	};

	struct WavefrontPolygonGroup {
		std::vector<WavefrontFace> wavefrontFaces;
		std::string name;
		WavefrontPolygonGroup() {
			this->wavefrontFaces = std::vector<WavefrontFace>();
			this->name = "unnamed polygon group";
		}
	};

	struct WavefrontObject {
		std::vector<WavefrontPolygonGroup> polygonGroups;
		std::string name;
		WavefrontObject() {
			this->polygonGroups = std::vector<WavefrontPolygonGroup>();
			this->name = "unnamed object";
		}
	};

	class WavefrontParser {

	public:
		WavefrontParser();
		bool parse(std::string const &fname);
		void render();
		std::vector<GLfloat> exportAsArray();
	protected:
		std::vector<WavefrontObject> objects;
		std::vector<WavefrontVertex> vertices;
		std::vector<WavefrontTextureCoordinates> textureCoords;
		std::vector<WavefrontVertexNormal> vertexNormals;
	}; // end class

}; // close namespace
