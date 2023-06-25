// Simple.cpp
// The Simplest OpenGL program with GLUT

#define FREEGLUT_STATIC

#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)

#include <math.h>
#include <cmath>
#include <iostream>
#include "WavefrontParser.hpp"

// draw a simple rectangle.
void drawRectangle(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glRectf(x0, y0, x1, y1);
}

///////////////////////////////////////////////////////////
// RECTANGLE FUNCTIONS

// rectangle coordinates
GLfloat x = 0.f;
GLfloat y = 0.f;
GLfloat xstep = 0.03f;
GLfloat ystep = 0.02f;

// repeated movement function
void RectangleMovementTimerFunction(int value) {
	if (x > 1 - 0.25 || x < -1 + 0.25) {
		xstep = -xstep;
	}
	if (y > 1 - 0.25 || y < -1 + 0.25) {
		ystep = -ystep;
	}

	x += xstep;
	y += ystep;

	glutPostRedisplay();
	glutTimerFunc(33, RectangleMovementTimerFunction, 1);
}

///////////////////////////////////////////////////////////
// Utah-Teapot functions - 6.1)

// coordinates
GLfloat teapotX = 0.f;
GLfloat teapotY = 0.f;
GLfloat teapotZ = 0.f;
GLfloat teapotStep = 0.02f;
GLfloat teapotRotateX = 0.f;
GLfloat teapotRotateY = 0.f;
GLfloat teapotRotateZ = 0.f;
GLfloat teapotRotateStep = 1.f;
GLfloat teapotCameraX = 0.f;
GLfloat teapotCameraY = 0.f;
GLfloat teapotCameraZ = 2.f;
GLboolean teapotIsPerspectiveCamera = true;
GLfloat teapotCameraNear = 0.1f;
GLfloat teapotCameraFar = 10.f;
GLfloat teapotCameraFovy = 45.f;
GLfloat teapotCameraLeft = -10.f;
GLfloat teapotCameraRight = 10.f;
GLfloat teapotCameraTop = 10.f;
GLfloat teapotCameraBottom = 10.f;

// draw function
void drawTeapot(GLfloat size, GLfloat r, GLfloat g, GLfloat b) {
	glMatrixMode(GL_MODELVIEW);
	glColor3f(r, g, b);

	glLoadIdentity();
	gluLookAt(teapotCameraX, teapotCameraY, teapotCameraZ, teapotCameraX, teapotCameraY, teapotCameraZ - 2, 0, 1, 0);
	glTranslatef(teapotX, teapotY, teapotZ);
	glRotatef(teapotRotateZ, 0, 0, 1);
	glRotatef(teapotRotateY, 0, 1, 0);
	glRotatef(teapotRotateX, 1, 0, 0);
	glutWireTeapot(size);
}

void teapotCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (teapotIsPerspectiveCamera) {
		gluPerspective(teapotCameraFovy, 1.f, teapotCameraNear, teapotCameraFar);
	}
	else {
		glOrtho(teapotCameraLeft, teapotCameraRight, teapotCameraBottom, teapotCameraTop, teapotCameraNear, teapotCameraFar);
	}
}

// keypress handling
// move teapot using wasd, up/down using q and e
// move camera using WASD, up/down using Q and E
// rotate teapot around X axis using 1/2
// rotate teapot around Y axis using 3/4
// rotate teapot around Z axis using 5/6
// switch between perspectivic and parallel camera using p
// move near/far limits using 7/8 and 9/0
// change fovy using x/y
// change perspectivic camera's L/R/T/B using c/C, v/V, b/B, n/N
void teapotHandleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		teapotZ -= teapotStep;
		break;
	case 'a':
		teapotX -= teapotStep;
		break;
	case 's':
		teapotZ += teapotStep;
		break;
	case 'd':
		teapotX += teapotStep;
		break;
	case 'q':
		teapotY += teapotStep;
		break;
	case 'e':
		teapotY -= teapotStep;
		break;
	case 'W':
		teapotCameraZ -= teapotStep;
		break;
	case 'A':
		teapotCameraX += teapotStep;
		break;
	case 'S':
		teapotCameraZ += teapotStep;
		break;
	case 'D':
		teapotCameraX -= teapotStep;
		break;
	case 'Q':
		teapotCameraY += teapotStep;
		break;
	case 'E':
		teapotCameraY -= teapotStep;
		break;
	case '1':
		teapotRotateX += teapotRotateStep;
		break;
	case '2':
		teapotRotateX -= teapotRotateStep;
		break;
	case '3':
		teapotRotateY += teapotRotateStep;
		break;
	case '4':
		teapotRotateY -= teapotRotateStep;
		break;
	case '5':
		teapotRotateZ += teapotRotateStep;
		break;
	case '6':
		teapotRotateZ -= teapotRotateStep;
		break;
	case 'p':
		teapotIsPerspectiveCamera = !teapotIsPerspectiveCamera;
		break;
	case '7':
		if (++teapotCameraNear >= teapotCameraFar) {
			teapotCameraNear--;
		}
		break;
	case '8':
		if (teapotCameraNear > 1) {
			teapotCameraNear--;
		}
		break;
	case '9':
		teapotCameraFar++;
		break;
	case '0':
		if (--teapotCameraFar <= teapotCameraNear) {
			teapotCameraFar++;
		}
		break;
	case 'y':
		teapotCameraFovy++;
		break;
	case 'x':
		if (teapotCameraFovy > 1) {
			teapotCameraFovy--;
		}
		break;
	case 'c':
		teapotCameraLeft--;
		break;
	case 'C':
		if (++teapotCameraLeft >= teapotCameraRight) {
			teapotCameraLeft--;
		}
		break;
	case 'v':
		teapotCameraRight++;
		break;
	case 'V':
		if (--teapotCameraRight <= teapotCameraLeft) {
			teapotCameraRight++;
		}
		break;
	case 'b':
		teapotCameraTop++;
		break;
	case 'B':
		if (--teapotCameraTop <= teapotCameraBottom) {
			teapotCameraTop++;
		}
		break;
	case 'n':
		teapotCameraBottom--;
		break;
	case 'N':
		if (++teapotCameraBottom >= teapotCameraTop) {
			teapotCameraBottom--;
		}
		break;
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// robot functions - 6.2)
GLfloat baseRot = 0.f;
GLfloat lUpperRot = 0.f;
GLfloat lLowerRot = 0.f;
GLfloat lHandRot = 0.f;
GLfloat rUpperRot = 0.f;
GLfloat rLowerRot = 0.f;
GLfloat rHandRot = 0.f;

