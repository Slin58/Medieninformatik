/*


//
//  main.cpp
//  OpenGL
//

// IMPORTANT: use Homebrew to install glfw package
// brew install glfw
#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include <stdio.h>
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)

#include <math.h>--
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

#include <GLFW/glfw3.h>
#include "gl/gl.h"
#include <glm/ext.hpp>
#include "texture/wmap.h"
#include "texture/moonmap.h"
#include "texture/brick.h"
#include <glm/gtx/string_cast.hpp>
#include "WavefrontObject.hpp"
using namespace std;

GLFWwindow* window = nullptr;


struct Tblock {
    glm::mat4 transform;
    glm::mat4 look;
    glm::mat4 proj;
} Tblock;
const GLfloat vertexPositions[] = {
    0.75f, 0.75f, 0.0f, 1.0f,
    0.75f, -0.75f, 0.0f, 1.0f,
    -0.75f, -0.75f, 0.0f, 1.0f,

};


GLfloat tz = 0;
struct Tblock tblock;
glm::vec3 rot(0.0f);
glm::vec3 trans(0.0f);
GLuint positionBufferObject = 0;
GLint colorBufferObject = 0;
GLuint vertexBufferArray = 0;
GLuint colorBufferArray = 0;
GLuint theProgram = 0;
GLfloat fovy = 60.0f;
int triangleCount;
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);

const char vs1[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normalIn;
smooth out vec3 normal;
layout(std140) uniform TBlock {
mat4 transform;
mat4 look;
mat4 proj;
};

void main() {
position = normalize(position);
gl_Position = normalize(proj * look * transform * position);
normal = normalize(transpose(inverse(mat3(transform)))*normalIn);
}
)EOF";

const char vs2[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoord;
out vec3 pos;
out vec3 normal;
smooth out vec2 intTexCoord;
layout(std140) uniform TBlock {
mat4 transform;
mat4 look;
mat4 proj;
};

void main() {
intTexCoord = texCoord;
gl_Position = proj * look * transform * position;
pos = vec3(gl_Position[0], gl_Position[1], gl_Position[2]);
normal = normalIn;
}
)EOF";

const char fs1[] = R"EOF(
#version 330
out vec4 outColor;
smooth in vec2 intTexCoord;
uniform sampler2D tex;
in vec3 normal;
in vec3 pos;
void main(){
    outColor = texture(tex, intTexCoord);
}
)EOF";


const char fs2[] = R"EOF(
#version 330
out vec4 outColor;
in vec3 pos;

void main(){
    vec3 dx = dFdx( pos );
    vec3 dy = dFdy( pos );
    vec3 N = normalize(cross(dx, dy));
    N *= sign(N.z);
    vec3 L = vec3(0.0f, 0.0f, 1.0f);
    float NdotL = max(dot(N, L), 0.0f);
    outColor = NdotL*vec4( 1.0f, 1.0f, 1.0f, 1.0f );
}
)EOF";


const char fs3[] = R"EOF(
#version 330
varying vec3 normal, lightDir;
vec4 toonify(in float intensity) {
vec4 color;
if (intensity > 0.98) color = vec4(0.8,0.8,0.8,1.0); else if (intensity > 0.5) color = vec4(0.4,0.4,0.8,1.0); else if (intensity > 0.25) color = vec4(0.2,0.2,0.4,1.0); else color = vec4(0.1,0.1,0.1,1.0);
return color;
}
void main() {
vec3 n = normalize(normal);
float intensity = max(dot(lightDir, n),0.0); gl_FragColor = toonify(intensity);
}
)EOF";

GLuint CreateShader(GLenum eShaderType, const char* strShader)
{
    GLuint shader = glCreateShader(eShaderType);
    glShaderSource(shader, 1, &strShader, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Compile failure in %s shader:\n%s\n", (eShaderType == GL_VERTEX_SHADER ? "vertex" : (eShaderType == GL_GEOMETRY_SHADER ? "geometry" : "fragment")), strInfoLog);
        delete[] strInfoLog;
    }
    return shader;
}

GLuint CreateProgram(const vector<GLuint>& shaderList)
{
    GLuint program = glCreateProgram();
    for (size_t i = 0; i < shaderList.size(); i++)
        glAttachShader(program, shaderList[i]);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }
    for (size_t i = 0; i < shaderList.size(); i++)
        glDetachShader(program, shaderList[i]);
    return program;
}

///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    ////        glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //        glFrontFace(GL_CW);
    glUseProgram(theProgram);

    tblock.transform = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1, 0, 0));
    tblock.transform = glm::rotate(tblock.transform, rot.y, glm::vec3(0, 1, 0));
    tblock.transform = glm::rotate(tblock.transform, rot.z, glm::vec3(0, 0, 1));
    tblock.transform = glm::translate(tblock.transform, trans);
    tblock.look = glm::lookAt(glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 1));
    float fovyRad = fovy * 3.1415 / 180;
    tblock.proj = glm::perspective(fovyRad, 1.0f, 0.1f, 20.0f);
    GLuint blockIndex = glGetUniformBlockIndex(theProgram, "TBlock");
    GLint blockSize;
    glGetActiveUniformBlockiv(theProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);
    const GLchar* names[] = { "transform", "look", "proj" };
    GLuint indices[3];
    glGetUniformIndices(theProgram, 3, names, indices);
    GLint offset[3];
    glGetActiveUniformsiv(theProgram, 3, indices, GL_UNIFORM_OFFSET, offset);
    memcpy(blockBuffer + offset[0], glm::value_ptr(tblock.transform), sizeof(tblock.transform));
    memcpy(blockBuffer + offset[1], glm::value_ptr(tblock.look), sizeof(tblock.look));
    memcpy(blockBuffer + offset[2], glm::value_ptr(tblock.proj), sizeof(tblock.proj));
    GLuint uBuf;
    glGenBuffers(1, &uBuf);
    glBindBuffer(GL_UNIFORM_BUFFER, uBuf);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uBuf);
    glUniform1i(glGetUniformLocation(theProgram, "tex"), 27);

    glBindVertexArray(vertexBufferArray);
    glDrawArrays(GL_TRIANGLES, 0, triangleCount);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    //        glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glGenVertexArrays(1, &vertexBufferArray);
    glBindVertexArray(vertexBufferArray);
    // setup vertex buffer
    glGenBuffers(1, &positionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    //    WavefrontObject object = readWavefrontObject("/Users/jogehring/Documents/Informatik/Computergrafik/datasets/AnnoComplete/AnnoComplete.obj");
    WavefrontObject object = readWavefrontObject("C:/Users/Luca/Downloads/Wavefront Datasets CG/datasets/cube.obj");
    vector<float> buffer = object.transformToBuffer();
    triangleCount = static_cast<int>(object.vertices.size() * 3);
    int stride = 0;
    if (object.hasNormals && object.hasTextures) stride = 9;
    else if (object.hasTextures) stride = 6;
    else if (object.hasNormals) stride = 7;
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    //    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer.size(), buffer.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    if (object.hasNormals) glEnableVertexAttribArray(1);
    if (object.hasTextures) glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, 0);
    if (object.hasNormals) glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (const GLvoid*)(sizeof(GLfloat) * 4));
    if (object.hasTextures) glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (const GLvoid*)(sizeof(GLfloat) * (object.hasNormals ? 6 : 4)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint tex;
    GLuint sampler;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE27);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, brick);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindSampler(27, sampler);

    // make shaders
    vector<GLuint> shaders;
    shaders.push_back(CreateShader(GL_VERTEX_SHADER, vs2));
    shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fs1));
    theProgram = CreateProgram(shaders);
}

void keyPress(GLFWwindow* window, int k, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) {
        if (mods != GLFW_MOD_SHIFT) k += 32;
        char c = (char)k;
        switch (c) {
        case 'x': rot.x += 0.1; break;
        case 'y': rot.y += 0.1; break;
        case 'z': rot.z += 0.1; break;
        case 'X': rot.x -= 0.1; break;
        case 'Y': rot.y -= 0.1; break;
        case 'Z': rot.z -= 0.1; break;
        case 'w': trans.y += 0.1; break;
        case 's': trans.y -= 0.1; break;
        case 'a': trans.x -= 0.1; break;
        case 'd': trans.x += 0.1; break;
        case 'q': exit(1);
        case 'f': fovy -= 0.5; break;
        case 'F': fovy += 2; break;

        }
    }
}

void changeSize(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
    if (!glfwInit()) {
        printf("glfwInit failed\n");
        exit(1);
    }
    printf("glfw version: %s\n", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(500, 500, "Shader", NULL, NULL);
    if (!window) {
        printf("Error opening window");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyPress);
    glfwSetFramebufferSizeCallback(window, changeSize);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    SetupRC();
    printf("%s\n%s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

    while (!glfwWindowShouldClose(window)) {
        RenderScene();
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

*/


