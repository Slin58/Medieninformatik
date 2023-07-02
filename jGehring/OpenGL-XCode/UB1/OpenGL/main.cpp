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
//#include "algorithm"
//#include "limits"
#include "/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3/Eigen/Dense"
using namespace::std;
//using namespace::Eigen;

#define PI 3.14159265

const int RESOLUTION = 256;


struct RGBPixel {
    GLfloat R, G, B;
    RGBPixel(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0) : R(r), G(g), B(b) {};
};

struct RGBAPixel {
    GLfloat R, G, B, A;
    RGBAPixel(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0, GLfloat a = 0) : R(r), G(g), B(b), A(a) {};
};
RGBAPixel framebuffer[RESOLUTION][RESOLUTION];


struct Point {
    int x, y;
    Point(int x = 0, int y = 0): x(x), y(y) {};
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

void basicBresenham(int x1, int y1, int x2, int y2){
    int dx = x2 - x1;
    int dy = y2 - y1;
    int const1 = 2 * dx;
    int const2 = 2 * dy;
    
    int error = const2 - dx;
    
    while(x1 <= x2){
        setPixel(x1, y1, 125, 125, 125);
        x1++;
        
        if(error <= 0){
            error += const2;
        } else{
            y1++;
            error += const2 - const1;
        }
    }
}

void fullUglyBresenham(int x1, int y1, int x2, int y2){
    int dx = x2 - x1;
    int dy = y2 - y1;
    int const1 = 2 * dx;
    int const2 = 2 * dy;
    bool isDxNegative = dx < 0;
    bool isDyNegative = dy < 0;
    bool dXgrowingFaster = abs(dx) > abs(dy);
    
    int error = const2 - dx;
    if(dXgrowingFaster){
        for(int i = 0; i <= abs(dx); i++){
            setPixel(x1, y1, 0, 0, 255);
            if(isDxNegative){
                x1--;
            } else{
                x1++;
            }
            if(error <= 0){
                error = error + (isDyNegative ? -const2 : const2);
            } else{
                y1 = y1 + (isDyNegative ? - 1 : 1);
                error = error + (isDyNegative ? - const2 : const2) - (isDxNegative ? - const1 : const1);
            }
        }
    } else{
        for(int i = 0; i <= abs(dy); i++){
            setPixel(x1, y1, 0, 0, 255);
            if(isDyNegative){
                y1--;
            } else{
                y1++;
            }
            if(error <= 0){
                error = error + (isDxNegative ? -const1 : const1);
            } else{
                x1 = x1 + (isDxNegative ? - 1 : 1);
                error = error + (isDxNegative ? - const1 : const1) - (isDyNegative ? - const2 : const2);
            }
        }
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

void midpoint(int x1, int y1, int x2, int y2){
    int dx = x2 - x1;
    int dy = y2 - y1;
    int f = 2*dy - dx;
    
    for(int i = x1; i <= x2; i++){
        setPixel(x1, y1, 125, 125, 125);
        x1++;
        if(f > 0){
            y1++;
            f -= 2*dx;
        }
        f += 2*dy;
    }
}


static void saveImage() {
    std::string fileString("P3 \n256 256\n255\n");

    for (int y = RESOLUTION; y >= 0; y--){
        for (int x = 0; x < RESOLUTION; x++) {
            fileString += std::to_string((int)(framebuffer[x][y].R)) + " ";
            fileString += std::to_string((int)(framebuffer[x][y].G)) + " ";
            fileString += std::to_string((int)(framebuffer[x][y].B)) + "\t";
        }
        fileString += "\n";
    }

    std::ofstream file("output.ppm");
    file << fileString;
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

    hPoint p0(10, 10, 1);
    hPoint p1(250, 250, 1);
    prettyBresenham(p0, p1, 250, 0, 0);

    saveImage();
    glutMainLoop();
    
    return 0;
}