void drawRobotBase() {
	// draw base
	glPushMatrix();
	glScalef(1.f, 0.5f, 0.5f);
	glColor3f(0.f, 0.f, 1.f);
	glutSolidCube(1.f);
	glPopMatrix();
}

void drawRobotUpperArm() {
	glPushMatrix();
	glScalef(0.1f, 0.3f, 0.1f);
	glColor3f(0.f, 1.f, 0.f);
	glutSolidCube(1.f);
	glPopMatrix();
}

void drawRobotLowerArm(bool isLeft) {
	glPushMatrix();
	glTranslatef(isLeft ? -0.15f : 0.15f, 0.f, 0.f);
	glScalef(0.4f, 0.1f, 0.1f);
	glColor3f(1.f, 0.f, 0.f);
	glutSolidCube(1.f);
	glPopMatrix();
}

void drawRobotHand() {
	glScalef(0.05f, 0.2f, 0.1f);
	glColor3f(1.f, 1.f, 0.f);
	glutSolidCube(1.f);
}

void drawRobot() {
	// setupA
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, 0.2, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.f, 0.2f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	// base transformations
	glTranslatef(0.f, -0.4f, 0.f);
	glRotatef(baseRot, 0.f, 1.f, 0.f);

	// draw base
	drawRobotBase();

	// finish up base transformations
	glTranslatef(0.f, 0.4f, 0.f);
	glPushMatrix();

	// left upper arm transformation
	glTranslatef(-0.3f, 0.f, 0.f);
	glRotatef(lUpperRot, 0.f, 1.f, 0.f);

	// draw left upper arm
	drawRobotUpperArm();

	// left lower arm transformation
	glTranslatef(0.f, 0.2f, 0.f);
	glRotatef(lLowerRot, 0.f, 1.f, 0.f);

	// draw left lower arm
	drawRobotLowerArm(true);

	// left hand transformations
	glTranslatef(-0.38f, 0.f, 0.f);
	glRotatef(lHandRot, 1.f, 0.f, 0.f);

	//draw left hand
	drawRobotHand();

	// back to base transformation
	glPopMatrix();

	// right upper arm transformation
	glTranslatef(0.3f, 0.f, 0.f);
	glRotatef(rUpperRot, 0.f, 1.f, 0.f);

	// draw right upper arm
	drawRobotUpperArm();

	// right lower arm transformation
	glTranslatef(0.f, 0.2f, 0.f);
	glRotatef(rLowerRot, 0.f, 1.f, 0.f);

	// draw right lower arm
	drawRobotLowerArm(false);

	// right hand transformations
	glTranslatef(0.38f, 0.f, 0.f);
	glRotatef(rHandRot, 1.f, 0.f, 0.f);

	// draw right hand 
	drawRobotHand();

}

GLfloat baseRotateStep = 0.05f;
GLfloat lUpperRotateStep = 0.2f;
GLfloat lLowerRotateStep = 0.4f;
GLfloat lHandRotateStep = -0.3f;
GLfloat rUpperRotateStep = 0.1f;
GLfloat rLowerRotateStep = 0.5f;
GLfloat rHandRotateStep = 0.4f;