#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include <stdio.h>
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)

#include <math.h>--
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

GLfloat rotx = 0;
GLfloat roty = 0;
GLfloat rotz = 0;
GLfloat tx = 0;
GLfloat ty = 0;
GLfloat tz = 0;


const GLfloat vertexPositions[] = {		// UB07 Aufgabe 2
	//Positionen                //Farben		   //Normalen
   0.75f, 0.75f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   0.75f, -0.75f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
   -0.75f, -0.75f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
};

/*
const GLfloat vertexPositions[] = {		// Original
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f
};
*/


GLuint vertexBufferArray = 0;		// entspricht vertexArrayObject 
GLuint positionBufferObject = 0;	// entspricht vertexBufferObject 
GLuint theProgram = 0;
/*
const char vs1[] = R"EOF(
#version 330
layout(location = 1) in vec4 position;
layout (location = 0) in vec4 color;
uniform mat4 transform;
smooth out vec4 myColor;
void main() {
gl_Position = transform * position;
myColor = color;
}
)EOF";
*/

// UB07 Aufgabe 2
//Vertex shader
const char vs1[] = R"EOF(		
#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
uniform mat4 transform;

out vec4 myColor;
void main() {
gl_Position = transform * position;
myColor = color;
}
)EOF";

//Fragment shader
const char fs1[] = R"EOF(
#version 330

