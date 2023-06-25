// Shader.cpp
// The Simplest OpenGL shader program with GLUT

#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include <stdio.h>
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)
#include <glm/glm/glm.hpp>
#include <glm/glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/glm/gtc/type_ptr.hpp> // glm::translate, glm::rotate, glm::scale

#include <math.h>
#include <string>
#include <vector>
#include <iostream>
#include "../OpenGL-compatibility/WavefrontParser.hpp"
#include "ObjectGenerator.h"

#include "textures/moonmap.h"
#include "textures/wmap.h"

GLfloat rotx = 0;
GLfloat roty = 0;
GLfloat rotz = 0;
GLfloat tz = 1;

GLuint vertexBufferArray = 0;
GLuint positionBufferObject = 0;
GLuint theProgram = 0;
GLuint tex;
GLuint sampler;

///////////////////////////////////////////////////////////
// TRIANGLE VARIABLES AND FUNCTIONS (7.2)

const GLfloat vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
};

void keyPress(unsigned char k, int x, int y)
{
	switch (k) {
	case 'x': rotx += 0.1f; break;
	case 'y': roty += 0.1f; break;
	case 'z': rotz += 0.1f; break;
	case 'X': rotx -= 0.1f; break;
	case 'Y': roty -= 0.1f; break;
	case 'Z': rotz -= 0.1f; break;
	case '+': tz += 0.1f; break;
	case '-': tz -= 0.1f; break;
	case 'q': exit(1);
	}
	glutPostRedisplay();
}

// TRIANGLE SHADERS
const char vs1[] = R"EOF(
#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
smooth out vec4 myColor;
uniform mat4 transform;
void main() {
    gl_Position = transform * position;
	myColor = color;
}
)EOF";

const char fs1[] = R"EOF(
#version 330
smooth in vec4 myColor;
out vec4 outColor;
void main(){
   outColor = myColor;
}
)EOF";

///////////////////////////////////////////////////////////
// MODEL VARIABLES AND FUNCTIONS (7.3)

std::string fname = "";
Wavefront::WavefrontParser parser = Wavefront::WavefrontParser();
std::vector<GLfloat> values;
void modelKeyPress(unsigned char k, int x, int y)
{
	switch (k) {
	case 'x': rotx += 0.1f; break;
	case 'y': roty += 0.1f; break;
	case 'z': rotz += 0.1f; break;
	case 'X': rotx -= 0.1f; break;
	case 'Y': roty -= 0.1f; break;
	case 'Z': rotz -= 0.1f; break;
	case '+': tz += 0.1f; break;
	case '-':
		if ((tz -= 0.1f) <= 0.f) {
			tz += 0.1f;
		}
		break;
	case 'a':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/cube.obj";
		std::cout << "reading file " << fname << std::endl;
		parser.parse(fname);
		break;
	case 's':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/sphere.obj";
		std::cout << "reading file " << fname << std::endl;
		parser.parse(fname);
		break;
	case 'd':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/teapot/teapot.obj";
		std::cout << "reading file " << fname << std::endl;
		parser.parse(fname);
		break;
	case 'f':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/lucy/lucy_low.obj";
		std::cout << "reading file " << fname << std::endl;
		parser.parse(fname);
		break;
	case 'g':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/happybuddha/happybuddha.obj";
		std::cout << "reading file " << fname << std::endl;
		parser.parse(fname);
		break;
	case 'q': exit(1);
	}

	if (fname != "") {
		glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
		values = parser.exportAsArray();
		glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(GLfloat), &values.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glutPostRedisplay();
}


// MODEL SHADERS
const char vsModel1[] = R"EOF(
#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
flat out vec4 myColor;
layout(std140) uniform TBlock {
	mat4 transform;
	mat4 look;
	mat4 proj;
};
void main() {
    gl_Position = proj * look * transform * position;
	myColor = 0.5 + 0.5 * vec4(normal.x, normal.y, normal.z, 1.0);
}
)EOF";

const char fsModel1[] = R"EOF(
#version 330
flat in vec4 myColor;
out vec4 outColor;
void main(){
   outColor = myColor;
}
)EOF";

///////////////////////////////////////////////////////////
// OBJECT GENERATOR STUFF

std::vector<GLfloat> objectVertices;

void bindEarthTexture() {
	glActiveTexture(GL_TEXTURE1); // GL_TEXTURE1 is earth, GL_TEXTURE2 is moon
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, world);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
}

void bindMoonTexture() {
	glActiveTexture(GL_TEXTURE2); // GL_TEXTURE1 is earth, GL_TEXTURE2 is moon
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, moon);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
}

