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



const int RESOLUTION = 512;

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

int outcode( int x, int y, int xmin, int xmax, int ymin, int ymax ) {
    int c = 0;
    if (x < xmin) c = c | 8; // 1000
    if (x > xmax) c = c | 4; // 0100
    if (y < ymin) c = c | 2; // 0010
    if (y > ymax) c = c | 1; // 0001
    return c;
}

int outcode(hPoint &p, int xmin, int xmax, int ymin, int ymax){
    return outcode(p.vec[0], p.vec[1], xmin, xmax, ymin, ymax);
}

bool cohenSutherland(hPoint &p0, hPoint &p1, int r, int g, int b, int xmin = 0, int xmax = RESOLUTION, int ymin = 0, int ymax = RESOLUTION ){
    bool result = false;
    while(true){
        auto code0 = outcode(p0, xmin, xmax, ymin, ymax);
        auto code1 = outcode(p1, xmin, xmax, ymin, ymax);
        if ((code0 | code1) == 0){
            result = true;
            break;
        }
        else if ((code0 & code1) != 0) break;
        else {
            auto code = code0 > code1 ? code0 : code1;
            auto x = 0;
            auto y = 0;
            //y = mx+t
            //m = (y1 - y0)/(x1 - x0)
            //x = (y-t)/m
            //in the following formulas:
            // 1/m = 1/ ((y1 - y0)/(x1 - x0))
            // = (x1 - x0)/(y1 - y0)
            auto test = code & 1;
            if(code & 1){
                y = ymax;
                x = p0.vec[0] + (p1.vec[0] - p0.vec[0]) * (ymax - p0.vec[1])/(p1.vec[1] - p0.vec[1]);
            } else if(code & 2){
                y = ymin;
                x = p0.vec[0] + (p1.vec[0] - p0.vec[0]) * (ymin - p0.vec[1])/(p1.vec[1] - p0.vec[1]);
            }else if(code & 4){
                x = xmax;
                y = p0.vec[1] + (p1.vec[1] - p0.vec[1])*(xmax - p0.vec[0])/(p1.vec[0] - p0.vec[0]);
            } else {
                x = xmin;
                y = p0.vec[1] + (p1.vec[1] - p0.vec[1])*(xmin - p0.vec[0])/(p1.vec[0] - p0.vec[0]);
            }
            if(code == code0){
                p0 = hPoint(x, y, p0.vec[2]);
            }else{
                p1 = hPoint(x, y, p1.vec[2]);
            }
        }
    }
    return result;
}

hPoint computeIntersection(hPoint &prevPoint, hPoint &currPoint, Edge &clipEdge){
    return hPoint(0, 0);
}

bool inside(hPoint &point, Edge &clipEdge){
    return true;
}

vector<hPoint> clip(vector<hPoint>& polygon, Edge &clipEdge){
    vector<hPoint> result = {};

    for(auto i = 0; i < polygon.size(); i++){
        hPoint currentPoint = polygon[i];
        hPoint prevPoint = polygon[(i - 1) % polygon.size()];
        hPoint intersectingPoint = computeIntersection(prevPoint, currentPoint, clipEdge);
        if(inside(currentPoint, clipEdge)){
            if(!inside(prevPoint, clipEdge)) result.push_back(intersectingPoint);
            result.push_back(currentPoint);
        } else if(inside(prevPoint, clipEdge)) result.push_back(intersectingPoint);
    }
    return result;
}

vector<hPoint> sutherlandHodgman(vector<hPoint> &polygon, vector<hPoint> &border){
    for(int i = 1; i < border.size(); i++){
        Edge clipEdge(border[i-1].vec[0], border[i-1].vec[1], border[i].vec[0], border[i].vec[1]);
        polygon = clip(polygon, clipEdge);
    }
    return polygon;
}

//List outputList = subjectPolygon;
//
//for (Edge clipEdge in clipPolygon) do
//List inputList = outputList;
//outputList.clear();
//
//for (int i = 0; i < inputList.count; i += 1) do
//Point current_point = inputList[i];
//Point prev_point = inputList[(i − 1) % inputList.count];
//
//Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
//
//if (current_point inside clipEdge) then
//if (prev_point not inside clipEdge) then
//outputList.add(Intersecting_point);
//end if
//outputList.add(current_point);
//
//else if (prev_point inside clipEdge) then
//outputList.add(Intersecting_point);
//end if
//
//done
//done

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
        if(cohenSutherland(b0, b1, r, g, b))
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

void fillTriangle(const vector<hPoint> &triangle, const Vector3f &lowerLeft, const Vector3f &lowerRight, const Vector3f &upper){
    Point p0 = Point(triangle[0].vec[0], triangle[0].vec[1]);
    Point p1 = Point(triangle[1].vec[0], triangle[1].vec[1]);
    Point p2 = Point(triangle[2].vec[0], triangle[2].vec[1]);
    int xmin = min(p0.x, min(p1.x,p2.x));
    int ymin = min(p0.y, min(p1.y,p2.y));
    int xmax = max(p0.x, max(p1.x,p2.x));
    int ymax = max(p0.y, max(p1.y,p2.y));
    int f0 = (p0.y-p1.y)*(xmin-p0.x) + (p1.x-p0.x)*(ymin-p0.y);
    int f1 = (p1.y-p2.y)*(xmin-p1.x) + (p2.x-p1.x)*(ymin-p1.y);
    int f2 = (p2.y-p0.y)*(xmin-p2.x) + (p0.x-p2.x)*(ymin-p2.y);
    const float c = f0 + f1 + f2;
    cout << c << endl;
    for(int y = ymin; y<=ymax; y++) {
        int ff0 = f0, ff1 = f1, ff2 = f2;
        for(int x = xmin; x<=xmax; x++) {
            if(ff0 >= 0 && ff1 >=0 && ff2 >= 0){
                float ratio0 = (ff0/c), ratio1 = (ff1/c), ratio2 = (ff2/c);
                float r = ratio0*lowerLeft[0]+ratio1*lowerRight[0]+ratio2*upper[0], g = ratio0*lowerLeft[1]+ratio1*lowerRight[1]+ratio2*upper[1], b = ratio0*lowerLeft[2]+ratio1*lowerRight[2]+ratio2*upper[2];
                setPixel(x,y, r, g, b);
            }
            ff0 = ff0 + (p0.y-p1.y);
            ff1 = ff1 + (p1.y-p2.y);
            ff2 = ff2 + (p2.y-p0.y);
        }
        f0 = f0 + (p1.x-p0.x);
        f1 = f1 + (p2.x-p1.x);
        f2 = f2 + (p0.x-p2.x);
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

    
    std::vector<hPoint> polygon = {hPoint(30, 430, 1), hPoint(100, 200, 1), hPoint(350, 50), hPoint(450, 450), hPoint(30, 430)};
    std::vector<hPoint> border = {hPoint(100, 100), hPoint(400, 450), hPoint(500, 300), hPoint(400, 100), hPoint(100, 100)};
    //    fillTriangle(polygon, Vector3f(1.0, 0.0, 0.0), Vector3f(0.0, 1.0, 0.0), Vector3f(0.0, 0.0, 1.0));
    drawPolygon(border, 250, 0, 0);
//    drawPolygon(polygon, 0, 250, 0);
    polygon = sutherlandHodgman(polygon, border);
    drawPolygon(polygon, 0, 0, 250);
    glutMainLoop();
    
    return 0;
}