smooth in vec4 myColor; 
out vec4 outColor;

void main(){
   outColor = myColor; 
}
)EOF";

struct HPoint {
	float x, y, z, w;
	HPoint(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w) {};
};

struct TextureCoord {
	float x, y;
	TextureCoord(float x = 0, float y = 0) : x(x), y(y) {};
};

struct Normal {
	float x, y, z;
	Normal(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {};
};

struct Triangle {
	vector<HPoint> vertices;
	vector<TextureCoord> textureCoords;
	vector<Normal> verticesNormals;
	Triangle(vector<HPoint> vertices = {}, vector<TextureCoord> textureCoords = {}, vector<Normal> verticesNormals = {}) : vertices(vertices), textureCoords(textureCoords), verticesNormals(verticesNormals) {};
};

vector<string> split(const char* str, char c = ' ')
{
	vector<string> result;

	do
	{
		const char* begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}

vector<Triangle> readWavefrontObj(string fPath) {	// ¯\_(ツ)_/¯ dogshit code ngl
	std::ifstream input(fPath);

	vector<Triangle> triangles = {};
	vector<HPoint> vertices = {};
	vector<TextureCoord> textureCoords = {};
	vector<Normal> normals = {};

	string line = "";
	getline(input, line);

	while (getline(input, line))
	{
		if (line.find("v ") != std::string::npos) {
			vector<string> tokenizedLine = split(line.c_str(), ' ');
			HPoint point = HPoint(stof(tokenizedLine[1]), stof(tokenizedLine[2]), stof(tokenizedLine[3]), 1.0);
			vertices.push_back(point);
		}
		else if (line.find("vt ") != std::string::npos) {
			vector<string> tokenizedLine = split(line.c_str(), ' ');
			TextureCoord textureCord = TextureCoord(stof(tokenizedLine[1]), stof(tokenizedLine[2]));
			textureCoords.push_back(textureCord);
		}
		else if (line.find("vn ") != std::string::npos) {
			vector<string> tokenizedLine = split(line.c_str(), ' ');
			Normal normalTemp = Normal(stof(tokenizedLine[1]), stof(tokenizedLine[2]), stof(tokenizedLine[3]));
			normals.push_back(normalTemp);
		}
		else if ((line.find("f ") != std::string::npos) && (line.find("//") != std::string::npos)) { // do triangle 
			cout << "Truly a bruh moment" << endl;
			vector<string> tokenizedLine = split(line.c_str(), ' ');
			tokenizedLine.erase(tokenizedLine.begin());

			Triangle triangle = {};
			for (string s : tokenizedLine) {
				vector<string> triangleVertexInfo = split(s.c_str(), '//');
				triangle.vertices.push_back(vertices[stoi(triangleVertexInfo[0]) - 1]);
				triangle.verticesNormals.push_back(normals[stoi(triangleVertexInfo[1]) - 1]);
			}
			triangles.push_back(triangle);
		}
		else if ((line.find("f ") != std::string::npos) && (line.find("/") != std::string::npos)) { // do triangle 
			vector<string> tokenizedLine = split(line.c_str(), ' ');
			tokenizedLine.erase(tokenizedLine.begin());

			Triangle triangle = {};
			for (string s : tokenizedLine) {
				vector<string> triangleVertexInfo = split(s.c_str(), '/');
				triangle.vertices.push_back(vertices[stoi(triangleVertexInfo[0]) - 1]);
				triangle.textureCoords.push_back(textureCoords[stoi(triangleVertexInfo[1]) - 1]);
				triangle.verticesNormals.push_back(normals[stoi(triangleVertexInfo[2]) - 1]);
			}
			triangles.push_back(triangle);
		}
		else if ((line.find("f ") != std::string::npos) && !(line.find("/") != std::string::npos)) { // do triangle 
			vector<string> triangleVertexInfo = split(line.c_str(), ' ');
			triangleVertexInfo.erase(triangleVertexInfo.begin());

			Triangle triangle = {};
			for (string s : triangleVertexInfo) {
				triangle.vertices.push_back(vertices[stoi(s) - 1]);
			}
			triangles.push_back(triangle);
		}
	}
	cout << "bruh lol lmao" << endl;
	return triangles;
}

vector<float> getAllTrianglePointsAsOneVec(vector<Triangle>& triangles) {
	vector<float> result = {};
	//float j = 0;
	for (Triangle& triangle : triangles) {

		for (int i = 0; i < triangle.vertices.size(); i++) {
			result.push_back(triangle.vertices[i].x);
			result.push_back(triangle.vertices[i].y);
			result.push_back(triangle.vertices[i].z);
			result.push_back(triangle.vertices[i].w);
			result.push_back(0.8);
			result.push_back(0.8);
			result.push_back(0.8);
			result.push_back(0.8);
			/*result.push_back(j);
			result.push_back(1 - j);
			result.push_back(1);
			result.push_back(1.0f);*/
		}
		//j = j + 1.0f / 12.0f;
	}
	return result;
}


GLuint CreateShader(GLenum eShaderType, const char* strShader)
{
	GLuint shader = glCreateShader(eShaderType);
	glShaderSource(shader, 1, &strShader, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", (eShaderType == GL_VERTEX_SHADER ? "vertex" : (eShaderType == GL_GEOMETRY_SHADER ? "geometry" : "fragment")), strInfoLog);
		delete[] strInfoLog;
	}
	return shader;
}

GLuint CreateProgram(const vector<GLuint>& shaderList)
{
	GLuint program = glCreateProgram();
	for (size_t i = 0; i < shaderList.size(); i++)
		glAttachShader(program, shaderList[i]);
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	for (size_t i = 0; i < shaderList.size(); i++)
		glDetachShader(program, shaderList[i]);
	return program;
}

///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);


	glClear(GL_DEPTH_BUFFER_BIT);


	glEnable(GL_DEPTH_TEST);
	glUseProgram(theProgram);

	/*
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0, 1, 0));
	glm::mat4 t = glm::translate(glm::mat4(1.0f), trans);
	glm::mat4 m = r * t;
	glUniformMatrix4fv(
		glGetUniformLocation(theProgram, "transform"),
		1, GL_FALSE, glm::value_ptr(m));
		*/

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));

	glm::mat4 rotationx = glm::rotate(glm::mat4(1.0f), glm::radians<float>(rotx), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationy = glm::rotate(glm::mat4(1.0f), glm::radians<float>(roty), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationz = glm::rotate(glm::mat4(1.0f), glm::radians<float>(rotz), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 transform = translation * rotationx * rotationy * rotationz;

	glUniformMatrix4fv(glGetUniformLocation(theProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

	glBindVertexArray(vertexBufferArray);
	//glDrawArrays(GL_LINE_STRIP, 0, 3); // UB07 AUfgabe 2
	//glDrawArrays(GL_TRIANGLES, 0, 288);	 // UB06 Aufgabe 3
	glDrawArrays(GL_TRIANGLES, 0, 80000000);	 // fuer mammuth kp ob das funktioniert 

	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// setup vertex buffer
	glGenVertexArrays(1, &vertexBufferArray);
	glBindVertexArray(vertexBufferArray);
	// setup data buffer
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);	// UB07  Aufgabe 2

	// UB06 Aufgabe 3
	vector<Triangle> triangles = readWavefrontObj("C:/Users/Luca/Downloads/Wavefront Datasets CG/datasets/sphere.obj");
	//vector<Triangle> triangles = readWavefrontObj("C:/Users/Luca/Downloads/mammut/mammoth - master_model.obj");
	vector<float> vertexData = getAllTrianglePointsAsOneVec(triangles);
	for (float f : vertexData) {
		cout << f << endl;
	}

	///TODO: Alle Vertex Punkte in Vektor aus VertexPositionsObjects packen und schauen ob's dann funktioniert 

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);			// HERE 

	glEnableVertexAttribArray(0);  // location 0
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);  // location 1
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// make shaders
	vector<GLuint> shaders;
	shaders.push_back(CreateShader(GL_VERTEX_SHADER, vs1));
	shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fs1));
	theProgram = CreateProgram(shaders);
}

void keyPress(unsigned char k, int x, int y)
{
	switch (k) {
	case 'w': ty += 0.1f; break;
	case 's': ty -= 0.1f; break;
	case 'a': tx += 0.1f; break;
	case 'd': tx -= 0.1f; break;
	case 'e': tz += 0.1f; break;
	case 'r': tz -= 0.1f; break;
	case 'j': roty += 10.0f; break;
	case 'l': roty -= 10.0f; break;
	case 'i': rotx += 10.0f; break;
	case 'k': rotx -= 10.0f; break;
	case 'o': rotz += 10.0f; break;
	case 'p': rotz -= 10.0f; break;
	case 'q': exit(1);
	}
	glutPostRedisplay();
}

void changeSize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Shader");

	glutKeyboardFunc(keyPress);
	glutReshapeFunc(changeSize);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 not supported\n");
		exit(1);
	}

	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}

