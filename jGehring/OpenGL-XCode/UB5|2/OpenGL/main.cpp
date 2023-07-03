//
//  main.cpp
//  OpenGL
//
//  Created by Frank Deinzer on 17.02.16.
//  Copyright © 2016 Frank Deinzer. All rights reserved.
//

// IMPORTANT: use Homebrew to install glew package
// brew install glew

#include "OpenGL/gl.h"
#include "GLUT/glut.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
using namespace::std;
GLfloat x = 0.0f;
GLfloat y = 0.0f;
GLfloat z = 0.0f;
GLint deg = 10;
GLfloat sx =1;
GLfloat sy = 1;
GLint xRot = 0.0;
GLint yRot = 0.0;
GLint zRot = 0.0;
GLboolean smaller = false;
///////////////////////////////////////////////////////////
// Called to draw scene


///////////////////////////////////////////////////////////
// Setup the rendering state



void translateBySteps(float xStep, float yStep, float zStep){
    glTranslatef(-x, -y, 0);
    x += xStep;
    y += yStep;
    z += zStep;
    glTranslatef(x, y, z);
    glutPostRedisplay();
}

void drawCuboid(GLfloat size, GLfloat r = 0, GLfloat g=1.0, GLfloat b=0){
    glutSolidCube(size);
}

void KeyPress(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q': exit(1);
        case 'a': {yRot += (yRot == 360) ? -355 : 5; glRotatef(yRot,0, 1, 0); break;}
        case 'd': {yRot -= (yRot == -360) ? 355 : 5; glRotatef(yRot, 0, 1, 0);break;}
        case 's': {xRot += (xRot == 360) ? -355 : 5; glRotatef(xRot, 1, 0, 0);break;}
        case 'w': {xRot -= (xRot == -360) ? 355 : 5; glRotatef(xRot, 1, 0, 0);break;}
//        case 'R': drawRobot(); break;
    }
    glutPostRedisplay();
}

void TimerFunction(int value){
    glutTimerFunc(33,TimerFunction,1);
    deg+= 10;
    if(deg ==360) deg = 0;
    if(sx > 2.0 || sx < 1.0) smaller = !smaller;
    if(!smaller){
        sx += 0.1;
        sy += 0.1;
    } else{
        sx -= 0.1;
        sy -= 0.1;
    }
}
void SetupRC(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
}

void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    drawCuboid(1.0);
    glFlush();
    
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Simple");
    glutReshapeWindow(512, 512);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(KeyPress);
    glutTimerFunc(33, TimerFunction, 1);
    
    SetupRC();
    
    glutMainLoop();
    
    return 0;
}

