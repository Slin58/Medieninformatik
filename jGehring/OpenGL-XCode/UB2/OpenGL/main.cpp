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


void drawPolygon(const vector<Edge> &edges, int r, int g, int b){
    for(auto &edge : edges){
        hPoint b0 = hPoint(edge.x0, edge.y0, 1);
        hPoint b1 = hPoint(edge.x1, edge.y1, 1);
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

void midpointCircle(int x, int y, int radius, int r, int g, int b){
    int x1 = 0;
    int y1 = radius;
    int f = 1-radius;
    int dx = 3;
    int dy = 2 - 2*radius;
    while(x1<=y1) {
        setPixel(x+x1, y-y1, r, g, b);
        setPixel(x+x1, y+y1, r, g, b);
        setPixel(x-x1, y-y1, r, g, b);
        setPixel(x-x1, y+y1, r, g, b);
        setPixel(x+y1, y-x1, r, g, b);
        setPixel(x+y1, y+x1, r, g, b);
        setPixel(x-y1, y-x1, r, g, b);
        setPixel(x-y1, y+x1, r, g, b);
        x1++;
        if(f > 0) {
            y1--;
            f += dy;
            dy += 2;
        }
        f += dx;
        dx +=2;
        
    }
}


void bezierCurve(std::vector<Point> polygon){
    std::vector<std::vector<Point>> newPoints(polygon.size(), std::vector<Point>(polygon.size()));
    std::vector<Point> curve = {};
    
    for(float x = 0; x < RESOLUTION; x++){
        float t = x/RESOLUTION;
        for(int i = 0; i < polygon.size(); i++){
            newPoints[i][0] = polygon[i];
        }
        for(int j = 1; j < polygon.size(); j++){
            for (int i = j; i < polygon.size(); i++) {
                Point point = Point(0,0);
                point.x = round((1-t) * newPoints[i-1][j-1].x + t * newPoints[i][j-1].x);
                point.y = round((1-t) * newPoints[i-1][j-1].y + t * newPoints[i][j-1].y);
                newPoints[i][j] = point;
            }
        }
        curve.push_back(newPoints[polygon.size()-1][polygon.size()-1]);
    }
    
    drawPolygon(curve, 0, 255, 0);
}

void deBoor(vector<float> T, vector<Point> b, int n){
    auto m = b.size()-1;
    vector<vector<Point>> temp(m+n, std::vector<Point>(n));
    vector<Point> result = {};
    for(double t = 0.0; t <= T[m + 1]; t=t+0.01){
        int i = 0;
        while(!(T[i] <= t && t < T[i+1] && i <= m)){
            i++;
        }
        for(int j = 0; j <= n; j++){
            for(int l = i -n+j; l <=i; l++){
                if(j == 0) temp[l][0] = b[l];
                else{
                    float newT = (t - T[l])/(T[l+n+1-j] - T[l]);
                    temp[l][j].x = (1-newT) * temp[l-1][j-1].x + newT*temp[l][j-1].x;
                    temp[l][j].y = (1-newT) * temp[l-1][j-1].y + newT*temp[l][j-1].y;
                }
            }
        }
        result.push_back(temp[i][n]);
    }
    drawPolygon(result, 0, 255, 0);
}

void deBoorClosed(vector<float> T, vector<Point> b, int n) {
    int m = static_cast<int>(b.size()-1);
    vector<vector<Point>> temp(m+n, std::vector<Point>(n));
    vector<Point> result = {};
    for (double t = 0.0; t <= T[m+1]; t += 0.01) {
        double tMod = t;
        int i = 0;
        while (!(T[i] <= tMod && tMod < T[i + 1] && i <= m)) {
            i++;
        }
        for (int j = 0; j <= n; j++) {
            int l = i - n + j - 1;
            do{
                l++;
                if (l < 0) {
                    l += m + 1;
                    tMod = tMod - T[0] + T[m + 1];
                }
                else if (l >= m + 1) {
                    l -= m - 1;
                    tMod = tMod - T[m + 1] + T[0];
                }
                if (j == 0) {
                    temp[l][0] = b[l];
                }
                else {
                    int modulo = ((l - 1)%(m + 1) + (m + 1))%(m + 1);
                    float newT = (tMod - T[l]) / (T[l + n + 1 - j] - T[l]);
                    temp[l][j].x = (1 - newT) * temp[modulo][j - 1].x + newT * temp[l][j - 1].x;
                    temp[l][j].y = (1 - newT) * temp[modulo][j - 1].y + newT * temp[l][j - 1].y;
                }
            } while(l != i);
        }
        result.push_back(temp[i][n]);
    }
    drawPolygon(result, 0, 255, 0);
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
    std::vector<Point> polygon = {Point(30, 30), Point(70,400), Point(300, 400), Point(340, 30)};
//    drawPolygon(polygon, 255, 0, 0);
    vector<float> tParams = {0.0, 0.0, 0.0, 0.32, 0.49, 0.75, 1, 1, 1};
    deBoor(tParams, polygon, 2);
    
    glutMainLoop();
    
    return 0;
}


