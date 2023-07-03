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
GLfloat rightFrustum = 1;
GLfloat topFrustum = 1;
GLfloat bottomFrustum = -1;
GLfloat nearFrustum = 1.0;
GLfloat farFrustum = 10.0;
GLfloat leftFrustum = -1;
GLfloat leftOrtho = -1;
GLfloat rightOrtho = 1;
GLfloat topOrtho = 1;
GLfloat bottomOrtho = -1;
GLfloat nearOrtho = 1.0;
GLfloat farOrtho = 10.0;
GLfloat nearPersp = -1.0;
GLfloat farPersp = 10.0;
GLfloat ratioPersp = 1.0;
GLfloat anglePersp = 60.0;
GLfloat xCamera = 0;
GLfloat yCamera = 0;
GLfloat zCamera = 1.0;
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

void drawRobot(){
    glLoadIdentity();
    glColor3f(0.0, 0.0, 1.0);
    drawCuboid(1);
    glScalef(2.5, 1, 1);
}

void TranslateWithArrowKeys(int key, int x, int y){
    switch (key){
        case GLUT_KEY_LEFT: translateBySteps(-0.03, 0, 0); break;
        case GLUT_KEY_RIGHT: translateBySteps(0.03, 0, 0); break;
        case GLUT_KEY_UP: translateBySteps(0, 0.03, 0); break;
        case GLUT_KEY_DOWN: translateBySteps(0, -0.03, 0); break;
    }
}
void PerspectiveProjectionWithArrowKeys(int key, int x, int y){
    glViewport(0, 0, 512, 512);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (key){
        case GLUT_KEY_UP: ratioPersp += 0.1; break;
        case GLUT_KEY_DOWN: ratioPersp -= 0.1; break;
        case GLUT_KEY_LEFT: anglePersp -= 1; break;
        case GLUT_KEY_RIGHT:anglePersp += 1; break;
        case GLUT_KEY_F1: nearPersp -= 0.2; break;
        case GLUT_KEY_F2: nearPersp += 0.2; break;
            case GLUT_KEY_F3: farPersp -= 0.2; break;
        case GLUT_KEY_F4: farPersp += 0.2; break;
        case GLUT_KEY_F5: ratioPersp = 1.0; anglePersp = 60.0; nearPersp = -1.0; farPersp = 10.0; break;
        
    }
    gluPerspective(anglePersp, ratioPersp, nearPersp, farPersp);
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void FrustumProjectionWithArrowKeys(int key, int x, int y){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (key){
        case GLUT_KEY_UP: topFrustum += 0.1; break;
        case GLUT_KEY_DOWN: bottomFrustum -= 0.1; break;
        case GLUT_KEY_LEFT: leftFrustum -= 0.1; break;
        case GLUT_KEY_RIGHT: rightFrustum += 0.1; break;
        case GLUT_KEY_F1: nearFrustum -= 0.2; break;
        case GLUT_KEY_F2: nearFrustum += 0.2; break;
        case GLUT_KEY_F3: farFrustum -= 0.2; break;
        case GLUT_KEY_F4: farFrustum += 0.2; break;
        case GLUT_KEY_F5: leftFrustum = -1; rightFrustum = 1; bottomFrustum = -1; topFrustum = 1; nearFrustum = -1; farFrustum = 1; break;
    }
    glFrustum(leftFrustum, rightFrustum, bottomFrustum, topFrustum, nearFrustum, farFrustum);
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void ParallelProjectionWithArrowKeys(int key, int x, int y){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (key){
        case GLUT_KEY_UP: topOrtho += 0.1; break;
        case GLUT_KEY_DOWN: bottomOrtho -= 0.1; break;
        case GLUT_KEY_LEFT: leftOrtho -= 0.1; break;
        case GLUT_KEY_RIGHT: rightOrtho += 0.1; break;
        case GLUT_KEY_F1: nearOrtho -= 0.2; break;
        case GLUT_KEY_F2: nearOrtho += 0.2; break;
        case GLUT_KEY_F3: farOrtho -= 0.2; break;
        case GLUT_KEY_F4: farOrtho += 0.2; break;
        case GLUT_KEY_F5: leftOrtho = -1; rightOrtho = 1; bottomOrtho = -1; topOrtho = 1; nearOrtho = 1.0; farOrtho = 10.0; break;
    }
    glOrtho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, nearOrtho, farOrtho);
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadIdentity();
}

void MoveCamera(int key, int x, int y){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (key){
        case GLUT_KEY_UP: yCamera+=0.1; break;
        case GLUT_KEY_DOWN: yCamera -= 0.1; break;
        case GLUT_KEY_LEFT: xCamera -= 0.1; break;
        case GLUT_KEY_RIGHT: xCamera += 0.1; break;
        case GLUT_KEY_F1: zCamera -= 0.1; break;
        case GLUT_KEY_F2: zCamera += 0.1; break;
        case GLUT_KEY_F3: xCamera = 0; yCamera = 0; zCamera = 1; break;
    }
    gluLookAt(xCamera, yCamera, zCamera, 0, 0, 0, 0, 1, 0);
    glutPostRedisplay();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



void KeyPress(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q': exit(1);
        case 'a': {yRot += (yRot == 360) ? -355 : 5; glRotatef(yRot,0, 1, 0); break;}
        case 'd': {yRot -= (yRot == -360) ? 355 : 5; glRotatef(yRot, 0, 1, 0);break;}
        case 's': {xRot += (xRot == 360) ? -355 : 5; glRotatef(xRot, 1, 0, 0);break;}
        case 'w': {xRot -= (xRot == -360) ? 355 : 5; glRotatef(xRot, 1, 0, 0);break;}
        case 't': glutSpecialFunc(TranslateWithArrowKeys); return;
        case 'o': glutSpecialFunc(ParallelProjectionWithArrowKeys); return;
        case 'p': glutSpecialFunc(PerspectiveProjectionWithArrowKeys); return;
        case 'f': glutSpecialFunc(FrustumProjectionWithArrowKeys); return;
        case 'k': glutSpecialFunc(MoveCamera); return;
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
//    drawRobot();
    glutWireTeapot(.3);
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
//    glutTimerFunc(33, TimerFunction, 1);
    
    SetupRC();
    
    glutMainLoop();
    
    return 0;
}

