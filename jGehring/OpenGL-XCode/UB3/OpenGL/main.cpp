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



const int RESOLUTION = 264;

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

struct PassiveEdge{
    int xmin, ymin, xmax, ymax;
    PassiveEdge(int xmin, int ymin,int xmax,int ymax): xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax){};
};
struct ActiveEdge{
    int ymax;
    float xs,dx;
    ActiveEdge(float xs, int ymax, float dx): xs(xs), ymax(ymax), dx(dx){};
};

bool compareByYmin(const PassiveEdge &a, const PassiveEdge &b){
    return a.ymin < b.ymin;
}

bool compareByXs(const ActiveEdge &a, const ActiveEdge &b){
    return a.xs < b.xs;
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

void fillTriangle(Point p0, Point p1, Point p2, int r, int g, int b){
    int xmin = min(p0.x, min(p1.x,p2.x));
    int ymin = min(p0.y, min(p1.y,p2.y));
    int xmax = max(p0.x, max(p1.x,p2.x));
    int ymax = max(p0.y, max(p1.y,p2.y));
    int f0 = (p0.y-p1.y)*(xmin-p0.x) + (p1.x-p0.x)*(ymin-p0.y);
    int f1 = (p1.y-p2.y)*(xmin-p1.x) + (p2.x-p1.x)*(ymin-p1.y);
    int f2 = (p2.y-p0.y)*(xmin-p2.x) + (p0.x-p2.x)*(ymin-p2.y);
    for(int y = ymin; y<=ymax; y++) {
        int ff0 = f0, ff1 = f1, ff2 = f2;
        for(int x = xmin; x<=xmax; x++) {
            if(ff0 >= 0 && ff1 >=0 && ff2 >= 0) setPixel(x,y, r,g,b);
            ff0 = ff0 + (p0.y-p1.y);
            ff1 = ff1 + (p1.y-p2.y);
            ff2 = ff2 + (p2.y-p0.y);
        }
        f0 = f0 + (p1.x-p0.x);
        f1 = f1 + (p2.x-p1.x);
        f2 = f2 + (p0.x-p2.x);
    }
}

void fillTriangle(vector<hPoint> &triangle, int r, int g, int b){
    Point p0 = Point(triangle[0].vec[0], triangle[0].vec[1]);
    Point p1 = Point(triangle[1].vec[0], triangle[1].vec[1]);
    Point p2 = Point(triangle[2].vec[0], triangle[2].vec[1]);
    fillTriangle(p0, p1, p2, r, g, b);
}

void scanline(const vector<Edge> &polygon, int r, int g, int b){
    
    //init list of passive edges and sort it
    vector<PassiveEdge> passiveEdges = {};
    for(auto &edge : polygon){
        int xmin = edge.x0;
        int ymin = edge.y0;
        int xmax = edge.x1;
        int ymax = edge.y1;
        
        PassiveEdge pEdge = PassiveEdge(xmin, ymin, xmax, ymax);
        passiveEdges.push_back(pEdge);
    }
    sort(passiveEdges.begin(), passiveEdges.end(), compareByYmin);
    
    int y = passiveEdges[0].ymin;
    int limit = passiveEdges.back().ymax;
    vector<ActiveEdge> activeEdges = {};
    
    while(y <= limit){
        //move passive edges to active
        for(auto &edge : passiveEdges){
            if(edge.ymin == y){
                float xmax = edge.xmax;
                float dx = (xmax - edge.xmin)/(edge.ymax - edge.ymin);
                ActiveEdge activeEdge = ActiveEdge(edge.xmin, edge.ymax, dx);
                activeEdges.push_back(activeEdge);
            }
        }
        //remove non-active edges
        for(auto iter = activeEdges.begin(); iter != activeEdges.end();){
            if(iter->ymax == y){
                iter = activeEdges.erase(iter);
            } else{
                ++iter;
            }
        }
        
        sort(activeEdges.begin(), activeEdges.end(), compareByXs);
        
        bool isDrawing = true;
        
        for(int i = 1; i <= activeEdges.size(); i++){
            if(isDrawing){
                for(int x = ceil(activeEdges[i -1].xs); x <= floor(activeEdges[i].xs); x++){
                    setPixel(x, y, r, g, b);
                }
            }
            isDrawing = !isDrawing;
        }
        
        for(auto &edge : activeEdges){
            edge.xs += edge.dx;
        }
        y++;
    }
}

void scanline(const vector<hPoint> &polygon, int r, int g, int b){
    
    //init list of passive edges and sort it
    vector<PassiveEdge> passiveEdges = {};
    for(int i = 1; i <= polygon.size(); i++){
        int xmin = min(polygon[i].vec[0], polygon[i-1].vec[0]);
        int ymin = min(polygon[i].vec[1], polygon[i-1].vec[1]);
        int xmax = max(polygon[i].vec[0], polygon[i-1].vec[0]);
        int ymax = max(polygon[i].vec[1], polygon[i-1].vec[1]);
        
        PassiveEdge edge = PassiveEdge(xmin, ymin, xmax, ymax);
        passiveEdges.push_back(edge);
    }
    sort(passiveEdges.begin(), passiveEdges.end(), compareByYmin);
    
    int y = passiveEdges[0].ymin;
    int limit = passiveEdges.back().ymax;
    vector<ActiveEdge> activeEdges = {};
    
    while(y <= limit){
        //move passive edges to active
        for(auto &edge : passiveEdges){
            if(edge.ymin == y){
                float xmax = edge.xmax;
                float dx = (xmax - edge.xmin)/(edge.ymax - edge.ymin);
                ActiveEdge activeEdge = ActiveEdge(edge.xmin, edge.ymax, dx);
                activeEdges.push_back(activeEdge);
            }
        }
        //remove non-active edges
        for(auto iter = activeEdges.begin(); iter != activeEdges.end();){
            if(iter->ymax == y){
                iter = activeEdges.erase(iter);
            } else{
                ++iter;
            }
        }
        
        sort(activeEdges.begin(), activeEdges.end(), compareByXs);
        
        bool isDrawing = true;
        
        for(int i = 1; i <= activeEdges.size(); i++){
            if(isDrawing){
                for(int x = ceil(activeEdges[i -1].xs); x <= floor(activeEdges[i].xs); x++){
                    setPixel(x, y, r, g, b);
                }
            }
            isDrawing = !isDrawing;
        }
        
        for(auto &edge : activeEdges){
            edge.xs += edge.dx;
        }
        y++;
    }
}

hPoint translate(int tx, int ty, hPoint point){
    Matrix3f s = (Matrix3f() << 1, 0, tx, 0, 1, ty, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> translate(int tx, int ty, vector<hPoint> points){
    vector<hPoint> result = {};
    for(auto point : points){
        result.push_back(translate(tx, ty, point));
    }
    return result;
}

hPoint scale(int sx, int sy, hPoint point){
    Matrix3f s = (Matrix3f() << sx, 0, 0, 0, sy, 0, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> scale(int sx, int sy, vector<hPoint> points){
    vector<hPoint> result = {};
    for(auto point : points){
        result.push_back(scale(sx, sy, point));
    }
    return result;
}

hPoint rotate(int angle, hPoint point){
    float fAngle = (angle*PI/180);
    Matrix3f s = (Matrix3f() << cos(fAngle), -sin(fAngle), 0, sin(fAngle), cos(fAngle), 0, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> rotate(int angle, vector<hPoint> points){
    vector<hPoint> result = {};
    for(auto point : points){
        result.push_back(rotate(angle, point));
    }
    return result;
}

hPoint shear(float kx, float ky, hPoint point){
    Matrix3f s = (Matrix3f() << 1, kx, 0,ky, 1, 0, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> shear(float kx, float ky, vector<hPoint> points){
    int x = points[0].vec[0];
    int y = points[0].vec[1];
    points = translate(-x, -y, points);
    vector<hPoint> result = {};
    for(auto point : points){
        result.push_back(shear(kx, ky, point));
    }
    result = translate(x, y, result);
    return result;
}

void pythagoras(){
    vector<hPoint> triangle = {hPoint(0, 0, 1), hPoint(40, 0,1), hPoint(0, 30, 1), hPoint(0,0,1)};
    triangle = translate(50, 50, triangle);
    //fillTriangle(triangle, 250, 0, 0);
    
    vector<hPoint> square1 = {hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1)};
    square1 = scale(3, 3, square1);
    square1 = translate(triangle[0].vec[0], triangle[0].vec[1], square1);
    square1 = translate(-(square1[3].vec[0] - square1[4].vec[0]), -(square1[3].vec[1] - square1[4].vec[1]), square1);
    
    vector<hPoint> square2 = {hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1)};
    square2 = scale(4, 4, square2);
    square2 = translate(triangle[0].vec[0], triangle[0].vec[1], square2);
    square2 = translate(0, -(square2[1].vec[1] - square2[0].vec[1]), square2);
    
    vector<hPoint> square3 = {hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1)};
    square3 = scale(5, 5, square3);
    float alphaRad = atan(static_cast<float>(triangle[2].vec[1]-triangle[0].vec[1])/static_cast<float>(triangle[1].vec[0]-triangle[0].vec[0]));
    int alpha = static_cast<int>(alphaRad*180/PI);
    square3 = rotate(90-alpha, square3);
    square3 = translate(triangle[1].vec[0], triangle[1].vec[1], square3);
    
    drawPolygon(triangle, 250, 0, 0);
    drawPolygon(square1, 250, 0, 0);
    drawPolygon(square2, 250, 0, 0);
    drawPolygon(square3, 250, 0, 0);
}

void pythagoras(int angle, int tx, int ty, int sx, int sy, int kx, int ky){
    vector<hPoint> triangle = {hPoint(0, 0, 1), hPoint(40, 0,1), hPoint(0, 30, 1), hPoint(0,0,1)};
    triangle = rotate(angle, triangle);
    triangle = shear(kx, ky, triangle);
    triangle = scale(sx, sy, triangle);
    triangle = translate(tx, ty, triangle);
    fillTriangle(triangle, 250, 0, 0);
    
    vector<hPoint> square1 = {hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1)};
    square1 = rotate(angle, square1);
    square1 = shear(kx, ky, square1);
    square1 = scale(sx, sy, square1);
    square1 = scale(3, 3, square1);
    square1 = translate(tx, ty, square1);
    square1 = translate(-(square1[3].vec[0] - square1[4].vec[0]), -(square1[3].vec[1] - square1[4].vec[1]), square1);
    
    vector<hPoint> square2 = {hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1)};
    square2 = rotate(angle, square2);
    square2 = shear(kx, ky, square2);
    square2 = scale(sx, sy, square2);
    square2 = scale(4, 4, square2);
    square2 = translate(tx, ty, square2);
    square2 = translate((square2[0].vec[0] - square2[1].vec[0]), -(square2[1].vec[1] - square2[0].vec[1]), square2);
    
    vector<hPoint> square3 = {hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1)};
    square3 = rotate(angle, square3);
    float alphaRad = atan(static_cast<float>(triangle[2].vec[1]-triangle[0].vec[1])/static_cast<float>(triangle[1].vec[0]-triangle[0].vec[0]));
    int alpha = static_cast<int>(alphaRad*180/PI);
    square3 = rotate(90-alpha, square3);
    square3 = shear(kx, ky, square3);
    square3 = scale(sx, sy, square3);
    square3 = scale(5, 5, square3);
    square3 = translate(tx, ty, square3);
    square3 = translate(triangle[1].vec[0] - square3[0].vec[0], triangle[1].vec[1] - square3[0].vec[1], square3);
    
    drawPolygon(triangle, 250, 0, 0);
    drawPolygon(square1, 0, 0, 255);
    drawPolygon(square2, 0, 255, 0);
    drawPolygon(square3, 0, 125, 125);
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
    pythagoras();
//    fillRectangle(40, 40, 300, 300);
    
    glutMainLoop();
    
    return 0;
}


