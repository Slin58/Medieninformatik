// Simple.cpp
// The Simplest OpenGL program with GLUT

#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)
#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std; 

GLfloat xpos = 0;
GLfloat ypos = 0;
GLfloat zpos = 0;
GLfloat xstep = 0.03f;
GLfloat ystep = 0.02f;

GLfloat rotx = 0;
GLfloat roty = 0;
GLfloat rotz = 0;
GLfloat tz = 0;

///////////////////////////////////////////////////////////
// Called to draw scene

float matCol[] = { 1,0,0,0 };

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glTranslatef(xpos, ypos, zpos);
	//glRotatef(45, 1, 1, 0);	// ??
	//glRotatef(rotx, roty, rotz, 1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matCol); 
	//glutWireTeapot(0.5);


	glPopMatrix();
	glFlush();
}

GLfloat tr_base_x = 10.0f; 
GLfloat tr_base_y = 10.0f;




void keyPress(unsigned char k, int x, int y)
{
	switch (k) {
	case 'w': ypos += 1; break;
	case 's': ypos -= 1; break;
	case 'a': xpos += 1; break;
	case 'd': xpos -= 1; break;
	case 'e': zpos += 1; break;
	case 'r': zpos -= 1; break; 
	case 'l': rotx += 10; break;
	case 'i': roty += 10; break;
	case 'u': rotz += 10; break;
	case 'j': rotx -= 10; break;
	case 'k': roty -= 10; break;
	case 'o': rotz -= 10; break;
	case '+': tz += 0.1f; break;
	case '-': tz -= 0.1f; break;
	case 'q': exit(1);
	}
	glutPostRedisplay();
}

void onIdle() {
	//rot += 0.01;
	//glutPostRedisplay();
}

void init() {
	float pos[] = { 1,1,1,0 };
	float white[] = { 1,1,1,0 };
	float shini[] = { 70 };

	glClearColor(.5, .5, .5, 0);
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SHININESS, shini);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

int main(int argC, char* argV[])
{
	glutInit(&argC, argV);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(900, 700);
	//glutCreateWindow("The Amazing, Rotating Utah Teapot brought to you in OpenGL via freeglut.");
	init();

	//glutKeyboardFunc(keyPress);

	//glutDisplayFunc(display);
	//glutIdleFunc(onIdle);
	glutMainLoop();
	return 0;
}

/* 
void readWavefrontObj(string fPath) {
	std::ifstream input(fPath);

	string line = "";
	getline(input, line);

	while (!line.empty())
	{
		for each line in input...


		getline(input, line);
	}
	cout << "bruh lol lmao" << endl;
}
*/ 
