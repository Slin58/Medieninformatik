//
//  main.cpp
//  OpenGL
//

// IMPORTANT: use Homebrew to install glfw package
// brew install glfw

#include <stdio.h>
#include <cstdlib>
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include "OpenGL/gl.h"
#include "GLUT/glut.h"
#include <math.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "iostream"
#include "WavefrontObject.hpp"
#include <glm/gtx/string_cast.hpp>
#include "triangleColor.vs"
#include "triangleColor.fs"

using namespace std;
using namespace glm;
GLFWwindow* window = nullptr;

glm::vec3 rot(0.0f);
glm::vec3 trans(0.0f);
GLuint positionBufferObject = 0;
vector<GLuint> vertexBufferArrays = {};
GLuint colorBufferArray = 0;
GLuint theProgram = 0;
int triangleCount = 0;
bool lines = false;

GLuint CreateShader(GLenum eShaderType, const char *strShader)
{
    GLuint shader = glCreateShader(eShaderType);
    glShaderSource(shader, 1, &strShader, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Compile failure in %s shader:\n%s\n", (eShaderType == GL_VERTEX_SHADER ? "vertex" : (eShaderType == GL_GEOMETRY_SHADER ? "geometry" : "fragment")), strInfoLog);
        delete[] strInfoLog;
    }
    return shader;
}

GLuint CreateProgram(const vector<GLuint> &shaderList)
{
    GLuint program = glCreateProgram();
    for(size_t i = 0; i < shaderList.size(); i++)
        glAttachShader(program, shaderList[i]);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }
    for(size_t i = 0; i < shaderList.size(); i++)
        glDetachShader(program, shaderList[i]);
    return program;
}

///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    //            glEnable(GL_CULL_FACE);
    //            glEnable(GL_DEPTH_TEST);
    //            glFrontFace(GL_CCW);
    glUseProgram(theProgram);
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), trans);
    glUniformMatrix4fv(glGetUniformLocation(theProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
    
    
    for(GLuint vba : vertexBufferArrays){
        glBindVertexArray(vba);
        if(!lines) glDrawArrays(GL_TRIANGLES, 0, triangleCount);
        else glDrawArrays(GL_LINE_LOOP, 0, triangleCount);
        glFlush();
    }
    glBindVertexArray(0);
    
    glfwSwapBuffers(window);
    //            glDisable(GL_CULL_FACE);
    //            glDisable(GL_DEPTH_TEST);
}

static void bindWavefrontObject(string path, bool triangleNormals) {
    GLuint vba = 0;
    glGenVertexArrays(1, &vba);
    glBindVertexArray(vba);
    vertexBufferArrays.push_back(vba);
    // setup vertex buffer
    glGenBuffers(1, &positionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    WavefrontObject object = readWavefrontObject(path, triangleNormals);
    object = readWavefrontObject(path, true);
    vector<float> buffer = object.transformToBuffer();
    int stride = 0;
    triangleCount = static_cast<int>(object.vertices.size()*3);
    if(object.hasNormals && object.hasTextures) stride = 9;
    else if(object.hasTextures) stride = 6;
    else if(object.hasNormals) stride = 7;
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*buffer.size(), buffer.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if(object.hasTextures) glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, 0);
    if(object.hasNormals) glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, (const GLvoid*) (sizeof(GLfloat)*4));
    if(object.hasTextures) glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, (const GLvoid*) (sizeof(GLfloat)*(object.hasNormals ? 7 : 4)));
}


static void drawTriangle() {
    GLuint vba = 0;
    glGenVertexArrays(1, &vba);
    glBindVertexArray(vba);
    vertexBufferArrays.push_back(vba);
    // setup vertex buffer
    glGenBuffers(1, &positionBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
    int stride = 8;
    vector<float> buffer = {0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.37f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.37f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
    triangleCount = static_cast<int>(3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*buffer.size(), buffer.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, (const GLvoid*) (sizeof(GLfloat)*4));
    
    
}



///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    drawTriangle();
    
    // make shaders
    vector<GLuint> shaders;
    
    shaders.push_back(CreateShader(GL_VERTEX_SHADER, triangleColorVertex));
    shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, triangleColorFragment));
    theProgram = CreateProgram(shaders);
}

void keyPress(GLFWwindow* window, int k, int scancode, int action, int mods)
{
    if(action == GLFW_RELEASE){
        if(mods != GLFW_MOD_SHIFT) k += 32;
        char c = (char) k;
        switch(c) {
            case 'w': trans.y += 0.1; break;
            case 's': trans.y -= 0.1; break;
            case 'a': trans.x -= 0.1; break;
            case 'd': trans.x += 0.1; break;
            case 'q': exit(1);
            case 'l': lines = !lines; break;
        }
    }
}

void changeSize(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
    if(!glfwInit()) {
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
    if(!window) {
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

