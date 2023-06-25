// Wavefront Parser

#define FREEGLUT_STATIC

#include "WavefrontParser.hpp"
#include <fstream>
#include <sstream>

#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)
#include <Eigen/Dense>
#include <iostream>

namespace Wavefront {
	enum LineType {
		eObject,
		eGroup,
		eVertex,
		eTextureCoord,
		eVertexNormal,
		eFace,
		eSmoothing,
		eUnknown,
	};

	LineType strToType(std::string const& inString) {
		if (inString == "o") return eObject;
		if (inString == "g") return eGroup;
		if (inString == "v") return eVertex;
		if (inString == "vt") return eTextureCoord;
		if (inString == "vn") return eVertexNormal;
		if (inString == "f") return eFace;
		if (inString == "s") return eSmoothing;
		return eUnknown;
	}

	// inefficient
	void clearDelimiterFromStringBeginning(std::string& input, std::string const& delimiter) {
		size_t pos = 0;
		while ((pos = input.find(delimiter)) == 0) {
			input.erase(0, delimiter.length());
		}
	}

	// return part of string until delimiter and remove it from the string
	std::string eatUntilDelimiter(std::string& input, std::string const& delimiter) {
		size_t pos = 0;
		std::string token;
		if ((pos = input.find(delimiter)) != std::string::npos) {
			token = input.substr(0, pos);
			input.erase(0, pos + delimiter.length());
		}
		else {
			token = input;
			input = "";
		}
		clearDelimiterFromStringBeginning(input, delimiter);
		return token;
	}

	WavefrontParser::WavefrontParser() {
		this->objects = std::vector<WavefrontObject>();
		this->vertices = std::vector<WavefrontVertex>();
		this->textureCoords = std::vector<WavefrontTextureCoordinates>();
		this->vertexNormals = std::vector<WavefrontVertexNormal>();
	}

	bool WavefrontParser::parse(std::string const& fname) {
		this->objects = std::vector<WavefrontObject>();
		this->vertices = std::vector<WavefrontVertex>();
		this->textureCoords = std::vector<WavefrontTextureCoordinates>();
		this->vertexNormals = std::vector<WavefrontVertexNormal>();

		bool isSmooth = false;

		// variables to store stuff in
		WavefrontObject object;
		WavefrontPolygonGroup group;
		WavefrontVertex vertex;
		WavefrontTextureCoordinates textureCoord;
		WavefrontVertexNormal vertexNormal;
		WavefrontFace face;
		std::string text;
		std::string text2;
		float f;

		std::ifstream input(fname);
		if (!input.bad()) {
			for (std::string line; std::getline(input, line);) {
				switch (strToType(eatUntilDelimiter(line, " "))) {
				case eObject:
					object = WavefrontObject();
					object.name = eatUntilDelimiter(line, " ");
					this->objects.push_back(object);
					break;
				case eGroup:
					if (this->objects.size() == 0) {
						this->objects.push_back(WavefrontObject());
					}
					group = WavefrontPolygonGroup();
					group.name = eatUntilDelimiter(line, " ");
					this->objects.back().polygonGroups.push_back(group);
					break;
				case eVertex:
					vertex = {};
					vertex.x = std::stof(eatUntilDelimiter(line, " "));
					vertex.y = std::stof(eatUntilDelimiter(line, " "));
					vertex.z = std::stof(eatUntilDelimiter(line, " "));
					vertex.w = 1.0;
					if ((text = eatUntilDelimiter(line, " ")) != "") {
						f = std::stof(text);
						vertex.x /= f;
						vertex.y /= f;
						vertex.z /= f;
					}
					this->vertices.push_back(vertex);
					break;
				case eTextureCoord:
					textureCoord = {};
					textureCoord.u = std::stof(eatUntilDelimiter(line, " "));
					if ((text = eatUntilDelimiter(line, " ")) != "") {
						textureCoord.v = std::stof(text);
					}
					else {
						textureCoord.v = 0;
					}
					if ((text = eatUntilDelimiter(line, " ")) != "") {
						textureCoord.w = std::stof(text);
					}
					else {
						textureCoord.w = 0;
					}
					this->textureCoords.push_back(textureCoord);
					break;
				case eVertexNormal:
					vertexNormal = {};
					vertexNormal.x = std::stof(eatUntilDelimiter(line, " "));
					vertexNormal.y = std::stof(eatUntilDelimiter(line, " "));
					vertexNormal.z = std::stof(eatUntilDelimiter(line, " "));
					this->vertexNormals.push_back(vertexNormal);
					break;
				case eFace:
					if (this->objects.size() == 0) {
						this->objects.push_back(WavefrontObject());
					}
					if (this->objects.back().polygonGroups.size() == 0) {
						this->objects.back().polygonGroups.push_back(WavefrontPolygonGroup());
					}
					face = WavefrontFace();
					face.isSmooth = isSmooth;
					while ((text = eatUntilDelimiter(line, " ")) != "") {
						face.vertices.push_back(std::stoi(eatUntilDelimiter(text, "/")));
						text2 = eatUntilDelimiter(text, "/");
						if (text2 != "") {
							face.coords.push_back(std::stoi(text2));
						}
						// remaining text is vertex normal or nothing
						if (text != "") {
							face.normals.push_back(std::stoi(text));
						}
					}
					this->objects.back().polygonGroups.back().wavefrontFaces.push_back(face);
					break;
				case eSmoothing:
					isSmooth = eatUntilDelimiter(line, " ") != "off";
					break;
				}
			}
		}
		std::cout << "DONE PARSING" << std::endl;
		return true;
	}

