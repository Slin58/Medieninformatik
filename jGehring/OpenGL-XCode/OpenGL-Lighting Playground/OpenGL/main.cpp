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
#include "../../../texture/wmap.h"
#include "../../../texture/moonmap.h"
#include "../../../texture/brick.h"
#include <glm/gtx/string_cast.hpp>
#include "ShaderReader.hpp"
#include "gouraud.vs"
#include "gouraud.fs"
#include "phong.vs"
#include "phong.fs"
#include "texture.vs"
#include "texture.fs"
#include "toonify.fs"
using namespace std;

GLFWwindow* window = nullptr;


struct Tblock{
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
GLfloat fovy = 45.0f;
int triangleCount;
bool lines = false;
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);

const char fs1[] = R"EOF(
#version 330
out vec4 outColor;

void main(){
    outColor = vec4(0.0, 1.0, 0.0, 1.0);
}
)EOF";


const char basicLightingVertex[] = R"EOF(
#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normalIn;

out vec3 normal;
layout(std140) uniform TBlock {
mat4 transform;
mat4 look;
mat4 proj;
};

void main() {
gl_Position = proj * look * transform * position;
vec3 vertexPos = vec3(gl_Position);
vec3 lightDir = lightPos - vertexPos;
normal = normalIn;
}
)EOF";


const char basicLightingFragment[] = R"EOF(
#version 330
out vec4 outColor;
smooth in vec3 normal;
in vec3 lightDir;

void main(){
    vec3 l = normalize(lightDir);
    vec3 n = normalize(normal);
    float NdotL = max(dot(l, n), 0.0);
    NdotL = min(NdotL, 1.0);
    outColor = NdotL*vec4(0.0, 1.0, 0.0, 1.0);
}
)EOF";
//float otColor = NdotL*vec4( 1.0f, 1.0f, 1.0f, 1.0f );
//vec3 dx = dFdx( pos );
//vec3 dy = dFdy( pos );
//vec3 N = normalize(cross(dx, dy));
//N *= sign(N.z);


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
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glUseProgram(theProgram);
    
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);


//
//    glm::vec4 lightColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    glUniformMatrix4fv(glGetUniformLocation(theProgram, "lightColor"), 1, GL_FALSE, glm::value_ptr(lightColor));
    
    tblock.transform = glm::rotate(glm::mat4(1.0f), rot.x, glm::vec3(1,0,0));
    tblock.transform = glm::rotate(tblock.transform, rot.y, glm::vec3(0,1,0));
    tblock.transform = glm::rotate(tblock.transform, rot.z, glm::vec3(0,0,1));
    tblock.transform = glm::translate(tblock.transform, trans);
    tblock.look = glm::lookAt(glm::vec3(0,0,-3), glm::vec3(0,0,0), glm::vec3(0,1,1));
    glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(tblock.transform)));
    float fovyRad = fovy*3.1415/180;
    tblock.proj = glm::perspective(fovyRad, 1.0f, 0.1f, 20.0f);
   
    GLuint blockIndex = glGetUniformBlockIndex(theProgram, "TBlock");
    GLint blockSize;
    glGetActiveUniformBlockiv(theProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
    GLubyte *blockBuffer= (GLubyte *) malloc(blockSize);
    const GLchar *names[]={"transform", "look", "proj"};
    GLuint indices[3];
    glGetUniformIndices(theProgram, 3, names, indices);
    GLint offset[3];
    glGetActiveUniformsiv(theProgram, 3, indices, GL_UNIFORM_OFFSET, offset);
    memcpy(blockBuffer + offset[0], glm::value_ptr(tblock.transform), sizeof(tblock.transform));
    memcpy(blockBuffer + offset[1], glm::value_ptr(tblock.look), sizeof(tblock.look));
    memcpy(blockBuffer + offset[2], glm::value_ptr(tblock.proj), sizeof(tblock.proj));
    
    glUniformMatrix3fv(glGetUniformLocation(theProgram, "lightPos"), 1, GL_FALSE, glm::value_ptr(lightPos));
    glUniformMatrix3fv(glGetUniformLocation(theProgram, "normalTransform"), 1, GL_FALSE, glm::value_ptr(normalTransform));
    
    GLuint uBuf;
    glGenBuffers(1, &uBuf);
    glBindBuffer(GL_UNIFORM_BUFFER, uBuf);
    glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uBuf);
    glUniform1i(glGetUniformLocation(theProgram, "tex"), 27);

    
    glBindVertexArray(vertexBufferArray);
    if(!lines) glDrawArrays(GL_TRIANGLES, 0, triangleCount);
    else glDrawArrays(GL_LINE_LOOP, 0, triangleCount);
    glBindVertexArray(0);
    
    glfwSwapBuffers(window);
    glDisable(GL_CULL_FACE);
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
    WavefrontObject object = readWavefrontObject("/Users/jogehring/Documents/Informatik/Computergrafik/datasets/cube_without_normals.obj", true);
    vector<float> buffer = object.transformToBuffer();
    triangleCount = static_cast<int>(object.vertices.size()*3);
    int stride = 0;
    if(object.hasNormals && object.hasTextures) stride = 9;
    else if(object.hasTextures) stride = 6;
    else if(object.hasNormals) stride = 7;
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
    //    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*buffer.size(), buffer.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if(object.hasTextures) glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, 0);
    if(object.hasNormals) glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, (const GLvoid*) (sizeof(GLfloat)*4));
    if(object.hasTextures) glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*stride, (const GLvoid*) (sizeof(GLfloat)*(object.hasNormals ? 7 : 4)));
    
    GLuint tex;
    GLuint sampler;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE27);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, world);
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
    
//    char *vertex = readVertexShader("gouraud");
//    char *fragment = readFragmentShader("gouraud");
    
    shaders.push_back(CreateShader(GL_VERTEX_SHADER, gouraudVertex));
    shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, gouraudFragment));
    theProgram = CreateProgram(shaders);
}

void keyPress(GLFWwindow* window, int k, int scancode, int action, int mods)
{
    if(action == GLFW_RELEASE){
        if(mods != GLFW_MOD_SHIFT) k += 32;
        char c = (char) k;
        switch(c) {
            case 'x': rot.x +=0.1; break;
            case 'y': rot.y +=0.1; break;
            case 'z': rot.z +=0.1; break;
            case 'X': rot.x -=0.1; break;
            case 'Y': rot.y -=0.1; break;
            case 'Z': rot.z -=0.1; break;
            case 'w': trans.y += 0.1; break;
            case 's': trans.y -= 0.1; break;
            case 'a': trans.x -= 0.1; break;
            case 'd': trans.x += 0.1; break;
            case 'q': exit(1);
            case 'f': fovy -= 2; break;
            case 'F': fovy += 2; break;
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

