//  main.cpp
//  OpenGL
//

#include "OpenGL/gl.h"
#include "GLUT/glut.h"			// Glut

#include <math.h>
#include <stdlib.h> 
#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "algorithm"
#include "limits"
#include "/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3/Eigen/Dense"
using namespace::std;
using namespace::Eigen;

#define PI 3.14159265



const int RESOLUTION = 200;

struct RGBPixel {
    GLfloat R, G, B;
    RGBPixel(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0) : R(r), G(g), B(b) {};
};

struct RGBAPixel {
    GLfloat R, G, B, A;
    RGBAPixel(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0, GLfloat a = 0) : R(r), G(g), B(b), A(a) {};
};

struct Point {
    int x, y;
    Point(int x = 0, int y = 0): x(x), y(y) {};
};

struct Edge{
    int x0,y0, x1, y1;
    Edge(int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0): x0(x0), y0(y0), x1(x1), y1(y1){};
};

struct hPoint{
    Eigen::Vector3f vec;
    hPoint(int x = 0, int y = 0, int w = 0): vec(x, y, w){};
};

bool compareHpointByX(const hPoint& a, const hPoint& b) {
    return a.vec[0] < b.vec[0];
}

hPoint transformToHomogenous(const Point &point){
    return hPoint(point.x, point.y, 1);
}

vector<hPoint> transformToHomogenous(const vector<Point> &points){
    vector<hPoint> result = {};
    for( auto point : points){
        result.push_back(transformToHomogenous(point));
    }
    return result;
}

RGBAPixel framebuffer[RESOLUTION][RESOLUTION];

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0) {
    framebuffer[x < 0 ? 0 : (x >= RESOLUTION ? RESOLUTION - 1 : x)][y < 0 ? 0 : (y >= RESOLUTION ? RESOLUTION - 1 : y)] =
    RGBAPixel(r, g, b, a);
    glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);
    
    GLfloat d = 2.0f / GLfloat(RESOLUTION);
    for (int y = 0; y < RESOLUTION; y++)
        for (int x = 0; x < RESOLUTION; x++) {
            const RGBAPixel& p = framebuffer[x][y];
            if (p.R == 0 && p.G == 0 && p.B == 0)
                continue;
            glColor4f(p.R, p.G, p.B, p.A);
            GLfloat vpx = GLfloat(x) * 2.0f / GLfloat(RESOLUTION) - 1.0f;
            GLfloat vpy = GLfloat(y) * 2.0f / GLfloat(RESOLUTION) - 1.0f;
            glRectf(vpx, vpy, vpx + d, vpy + d);
        }
    
    // Flush drawing commands
    glFlush();
}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    glClearColor(0.0,0.0,0.0,0.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void KeyPress(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q': exit(1);
    }
}

void prettyBresenham(Point a, Point b, int red, int green, int blue)
{
    int dx =  abs(b.x - a.x), sx = a.x < b.x ? 1 : -1;
    int dy = -abs(b.y - a.y), sy = a.y < b.y ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */
    
    while (1) {
        setPixel(a.x, a.y, red, green, blue);
        if (a.x == b.x && a.y == b.y) break;
        e2 = 2 * err;
        if (e2 > dy) { err += dy; a.x += sx; } /* e_xy+e_x> 0 */
        if (e2 < dx) { err += dx; a.y += sy; } /* e_xy+e_y < 0 */
    }
}

void prettyBresenham(hPoint a, hPoint b, int red, int green, int blue)
{
    Point pa = Point(static_cast<int>(round(a.vec[0])), static_cast<int>(round(a.vec[1])));
    Point pb = Point(static_cast<int>(round(b.vec[0])), static_cast<int>(round(b.vec[1])));
    prettyBresenham(pa, pb, red, green, blue);
}



void drawPolygon(const vector<Point> &points, int r, int g, int b){
    for(int i = 1; i < points.size(); i++){
        Point b0 = points[i - 1];
        Point b1 = points[i];
        prettyBresenham(b0, b1, r, g, b);
    }
}

void drawPolygon(const vector<hPoint> &points, int r, int g, int b){
    for(int i = 1; i < points.size(); i++){
        hPoint b0 = points[i - 1];
        hPoint b1 = points[i];
        prettyBresenham(b0, b1, r, g, b);
    }
}