	void WavefrontParser::render() {
		WavefrontVertex v;
		WavefrontVertexNormal vn;
		WavefrontTextureCoordinates vt;
		int i;
		bool hasNormal;
		Eigen::Vector3f start;
		Eigen::Vector3f second;
		Eigen::Vector3f end;
		glMatrixMode(GL_MODELVIEW);
		for (WavefrontObject o : this->objects) {
			for (WavefrontPolygonGroup g : o.polygonGroups) {
				for (WavefrontFace f : g.wavefrontFaces) {
					hasNormal = f.normals.size() != 0;
					if (!hasNormal) {
						// calculate normal
						v = this->vertices[f.vertices[0] - 1];
						start = { v.x, v.y, v.z };
						v = this->vertices[f.vertices[1] - 1];
						second = { v.x, v.y, v.z };
						v = this->vertices[f.vertices.back() - 1];
						end = { v.x, v.y, v.z };
						second -= start;
						end -= start;
						start = second.cross(end);
						// start = Eigen::dotProsecond * end;
					}

					glBegin(GL_TRIANGLES);
					for (i = 0; i < f.vertices.size(); i++) {
						if (hasNormal) {
							// normals/verts/such in wavefront files are 1-indexed.
							vn = this->vertexNormals[f.normals[i] - 1];
							glNormal3f(vn.x, vn.y, vn.z);
							glColor3f(0.5 + 0.5 * vn.x, 0.5 + 0.5 * vn.y, 0.5 + 0.5 * vn.z);
						}
						else {
							glNormal3f(start[0], start[1], start[2]);
							glColor3f(0.5 + 0.5 * start[0], 0.5 + 0.5 * start[1], 0.5 + 0.5 * start[2]);
						}
						v = this->vertices[f.vertices[i] - 1];
						glVertex3f(v.x, v.y, v.z);
					}
					glEnd();
				}
			}
		}
	}

	std::vector<GLfloat> WavefrontParser::exportAsArray()
	{
		std::vector<GLfloat> result;

		WavefrontVertex v;
		WavefrontVertexNormal vn;
		WavefrontTextureCoordinates vt;
		int i;
		bool hasNormal, hasTexCoords;
		Eigen::Vector3f start;
		Eigen::Vector3f second;
		Eigen::Vector3f end;
		for (WavefrontObject o : this->objects) {
			for (WavefrontPolygonGroup g : o.polygonGroups) {
				for (WavefrontFace f : g.wavefrontFaces) {
					hasNormal = f.normals.size() != 0;
					hasTexCoords = f.coords.size() != 0;
					if (!hasNormal) {
						// calculate normal
						v = this->vertices[f.vertices[0] - 1];
						start = { v.x, v.y, v.z };
						v = this->vertices[f.vertices[1] - 1];
						second = { v.x, v.y, v.z };
						v = this->vertices[f.vertices.back() - 1];
						end = { v.x, v.y, v.z };
						second -= start;
						end -= start;
						start = second.cross(end);
					}

					for (i = 0; i < f.vertices.size(); i++) {
						if (hasNormal) {
							// normals/verts/such in wavefront files are 1-indexed.
							vn = this->vertexNormals[f.normals[i] - 1];
						}
						else {
							vn = { start[0], start[1], start[2] };
						}
						if (hasTexCoords) {
							vt = this->textureCoords[f.coords[i] - 1];
						}
						else {
							vt = { 0.f, 0.f };
						}
						v = this->vertices[f.vertices[i] - 1];
						result.insert(result.end(), { v.x, v.y, v.z, v.w, vn.x, vn.y, vn.z, vt.u, vt.v });
					}
				}
			}
		}
		return result;
	}
}