// SHADERS
const char vsObject1[] = R"EOF(
#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;
smooth out vec3 myNormal;
smooth out vec2 myTexCoords;
smooth out vec4 myPosition;
layout(std140) uniform TBlock {
	mat4 transform;
	mat4 look;
	mat4 proj;
};
layout(std140) uniform LightSources {
	vec3 position[10];
	vec4 color[10];
	vec4 ambient;
};
layout(std140) uniform MaterialConstants {
	float mirrorReflexivity;
	float ambientReflexivity;
	float diffuseReflexivity;
};
void main() {
    gl_Position = proj * look * transform * position;
	myPosition = gl_Position;
	myNormal = normalize(inverse(transpose(mat3(look * transform))) * normal);
	myTexCoords = tex;
}
)EOF";

const char fsObject1[] = R"EOF(
#version 330
smooth in vec3 myNormal;
smooth in vec4 myPosition;
smooth in vec2 myTexCoords;
out vec4 outColor;
uniform sampler2D tex;
layout(std140) uniform LightSources {
	vec3 lightPosition[10];
	vec4 lightColor[10];
	vec4 ambient;
}
layout(std140) uniform MaterialConstants {
	float mirrorReflexivity;
	float ambientReflexivity;
	float diffuseReflexivity;
	float shininess
};
void getLighting(vec4 color) {
	vec4 light = ambient * ambientReflexivity;
	myNormal = normalize(myNormal);
	vec3 viewDir = normalize(-myPosition.xyz);
	for(int i=0;i<10;i++) {
		vec3 lightDir = normalize(-lightPosition[i]);
		vec3 reflectDir = reflect(-lightDir, myNormal);
		float lightNormalAngleCos = max(dot(lightDir, myNormal), 0.0);
		float specularAngleCos = max(dot(reflectDir, viewDir), 0.0);
		fac = (diffuseReflexivity * angleCos) + (mirrorReflexivity);
	}
}
void main(){
	vec4 currentColor = texture(tex, myTexCoords);
}
)EOF";


///////////////////////////////////////////////////////////
// GENERAL RENDER FUNCTIONS
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

GLuint CreateProgram(const std::vector<GLuint>& shaderList)
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(theProgram);

	glm::mat4 const rotxMat = glm::rotate(glm::mat4(1.0f), rotx, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 const rotyMat = glm::rotate(glm::mat4(1.0f), roty, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 const rotzMat = glm::rotate(glm::mat4(1.0f), rotz, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 const scale = glm::scale(glm::mat4(1.0f), glm::vec3(tz, tz, tz));
	// 7.2
	/*
	glm::mat4 m = scale * rotxMat * rotyMat * rotzMat;
	glUniformMatrix4fv(
		glGetUniformLocation(theProgram, "transform"),
		1, GL_FALSE, glm::value_ptr(m)
	);
	glBindVertexArray(vertexBufferArray);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	*/

	// 7.3
	struct {
		glm::mat4 transform;
		glm::mat4 look;
		glm::mat4 proj;
	} tblock;
	tblock.transform = scale * rotxMat * rotyMat * rotzMat;
	tblock.look = glm::lookAt(glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 1));
	tblock.proj = glm::perspective(90.0f, 1.0f, 0.01f, 200.0f);

	GLuint blockIndex = glGetUniformBlockIndex(theProgram, "TBlock");
	GLuint uBuf;
	glGenBuffers(1, &uBuf);
	glBindBuffer(GL_UNIFORM_BUFFER, uBuf);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(tblock), &tblock, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uBuf);

	/*
	if (fname != "") {
		glBindVertexArray(vertexBufferArray);
		glDrawArrays(GL_TRIANGLES, 0, values.size() / 7);
	}
	*/
	bindEarthTexture();
	glBindTexture(GL_TEXTURE_2D, tex);

	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindSampler(1, sampler);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, objectVertices.size() * sizeof(GLfloat), &objectVertices.front(), GL_STATIC_DRAW);
	glBindVertexArray(vertexBufferArray);
	glDrawArrays(GL_TRIANGLES, 0, objectVertices.size() / 9);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	// setup vertex buffer
	glGenVertexArrays(1, &vertexBufferArray);
	glBindVertexArray(vertexBufferArray);
	// setup data buffer
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);  // location 0
	glEnableVertexAttribArray(1);  // location 1
	glEnableVertexAttribArray(2);  // location 1
	// 7.2
	/*
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (const GLvoid*)(sizeof(GLfloat) * 4));
	*/
	// 7.3
	/*
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (const GLvoid*)(sizeof(GLfloat) * 4));
	*/
	// 7.4
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (const GLvoid*)(sizeof(GLfloat) * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (const GLvoid*)(sizeof(GLfloat) * 7));

	glGenTextures(1, &tex);

	objectVertices = ObjectGenerator::ObjectGenerator::generateSphere(1.f, 2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// make shaders
	std::vector<GLuint> shaders;
	/*
	shaders.push_back(CreateShader(GL_VERTEX_SHADER, vsModel1));
	shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fsModel1));
	*/
	shaders.push_back(CreateShader(GL_VERTEX_SHADER, vsObject1));
	shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, fsObject1));
	theProgram = CreateProgram(shaders);
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

	// 7.2
	// glutKeyboardFunc(keyPress);
	// 7.3
	glutKeyboardFunc(modelKeyPress);
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