void drawPolygon(const vector<Edge> &edges, int r, int g, int b){
    for(auto &edge : edges){
        hPoint b0 = hPoint(edge.x0, edge.y0, 1);
        hPoint b1 = hPoint(edge.x1, edge.y1, 1);
        prettyBresenham(b0, b1, r, g, b);
    }
}


void fillRectangle(int x0, int y0, int x1, int y1){
    for(; y0 <= y1; y0++){
        for(int x = x0; x <= x1; x++){
            setPixel(x, y0, 255, 0, 0);
        }
    }
}

int iPartOfNumber(float x)
{
    return (int)x;
}

//rounds off a number
int roundNumber(float x)
{
    return iPartOfNumber(x + 0.5) ;
}

//returns fractional part of a number
float fPartOfNumber(float x)
{
    if (x>0) return x - iPartOfNumber(x);
    else return x - (iPartOfNumber(x)+1);
    
}

//returns 1 - fractional part of number
float rfPartOfNumber(float x)
{
    return 1 - fPartOfNumber(x);
}

void swap(int* a , int*b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void wuDrawLine(const hPoint& p0, const hPoint& p1, int r, int g, int b){
    int x0 = p0.vec[0];
    int y0 = p0.vec[1];
    int x1 = p1.vec[0];
    int y1 = p1.vec[1];
    int steep = abs(y1 - y0) > abs(x1 - x0);
    
    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if (steep)
    {
        swap(&x0 , &y0);
        swap(&x1 , &y1);
    }
    if (x0 > x1)
    {
        swap(&x0 ,&x1);
        swap(&y0 ,&y1);
    }
    
    //compute the slope
    float dx = x1-x0;
    float dy = y1-y0;
    float gradient = dy/dx;
    if (dx == 0.0)
        gradient = 1;
    
    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;
    
    // main loop
    if (steep)
    {
        int x;
        for (x = xpxl1 ; x <=xpxl2 ; x++)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            setPixel(iPartOfNumber(intersectY), x,r,g,b, rfPartOfNumber(intersectY));
            setPixel(iPartOfNumber(intersectY)-1, x,r,g,b, fPartOfNumber(intersectY));
            intersectY += gradient;
        }
    }
    else
    {
        int x;
        for (x = xpxl1 ; x <=xpxl2 ; x++)
        {
            // pixel coverage is determined by fractional
            // part of y co-ordinate
            setPixel(x, iPartOfNumber(intersectY),
                     r,g,b, rfPartOfNumber(intersectY));
            setPixel(x, iPartOfNumber(intersectY)-1,
                     r,g,b,fPartOfNumber(intersectY));
            intersectY += gradient;
        }
    }
}

void newWu(const hPoint& p0, const hPoint& p1, int r, int g, int b){
    int x0 = p0.vec[0];
    int y0 = p0.vec[1];
    int x1 = p1.vec[0];
    int y1 = p1.vec[1];
    float i0 = 1.0;
    setPixel(x0, y0, r, g, b);
    setPixel(x1, y1, r, g, b);
    int D = 0;
    int oldD = D;
    float k = 1.0*(y1-y0)/(x1-x0);
    int d = floor(k*pow(2.0, 32)+0.5);
    while(x0 < x1){
        x0++;
        x1--;
        oldD = D;
        D += d;
        if(D < oldD){
            y0++;
            y1--;
        }
        i0 = abs(static_cast<float>(D)/std::numeric_limits<int>::max());
        setPixel(x0, y0, r, g, b, i0);
        setPixel(x1, y1, r, g, b, i0);
        
        setPixel(x0, y0+1, r, g, b, 1.0-i0);
        setPixel(x1, y1+1, r, g, b, 1.0-i0);
    }
}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("2D-Test");
    glutReshapeWindow(RESOLUTION, RESOLUTION);
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(KeyPress);
    
    SetupRC();
    ////////// put your framebuffer drawing code here /////////////
    

    hPoint p0 = hPoint(0, 0, 1);
    hPoint p1 = hPoint(200, 50, 1);
//    hPoint p2 = hPoint(255, 130, 1);
    hPoint p3 = hPoint(200, 30, 1);
//    wuDrawLine(p0, p2, 250, 0, 0);
    newWu(p0, p1, 0, 250, 0);
    prettyBresenham(p0, p3, 250, 0, 0);

    glutMainLoop();
    
    return 0;
}