// repeated movement function
void RobotMovementTimerFunction(int value) {
	if (std::abs(baseRot) > 30) {
		baseRotateStep = -baseRotateStep;
	}
	if (std::abs(lUpperRot) > 40) {
		lUpperRotateStep = -lUpperRotateStep;
	}
	if (std::abs(lLowerRot) > 70) {
		lLowerRotateStep = -lLowerRotateStep;
	}
	if (std::abs(lHandRot) > 160) {
		lHandRotateStep = -lHandRotateStep;
	}
	if (std::abs(rUpperRot) > 40) {
		rUpperRotateStep = -rUpperRotateStep;
	}
	if (std::abs(rLowerRot) > 70) {
		rLowerRotateStep = -rLowerRotateStep;
	}
	if (std::abs(rHandRot) > 160) {
		rHandRotateStep = -rHandRotateStep;
	}

	baseRot += baseRotateStep;
	lUpperRot += lUpperRotateStep;
	lLowerRot += lLowerRotateStep;
	lHandRot += lHandRotateStep;
	rUpperRot += rUpperRotateStep;
	rLowerRot += rLowerRotateStep;
	rHandRot += rHandRotateStep;

	glutPostRedisplay();
	glutTimerFunc(33, RobotMovementTimerFunction, 1);
}

GLfloat stepAmount = 1.f;
// manual rotation
// 1/2 to rotate base
// q/w to rotate left upper arm, Q/W to rotate right upper arm
// e/r to rotate left lower arm, E/R to rotate right lower arm
// t/z to rotate left hand, T/Z to rotate right hand
void RobotKeyboardHandlerFunction(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		baseRot += stepAmount;
		break;
	case '2':
		baseRot -= stepAmount;
		break;
	case 'q':
		lUpperRot += stepAmount;
		break;
	case 'w':
		lUpperRot -= stepAmount;
		break;
	case 'e':
		lLowerRot += stepAmount;
		break;
	case 'r':
		lLowerRot -= stepAmount;
		break;
	case 't':
		lHandRot += stepAmount;
		break;
	case 'z':
		lHandRot -= stepAmount;
		break;
	case 'Q':
		rUpperRot += stepAmount;
		break;
	case 'W':
		rUpperRot -= stepAmount;
		break;
	case 'E':
		rLowerRot += stepAmount;
		break;
	case 'R':
		rLowerRot -= stepAmount;
		break;
	case 'T':
		rHandRot += stepAmount;
		break;
	case 'Z':
		rHandRot -= stepAmount;
		break;
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// MODEL DRAWING FUNCIONS

GLfloat modelRotation = 0;
Wavefront::WavefrontParser parser = Wavefront::WavefrontParser();
std::string fname = "";

void drawModel()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, 0.1, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(modelRotation, 0, 1, 0);

	if (fname != "") {
		parser.render();
	}
}

void ModelKeyboardHandlerFunction(unsigned char key, int x, int y) 
{
	switch (key) {
	case 'a':
		modelRotation++;
		break;
	case 'd':
		modelRotation--;
		break;
	case 'y':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/sphere.obj";
		parser.parse(fname);
		break;
	case 'x':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/cube.obj";
		parser.parse(fname);
		break;
	case 'c':
		fname = "D:/programmspass/studium/computergrafik/testfiles/Wavefront Datasets CG/datasets/lucy/lucy_low.obj";
		parser.parse(fname);
		break;
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// RENDER FUNCTIONS

// Called on size change
void ChangeSize(GLsizei width, GLsizei height)
{
	if (width > height) {
		glViewport(0, 0, height, height);
	}
	else {
		glViewport(0, 0, width, width);
	}
}

// Called to draw scene
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Blatt 5
	// drawRectangle(x - 0.25f, y - 0.25f, x + 0.25f, y + 0.25f, 1.f, 0.f, 0.f);

	// Blatt 6
	// Aufgabe 1
	/*
	teapotCamera();
	drawTeapot(0.5f, 1.f, 0.f, 1.f);
	*/

	// Aufgabe 2
	/*
	drawRobot();
	*/

	// Aufgabe 3
	drawModel();

	// Flush drawing commands
	glFlush();
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////
// MAIN LOOP

// Main program entry point
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Simple");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	// Blatt 5
	// glutTimerFunc(33, RectangleMovementTimerFunction, 1);

	// Blatt 6
	// Aufgabe 1
	// glutKeyboardFunc(teapotHandleKeypress);

	// Aufgabe 2
	/*
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glutTimerFunc(33, RobotMovementTimerFunction, 1);
	// glutKeyboardFunc(RobotKeyboardHandlerFunction);
	*/

	// Aufgabe 3
	glEnable(GL_DEPTH_TEST);
	glutKeyboardFunc(ModelKeyboardHandlerFunction);

	SetupRC();

	glutMainLoop();

	return 0;
}

