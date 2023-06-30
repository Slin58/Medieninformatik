// 2D
// Simple framework for 2D-Drawing

#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
# define PI           3.14159265358979323846  /* pi */

#include <windows.h>		// Must have for Windows platform builds
#include "gl/glew.h"
#include "glut.h"			// Glut (Free-Glut on Windows)

#include <math.h>
#include <cmath>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include<eigen-3.4.0\Eigen\Dense>
#include <fstream>

using namespace std; 
using namespace Eigen;

const int RESOLUTION = 128;
const int INSIDE = 0; // 0000
const int LEFT = 8;    // 1000
const int RIGHT = 4;  // 0100
const int BOTTOM = 2; // 0010
const int TOP = 1;    // 0001


struct RGBPixel {
	GLfloat R, G, B;
	RGBPixel(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0) : R(r), G(g), B(b) {};
};


RGBPixel framebuffer[RESOLUTION][RESOLUTION];

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
	framebuffer[x < 0 ? 0 : (x >= RESOLUTION ? RESOLUTION - 1 : x)][y < 0 ? 0 : (y >= RESOLUTION ? RESOLUTION - 1 : y)] =
		RGBPixel(r, g, b);
	glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{
	 //Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat d = 2.0f / GLfloat(RESOLUTION);
	for (int y = 0; y < RESOLUTION; y++)
		for (int x = 0; x < RESOLUTION; x++) {
			const RGBPixel& p = framebuffer[x][y];
			if (p.R == 0 && p.G == 0 && p.B == 0)
				continue;
			glColor3f(p.R, p.G, p.B);
			GLfloat vpx = GLfloat(x) * 2.0f / GLfloat(RESOLUTION) - 1.0f;
			GLfloat vpy = GLfloat(y) * 2.0f / GLfloat(RESOLUTION) - 1.0f;
			glRectf(vpx, vpy, vpx + d, vpy + d);
		}

	// Flush drawing commands
    glFlush();


   /* glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(-0.25f, -0.25f, 0.25f, 0.25f);
    glFlush();*/

}

///////////////////////////////////////////////////////////
// Setup the rendering state
void SetupRC(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

}

void KeyPress(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q': exit(1);
	}
}


struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {};
};

//struct hPoint {
//    float x, y, w;
//    hPoint(float xVal, float yVal, float wVal) : x(xVal), y(yVal), w(wVal) {}; 
//    hPoint(Point p) : x(p.x), y(p.y), w(1) {};
//
//};

struct hPoint {
    Eigen::Vector3f vec;
    hPoint(int x = 0, int y = 0, int w = 0) : vec(x, y, w) {};
};


Point pointHToPoint(hPoint ph) {
    return Point(ph.vec[0] / ph.vec[2], ph.vec[1] / ph.vec[2]); 
}


struct Edge {
    int x0, y0, x1, y1;
    Edge(int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0) : x0(x0), y0(y0), x1(x1), y1(y1) {};
};

struct PassiveEdge {
    int xmin, ymin, xmax, ymax;
    PassiveEdge(int xmin, int ymin, int xmax, int ymax) : xmin(xmin), ymin(ymin), xmax(xmax), ymax(ymax) {};
};
struct ActiveEdge {
    int ymax;
    float xs, dx;
    ActiveEdge(float xs, int ymax, float dx) : xs(xs), ymax(ymax), dx(dx) {};
};

struct Line {
    Point p1, p2;
    Line(Point p1 = Point(0, 0), Point p2 = Point(0, 0)) : p1(p1), p2(p2) {};
};


bool compareByYmin(const PassiveEdge& a, const PassiveEdge& b) {
    return a.ymin < b.ymin;
}

bool compareByXs(const ActiveEdge& a, const ActiveEdge& b) {
    return a.xs < b.xs;
}

bool compareByY(const Point& a, const Point& b) {
    return a.y < b.y;
}


///////////////////////////////////////////////////////////
// Main program entry point

void prettyBresenham(Point b1, Point b2, int r, int b, int g); 
void prettyBresenham(hPoint a, hPoint b, int red, int green, int blue); 
void deBoor(std::vector<float> T, std::vector<Point> b, int n); 
void drawPolygon(std::vector<Point> points, int r, int g, int b); 
void drawPolygon(const std::vector<Edge>& edges, int r, int g, int b); 
void drawPolygonNewSuperSpecialEditionNotDumbISwear(std::vector<Point> points, int r, int g, int b); 
hPoint translate(int tx, int ty, hPoint point);
vector<hPoint> translate(int tx, int ty, vector<hPoint> points);
hPoint scale(int sx, int sy, hPoint point);
vector<hPoint> scale(int sx, int sy, vector<hPoint> points);
hPoint rotate(int angle, hPoint point);
vector<hPoint> rotate(int angle, vector<hPoint> points);
hPoint shear(float kx, float ky, hPoint point);
vector<hPoint> shear(float kx, float ky, vector<hPoint> points); 
void pythagoras();
void scanline(const std::vector<Edge>& polygon, int r, int g, int b);
void wrongCohenSutherlandClippingDoNotUse(std::vector<Point> const& figurePoint, int umin, int umax, int vmin, int vmax); 
void cohenSutherlandLineClip(std::vector<Point> const& figurePoint, int xmin, int xmax, int ymin, int ymax); 
int Outcode(int x, int y, int umin, int umax, int vmin, int vmax);
float getM(Point p1, Point p2);
Point getXs(Point p1, Point p2, float m, float c, float xBorder);
Point getYs(Point p1, Point p2, float m, float c, float yBorder);
Point getIntersectionPoint(Point p1, Point p2, int xmin, int xmax, int ymin, int ymax); 
void sutherlandHodgmanClipping(std::vector<Point> const& figurePoint, int xmin, int xmax, int ymin, int ymax);
vector<Point> getIntersectionPointForBothPointsOutOfBounds(Point p1, Point p2, int xmin, int xmax, int ymin, int ymax); 
bool compareByY(const Point& a, const Point& b); 
void rasterungDreieckWithColour(Point p0, Point p1, Point p2, int r0, int g0, int b0, int r1, int g1, int b1, int r2, int g2, int b2);
void plot(int x, int y, float r, float g, float b, float c);
float ipart(float x);
int roundToInteger(float num);
float rfpart(float x);
float fpart(float x); 
void drawLine(int x0, int y0, int x1, int y1); 

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
 	glutCreateWindow("2D-Test");
	glutReshapeWindow(512, 512);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(KeyPress);

	SetupRC();
////////// put your framebuffer drawing code here /////////////

	//for (int n = 0; n < RESOLUTION; n++)
	//	setPixel(n, n, GLfloat(n) / RESOLUTION, 0, 1.0f - GLfloat(n) / RESOLUTION);

   /* std::vector<Point> polygon = { Point(30, 30), Point(70,400), Point(300, 400), Point(340, 30), Point(400, 30), Point(430, 450) };
    drawPolygon(polygon, 255, 0, 0);
    std::vector<float> tParams = { 0.0, 0.0, 0.0, 0.02, 0.49, 0.75, 1, 1, 1 };
    deBoor(tParams, polygon, 2);*/

  // pythagoras(); 
  

    //std::vector<Point> frame = { Point(50,50), Point(200,50), Point(200,150), Point(50,150), Point(50,50) };
    //drawPolygonNewSuperSpecialEditionNotDumbISwear(frame, 0, 255, 0);

    //std::vector<Point> figurePoint = { Point(70,70), Point(120,70), Point(100, 250) };
    //std::vector<Point> figurePoint = { Point(60,80), Point(150,40), Point(100, 220) };
    //std::vector<Point> rectangle = { Point(20,20), Point(230,20), Point(230,230), Point(20,230), Point(20,20) };
    //drawPolygonNewSuperSpecialEditionNotDumbISwear(figurePoint, 0, 255, 255);
    //drawPolygon(rectangle, 0, 0, 250); 
    //cohenSutherlandLineClip(rectangle, 50, 200, 50, 150);
 
    //sutherlandHodgmanClipping(figurePoint, 50, 200, 50, 150);
   /* std::vector<Edge> polygon = {Edge(30, 5, 15, 30), Edge(15, 30, 15, 35), Edge(15, 35, 22, 45), Edge(30, 35, 22, 45), Edge(30, 35, 38, 45), Edge(45, 35, 38, 45), Edge(45, 30, 45, 35), Edge(30, 5, 45, 30)};
    scanline(polygon, 255, 0, 0);
    drawPolygon(polygon, 255, 0, 0); */


    //rasterungDreieckWithColour(Point(50, 50), Point(140, 50), Point(70, 120), 1, 0, 0, 0, 1, 0, 0, 0, 1);

    //drawLine(20, 20, 80, 80);




/////////////////////////////////
	glutMainLoop();
    
    return 0;
}

void drawPolygon(std::vector<Point> points, int r, int g, int b) {
    for (int i = 1; i < points.size(); i++) {
        Point b0 = points[i - 1];
        Point b1 = points[i];
        prettyBresenham(b0, b1, r, g, b);
    }
}
void drawPolygon(const std::vector<Edge>& edges, int r, int g, int b) {
    for (auto& edge : edges) {
        hPoint b0 = hPoint(edge.x0, edge.y0, 1);
        hPoint b1 = hPoint(edge.x1, edge.y1, 1);
        prettyBresenham(b0, b1, r, g, b);
    }
}
void drawPolygon(std::vector<hPoint> points, int r, int g, int b) {
    for (int i = 1; i < points.size(); i++) {
        Point b0 = pointHToPoint(points[i - 1]);
        Point b1 = pointHToPoint(points[i]);
        prettyBresenham(b0, b1, r, g, b);
    }
}
void drawPolygonNewSuperSpecialEditionNotDumbISwear(std::vector<Point> points, int r, int g, int b) {
    for (int i = 1; i < points.size(); i++) {
        if (i == points.size() - 1) {
            Point b0 = points[i];
            Point b1 = points[0];
            prettyBresenham(b0, b1, r, g, b);
        }
        Point b0 = points[i - 1];
        Point b1 = points[i];
        prettyBresenham(b0, b1, r, g, b);
    }
}
void prettyBresenham(hPoint a, hPoint b, int red, int green, int blue) {
    prettyBresenham(pointHToPoint(a), pointHToPoint(b), red, green, blue);
}


void prettyBresenham(Point a, Point b, int red, int green, int blue)
{
    int dx = abs(b.x - a.x), sx = a.x < b.x ? 1 : -1;
    int dy = -abs(b.y - a.y), sy = a.y < b.y ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    while (1) {
        setPixel(a.x, a.y, red, green, blue);
        if (a.x == b.x && a.y == b.y) break;
        e2 = 2 * err;
        if (e2 > dy) { err += dy; a.x += sx; } /* e_xy+e_x > 0 */
        if (e2 < dx) { err += dx; a.y += sy; } /* e_xy+e_y < 0 */
    }
}

void rasterungDreieckWithColour(Point p0, Point p1, Point p2, int r0, int g0, int b0, int r1, int g1, int b1, int r2, int g2, int b2) { //Punkte müssen gegen Uhrzeigersinn angegeben werden
    int xmin = min(p0.x, min(p1.x, p2.x));
    int ymin = min(p0.y, min(p1.y, p2.y));
    int xmax = max(p0.x, max(p1.x, p2.x));
    int ymax = max(p0.y, max(p1.y, p2.y));

    int f0 = (p0.y - p1.y) * (xmin - p0.x) + (p1.x - p0.x) * (ymin - p0.y); //initial decider
    int f1 = (p1.y - p2.y) * (xmin - p1.x) + (p2.x - p1.x) * (ymin - p1.y);
    int f2 = (p2.y - p0.y) * (xmin - p2.x) + (p0.x - p2.x) * (ymin - p2.y);

    double alpha; double beta; double gamma; float r; float g; float b;

    for (int y = ymin; y <= ymax; y++) {
        int ff0 = f0, ff1 = f1, ff2 = f2; //decider
        for (int x = xmin; x <= xmax; x++) {
            if (ff0 >= 0 && ff1 >= 0 && ff2 >= 0) {

                // Compute the barycentric coordinates of the current pixel
                alpha = ((float)(p1.y - p2.y) * (x - p2.x) + (p2.x - p1.x) * (y - p2.y)) /
                    ((float)(p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y));
                beta = ((float)(p2.y - p1.y) * (x - p2.x) + (p0.x - p2.x) * (y - p2.y)) /
                    ((float)(p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y));
                gamma = 1 - alpha - beta;

                // Caculate the r,g and b value
                r = r0 * alpha + r1 * beta + r2 * gamma;
                g = g0 * alpha + g1 * beta + g2 * gamma;
                b = b0 * alpha + b1 * beta + b2 * gamma;

                setPixel(x, y, r, g, b);
            }

            ff0 = ff0 + (p0.y - p1.y);
            ff1 = ff1 + (p1.y - p2.y);
            ff2 = ff2 + (p2.y - p0.y);
        }
        f0 = f0 + (p1.x - p0.x);
        f1 = f1 + (p2.x - p1.x);
        f2 = f2 + (p0.x - p2.x);
    }
}


void deBoor(std::vector<float> T, std::vector<Point> b, int n) {
    auto m = b.size() - 1;
    std::vector<std::vector<Point>> temp(m + n, std::vector<Point>(n));
    std::vector<Point> result = {};
    for (double t = 0.0; t <= T[m + 1]; t = t + 0.01) {
        int i = 0;
        while (!(T[i] <= t && t < T[i + 1] && i <= m)) {
            i++;
        }
        for (int j = 0; j <= n; j++) {
            for (int l = i - n + j; l <= i; l++) {
                if (j == 0) temp[l][0] = b[l];
                else {
                    float newT = (t - T[l]) / (T[l + n + 1 - j] - T[l]);
                    temp[l][j].x = (1 - newT) * temp[l - 1][j - 1].x + newT * temp[l][j - 1].x;
                    temp[l][j].y = (1 - newT) * temp[l - 1][j - 1].y + newT * temp[l][j - 1].y;
                }
            }
        }
        result.push_back(temp[i][n]);
    }
    drawPolygon(result, 0, 255, 0);
}

hPoint translate(int tx, int ty, hPoint point) {
    Matrix3f s = (Matrix3f() << 1, 0, tx, 0, 1, ty, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> translate(int tx, int ty, vector<hPoint> points) {
    vector<hPoint> result = {};
    for (auto point : points) {
        result.push_back(translate(tx, ty, point));
    }
    return result;
}

hPoint scale(int sx, int sy, hPoint point) {
    Matrix3f s = (Matrix3f() << sx, 0, 0, 0, sy, 0, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> scale(int sx, int sy, vector<hPoint> points) {
    vector<hPoint> result = {};
    for (auto point : points) {
        result.push_back(scale(sx, sy, point));
    }
    return result;
}

hPoint rotate(int angle, hPoint point) {
    float fAngle = (angle * PI / 180);
    Matrix3f s = (Matrix3f() << cos(fAngle), -sin(fAngle), 0, sin(fAngle), cos(fAngle), 0, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> rotate(int angle, vector<hPoint> points) {
    vector<hPoint> result = {};
    for (auto point : points) {
        result.push_back(rotate(angle, point));
    }
    return result;
}

hPoint shear(float kx, float ky, hPoint point) {
    Matrix3f s = (Matrix3f() << 1, kx, 0, ky, 1, 0, 0, 0, 1).finished();
    point.vec = s * point.vec;
    return point;
}

vector<hPoint> shear(float kx, float ky, vector<hPoint> points) {
    int x = points[0].vec[0];
    int y = points[0].vec[1];
    points = translate(-x, -y, points);
    vector<hPoint> result = {};
    for (auto point : points) {
        result.push_back(shear(kx, ky, point));
    }
    result = translate(x, y, result);
    return result;
}

/// <summary>
/// Warum wird hier ueberall die homogene Koordinate gebraucht? / Warum ist die Darstellung ohne Homogenen Anteil so komisch? 
/// </summary>

void pythagoras() {
    vector<hPoint> triangle = { hPoint(0, 0, 1), hPoint(40, 0,1), hPoint(0, 30, 1), hPoint(0,0,1) };
    triangle = translate(50, 50, triangle);
    //fillTriangle(triangle, 250, 0, 0);
    drawPolygon(triangle, 250, 0, 0);


    vector<hPoint> square1 = { hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1) };
    square1 = scale(3, 3, square1);
    square1 = translate(triangle[0].vec[0], triangle[0].vec[1], square1);
    square1 = translate(-(square1[3].vec[0] - square1[4].vec[0]), -(square1[3].vec[1] - square1[4].vec[1]), square1);
    drawPolygon(square1, 250, 0, 0); 

    vector<hPoint> square2 = { hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1) };
    square2 = scale(4, 3, square2); 
    square2 = translate(triangle[0].vec[0], triangle[0].vec[1], square2);
    square2 = translate(0, -(square2[1].vec[1] - square2[0].vec[1]), square2);
    drawPolygon(square2, 250, 0, 0); 

    vector<hPoint> square3 = { hPoint(0, 0, 1), hPoint(0, 10, 1), hPoint(10, 10,1), hPoint(10,0,1), hPoint(0,0,1) };
    square3 = scale(5, 5, square3); 
    float alphaRad = atan(static_cast<float>(triangle[2].vec[1] - triangle[0].vec[1]) / static_cast<float>(triangle[1].vec[0] - triangle[0].vec[0]));
    int alpha = static_cast<int>(alphaRad * 180 / PI);
    square3 = rotate(90 - alpha, square3);
    square3 = translate(triangle[1].vec[0], triangle[1].vec[1], square3);
    drawPolygon(square3, 250, 0, 0);

}


void scanline(const std::vector<Edge>& polygon, int r, int b, int g) {
    std::vector<PassiveEdge> passiveEdges = {};
    for (Edge e : polygon) {
        /*int xmin = min(e.x0, e.x1);
        int xmax = max(e.x0, e.x1);
        int ymin = min(e.y0, e.y1);
        int ymax = max(e.y0, e.y1); 
        */
        
        int xmin = e.x0;
        int ymin = e.y0;
        int xmax = e.x1;
        int ymax = e.y1;   

        PassiveEdge pE = PassiveEdge(xmin, ymin, xmax, ymax); 
        passiveEdges.push_back(pE);
    }
    sort(passiveEdges.begin(), passiveEdges.end(), compareByYmin);
    int start = passiveEdges[0].ymin;
    int limit = passiveEdges.back().ymax; 
    std::vector<ActiveEdge> activeEdges = {}; 
    for (int y = start; y <= limit; y++) {
        for (PassiveEdge pE : passiveEdges) {
            if (pE.ymin == y) {
                float xmax = pE.xmax;
                float dx = (xmax - pE.xmin) / (pE.ymax - pE.ymin); 
                ActiveEdge aE = ActiveEdge(pE.xmin, pE.ymax, dx);
                activeEdges.push_back(aE); 
            }  
        }  
        for (auto iter = activeEdges.begin(); iter != activeEdges.end();) {
            if (iter->ymax == y) {
                iter = activeEdges.erase(iter);
            }
            else {
                ++iter;
            }
        } 
        sort(activeEdges.begin(), activeEdges.end(), compareByXs); 
        
        bool isDrawing = true;
        for (int i = 1; i < activeEdges.size(); i++) {
            if (isDrawing) {
                for (int x = ceil(activeEdges[i - 1].xs); x <= floor(activeEdges[i].xs); x++) {
                    setPixel(x, y, r, b, g);
                }
                activeEdges[i - 1].xs += activeEdges[i - 1].dx;
                activeEdges[i].xs += activeEdges[i].dx;
            }
            isDrawing = !isDrawing; 
         } 
    }
} 


//void scanline(const std::vector<Edge>& polygon, int r, int g, int b) {
//
//    //init list of passive edges and sort it
//    std::vector<PassiveEdge> passiveEdges = {};
//    for (auto& edge : polygon) {
//        int xmin = edge.x0;
//        int ymin = edge.y0;
//        int xmax = edge.x1;
//        int ymax = edge.y1;
//
//        PassiveEdge pEdge = PassiveEdge(xmin, ymin, xmax, ymax);
//        passiveEdges.push_back(pEdge);
//    }
//    sort(passiveEdges.begin(), passiveEdges.end(), compareByYmin);
//
//    int y = passiveEdges[0].ymin;
//    int limit = passiveEdges.back().ymax;
//    std::vector<ActiveEdge> activeEdges = {};
//
//    while (y <= limit) {
//        //move passive edges to active
//        for (auto& edge : passiveEdges) {
//            if (edge.ymin == y) {
//                float xmax = edge.xmax;
//                float dx = (xmax - edge.xmin) / (edge.ymax - edge.ymin);
//                ActiveEdge activeEdge = ActiveEdge(edge.xmin, edge.ymax, dx);
//                activeEdges.push_back(activeEdge);
//            }
//        }
//        //remove non-active edges
//        for (auto iter = activeEdges.begin(); iter != activeEdges.end();) {
//            if (iter->ymax == y) {
//                iter = activeEdges.erase(iter);
//            }
//            else {
//                ++iter;
//            }
//        }
//
//        sort(activeEdges.begin(), activeEdges.end(), compareByXs);
//
//        bool isDrawing = true;
//
//        for (int i = 1; i <= activeEdges.size(); i++) {
//            if (isDrawing) {
//                for (int x = ceil(activeEdges[i - 1].xs); x <= floor(activeEdges[i].xs); x++) {
//                    setPixel(x, y, r, g, b);
//                }
//            }
//            isDrawing = !isDrawing;
//        }
//
//        for (auto& edge : activeEdges) {
//            edge.xs += edge.dx;
//        }
//        y++;
//    }
//}


int Outcode(int x, int y, int umin, int umax, int vmin, int vmax) {
    int c = 0;
    if (x < umin) c = c | 8; // 1000
    if (x > umax) c = c | 4; // 0100
    if (y < vmin) c = c | 2; // 0010
    if (y > vmax) c = c | 1; // 0001
    return c;
}


void wrongCohenSutherlandClippingDoNotUse(std::vector<Point> const& figurePoint, int xmin, int xmax, int ymin, int ymax) {
    vector<Line> lines(figurePoint.size());
    for (int i = 0; i < figurePoint.size()-1; i++) {
        //hinzufügen aller Linien in eine Liste
            lines[i] = Line(figurePoint[i], figurePoint[i+1]);
            cout << figurePoint[i ].x << "     " << figurePoint[i+1 ].x << endl;
            cout << figurePoint[i ].y << "     " << figurePoint[i+1 ].y << endl;

            //p1, p2, p3
            //p1mitp2 p2mitp3 p3mitp1
    }
    for (Line l : lines) {
       if((l.p1.x == 0 && l.p1.y == 0) || ((l.p2.x == 0 && l.p2.y == 0))) lines.pop_back(); 
        cout << l.p1.x << " " << l.p1.y << " " << l.p2.x << " " << l.p2.y << endl; 
    }
    cout << "ENd" << endl; 
    //lines[figurePoint.size() - 1] = Line(figurePoint[figurePoint.size() - 1], figurePoint[0]); //letzter Punkt des Polygons wird mit erstem Punkt verbunden


    std::vector<Point> newFigurePoint = {};
    for (int i = 0; i < lines.size(); i++) {
        cout << i << endl; 

        int c1 = Outcode(lines[i].p1.x, lines[i].p1.y, xmin, xmax, ymin, ymax);
        int c2 = Outcode(lines[i].p2.x, lines[i].p2.y, xmin, xmax, ymin, ymax);

        cout << c1 << " " << c2 << endl;


        if (c1 == 0 && c2 == 0) {
            //Line vollständig innerhalb
            newFigurePoint.push_back(lines[i].p1);

        }
        else if (c1 != 0 && c2 != 0) {
            cout << "Both points out of bounds: " << lines[i].p1.x << " " << lines[i].p1.y << " " << lines[i].p2.x << " " << lines[i].p2.y << endl;
            //Line vollständig außerhalb
            Point p1 = lines[i].p1;
            Point p2 = lines[i].p2;
            float dx = static_cast<float>(p2.x) - static_cast<float>(p1.x);
            float dy = static_cast<float>(p2.y) - static_cast<float>(p1.y);
            float m = 0.0;
            cout << "dx: " << dx << endl; 
            cout << "dy: " << dy << endl;
            if (dx != 0.0) {
                m = dy / dx;
            }
            float c = p1.y - m * p1.x;

            float ysXMin = m * xmin + c;
            float ysXMax = m * xmax + c;
            float xsYMax = 0.0;
            float xsYMin = 0.0; 
            if (m != 0.0) {
                xsYMax = (ymax - c) / m;
                xsYMin = (ymin - c) / m;
            }

            cout << "ysXMin: " << ysXMin << " ysXMax: " << ysXMax << " xsYMax: " << xsYMax << " xsYMin: " << xsYMin << endl; 



            if ((xsYMax >= xmin && xsYMax <= xmax) && (ysXMin >= ymin && ysXMin <= ymax)) {
                cout << "First" << endl;

                p1 = Point(xsYMax, ymax);
                p2 = Point(xmin, ysXMin); 
                newFigurePoint.push_back(p1);
                newFigurePoint.push_back(p2);
            }
            else if ((xsYMax >= xmin && xsYMax <= xmax) && (xsYMin >= xmin && xsYMin <= xmax)) {
                cout << "sec" << endl;

                if (dy > 0) {   //p2 hoeher als p1 
                    p1 = Point(xsYMin, ymin);
                    p2 = Point(xsYMax, ymax);
                }
                else {
                    p1 = Point(xsYMax, ymax);
                    p2 = Point(xsYMin, ymin);
                }
                newFigurePoint.push_back(p1);
                newFigurePoint.push_back(p2);
            }
            else if ((xsYMax >= xmin && xsYMax <= xmax) && (ysXMax >= ymin && ysXMax <= ymax)) {
                cout << "third" << endl;

                p1 = Point(xsYMax, ymax);
                p2 = Point(xmax, ysXMax);
                newFigurePoint.push_back(p1);
                newFigurePoint.push_back(p2);
            }
            else if ((ysXMin >= ymin && ysXMin <= ymax) && (ysXMax >= ymin && ysXMax <= ymax)) {
                cout << "4th" << endl;

                p1 = Point(xmin, ysXMin);
                p2 = Point(xmax, ysXMax);
                newFigurePoint.push_back(p1);
                newFigurePoint.push_back(p2);
            }
            else if ((ysXMin >= ymin && ysXMin <= ymax) && (xsYMin >= xmin && xsYMin <= xmax)) {
                cout << "4th" << endl;

                p1 = Point(xmin, ysXMin);
                p2 = Point(xsYMin, ymin);
                newFigurePoint.push_back(p1);
                newFigurePoint.push_back(p2);
            }
            else if ((ysXMax >= ymin && ysXMax <= ymax) && (xsYMin >= xmin && xsYMin <= xmax)) {
                cout << "4th" << endl;

                p1 = Point(xmax, ysXMax);
                p2 = Point(xsYMin, ymin);
                newFigurePoint.push_back(p1);
                newFigurePoint.push_back(p2);
            }
        }
        else {
            //Line zum Teil innerhalb
            Point p1 = lines[i].p1;
            Point p2 = lines[i].p2;
            float dx = static_cast<float>(p2.x) - static_cast<float>(p1.x);
            float dy = static_cast<float>(p2.y) - static_cast<float>(p1.y);
            float m = 0.0; 
            if(dx != 0.0) {
                m = dy / dx;
            }
            float c = p1.y - m * p1.x; 

            if (c1 != 0) {
                // && dy <= 0 ? 
                if (c1 == 9 || c1 == 8 || c1 == 10) {    // von links nach rechts unten oder gerade verlaufend 
                    float ysXMin = m * xmin + c;
                    p1 = Point(xmin, ysXMin);
                    newFigurePoint.push_back(p1); 
                    newFigurePoint.push_back(p2);
                }
                else if (c1 == 5 || c1 == 4 || c1 == 6) {
                    float ysXMax = m * xmax + c;
                    p1 = Point(xmax, ysXMax);
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
                else if (c1 == 1) {
                    float xsYMax = (ymax - c) / m;
                    p1 = Point(xsYMax, ymax); 
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
                else {
                    float xsYMin = (ymin - c) / m;
                    p1 = Point(xsYMin, ymin);
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
            }
            else {
                if (c2 == 9 || c2 == 8 || c2 == 10) {    // von links nach rechts unten oder gerade verlaufend 
                    float ysXMin = m * xmin + c;
                    p2 = Point(xmin, ysXMin);
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
                else if (c2 == 5 || c2 == 4 || c2 == 6) {
                    float ysXMax = m * xmax + c;
                    p2 = Point(xmax, ysXMax);
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
                else if (c2 == 1) {
                    float xsYMax = (ymax - c) / m;
                    p2 = Point(xsYMax, ymax);
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
                else {
                    float xsYMin = (ymin - c) / m;
                    p2 = Point(xsYMin, ymin);
                    newFigurePoint.push_back(p1);
                    newFigurePoint.push_back(p2);
                }
            }
            drawPolygon(newFigurePoint, 250, 0, 0); 
        }
    }            

    

    drawPolygonNewSuperSpecialEditionNotDumbISwear(newFigurePoint, 250, 0, 0);
}

float getM(Point p1, Point p2) {
    return (p2.y - p1.y) / (p2.x - p1.x);
}


Point getXs(Point p1, Point p2, float m, float c, float xBorder) {
    if (p1.y == p2.y) return Point(0, 0); 
    float y = m * xBorder + c; 
    Point xs = Point(xBorder, y);
    return xs; 
}
Point getYs(Point p1, Point p2, float m, float c, float yBorder) {
    if (p1.x == p2.x) return Point(0, 0);
    float x = (yBorder - c) / m;
    Point ys = Point(x, yBorder);
    return ys;
}

void cohenSutherlandLineClip(std::vector<Point> const& figurePoint, int xmin, int xmax, int ymin, int ymax) {
    vector<Point> clippedFigurePoints = {}; 
    vector<Line> lines = {};
    for (int i = 0; i < figurePoint.size() - 1; i++) {
        Line line = Line(figurePoint[i], figurePoint[i + 1]);
        lines.push_back(line);
    }

    for (Line l : lines) {
        int outcode0 = Outcode(l.p1.x, l.p1.y, xmin, xmax, ymin, ymax);
        int outcode1 = Outcode(l.p2.x, l.p2.y, xmin, xmax, ymin, ymax);

        if (!(outcode0 | outcode1)) {
                    // bitwise OR is 0: both points inside window; trivially accept and exit loop
            Point p1 = Point(l.p1.x, l.p1.y);
            Point p2 = Point(l.p2.x, l.p2.y); 
            clippedFigurePoints.push_back(p1);
            clippedFigurePoints.push_back(p2);
            continue; 
                }
        else if (outcode0 & outcode1) {
            // bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
            // or BOTTOM), so both must be outside window; exit loop (accept is false)
            double x1, y1, x2, y2;
            if (l.p1.x >= xmax) {
                x1 = l.p1.x >= xmax ? xmax : l.p1.x;
            }
            else {
                x1 = l.p1.x <= xmin ? xmin : l.p1.x;
            }
            if (l.p2.x >= xmax) {
                x2 = l.p2.x >= xmax ? xmax : l.p2.x;
            }
            else {
                x2 = l.p2.x <= xmin ? xmin : l.p2.x;
            }
            if (l.p1.y >= ymax) {
                y1 = l.p1.y >= ymax ? ymax : l.p1.y;
            }
            else {
                y1 = l.p1.y <= ymin ? ymin : l.p1.y;
            }
            if (l.p2.y >= ymax) {
                y2 = l.p2.y >= ymax ? ymax : l.p2.y;
            }
            else {
                y2 = l.p2.y <= ymin ? ymin : l.p2.y;
            }

            Point p1 = Point(x1, y1);
            Point p2 = Point(x2, y2);

            cout << "xmax: " << xmax << endl; 
            cout << "x1: " << x1 << " x2: " << x2 << " y1: " << y1 << " y2: " << y2 << endl; 

            clippedFigurePoints.push_back(p1);
            clippedFigurePoints.push_back(p2);
             
        }
        else {
            // failed both tests, so calculate the line segment to clip
                  // from an outside point to an intersection with clip edge
            double x, y;

            // At least one endpoint is outside the clip rectangle; pick it.
            int outcodeOut = outcode1 > outcode0 ? outcode1 : outcode0;
            bool isoutcodeOutOutcode0 = outcodeOut == outcode0 ? true : false;

            // Now find the intersection point;
            // use formulas:
            //   slope = (y1 - y0) / (x1 - x0)
            //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
            //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
            // No need to worry about divide-by-zero because, in each case, the
            // outcode bit being tested guarantees the denominator is non-zero
            if (outcodeOut & TOP) {           // point is above the clip window
                x = l.p1.x + (l.p2.x - l.p1.x) * (ymax - l.p1.y) / (l.p2.y - l.p1.y);
                y = ymax;
              }
            else if (outcodeOut & BOTTOM) { // point is below the clip window
                x = l.p1.x + (l.p2.x - l.p1.x) * (ymin - l.p1.y) / (l.p2.y - l.p1.y);
                y = ymin;
              }
            else if (outcodeOut & RIGHT) {  // point is to the right of clip window
                y = l.p1.y + (l.p2.y - l.p1.y) * (xmax - l.p1.x) / (l.p2.x - l.p1.x);
                x = xmax;
            }
            else if (outcodeOut & LEFT) {   // point is to the left of clip window
                y = l.p1.y + (l.p2.y - l.p1.y) * (xmin - l.p1.x) / (l.p2.x - l.p1.x);
                x = xmin;
            }

            if (isoutcodeOutOutcode0) {
                Point p1 = Point(x, y);
                Point p2 = Point(l.p2.x, l.p2.y);
                clippedFigurePoints.push_back(p1);
                clippedFigurePoints.push_back(p2);
            }
            else {
                Point p1 = Point(l.p1.x, l.p1.y);
                Point p2 = Point(x, y);
                clippedFigurePoints.push_back(p1);
                clippedFigurePoints.push_back(p2);
            }
            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
           /* if (outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = ComputeOutCode(x0, y0);
            }
            else {
                x1 = x;
                y1 = y;
                outcode1 = ComputeOutCode(x1, y1);
            }*/
        }
      }


    drawPolygonNewSuperSpecialEditionNotDumbISwear(clippedFigurePoints, 250, 0, 0);
    }

void sutherlandHodgmanClipping(std::vector<Point> const& figurePoint, int xmin, int xmax, int ymin, int ymax) {
    vector<Point> clippedFigurePoints = {};
    vector<Line> lines = {};
    for (int i = 0; i < figurePoint.size() - 1; i++) {
        Line line = Line(figurePoint[i], figurePoint[i + 1]);
        lines.push_back(line);
    }
    Line line = Line(figurePoint[figurePoint.size() - 1], figurePoint[0]);
    lines.push_back(line);

   


    for (Line l : lines) {
        cout  << l.p1.x << " " << l.p1.y << " " << l.p2.x << " " << l.p2.y << endl;
        int outcode0 = Outcode(l.p1.x, l.p1.y, xmin, xmax, ymin, ymax);
        int outcode1 = Outcode(l.p2.x, l.p2.y, xmin, xmax, ymin, ymax);

        if (!(outcode0 | outcode1)) {
            Point p1 = Point(l.p1.x, l.p1.y);
            Point p2 = Point(l.p2.x, l.p2.y);
            clippedFigurePoints.push_back(p1);
           // clippedFigurePoints.push_back(p2);
            //clippedFigurePoints.push_back(p2);
            continue;
        }
        else if(outcode0 != 0 && outcode1 != 0) {
            Point p1 = Point(l.p1.x, l.p1.y);
            Point p2 = Point(l.p2.x, l.p2.y);

            vector<Point> intersectionPoints = getIntersectionPointForBothPointsOutOfBounds(p1, p2, xmin, xmax, ymin, ymax);
            sort(intersectionPoints.begin(), intersectionPoints.end(), compareByY); 
            for (Point p : intersectionPoints) {
                cout <<"x: "<< p.x << " y: " << p.y << endl;
                clippedFigurePoints.push_back(p);
            }
        }
        else if (outcode0 == 0 ) {    //p1 is inside of frame p2 is not
            Point p1 = Point(l.p1.x, l.p1.y);
            Point p2 = Point(l.p2.x, l.p2.y);

            Point intersectionPoint = getIntersectionPoint(p1, p2, xmin, xmax, ymin, ymax);
            //clippedFigurePoints.push_back(p1);
            cout << "Intersec Point: " << intersectionPoint.x << " " << intersectionPoint.y << endl;
            clippedFigurePoints.push_back(p1);
            clippedFigurePoints.push_back(intersectionPoint);
        }
        else if (outcode1 == 0) {    //p2 is inside outside of frame p1 is not
            Point p1 = Point(l.p1.x, l.p1.y);
            Point p2 = Point(l.p2.x, l.p2.y);

            Point intersectionPoint = getIntersectionPoint(p2, p1, xmin, xmax, ymin, ymax);
            //clippedFigurePoints.push_back(p1);
            cout << "Intersec Point: " << intersectionPoint.x << " " << intersectionPoint.y << endl;
            clippedFigurePoints.push_back(intersectionPoint);
            clippedFigurePoints.push_back(p2);

        } 
    }
    drawPolygonNewSuperSpecialEditionNotDumbISwear(clippedFigurePoints, 250, 0, 0);
}

 vector<Point> getIntersectionPointForBothPointsOutOfBounds(Point p1, Point p2, int xmin, int xmax, int ymin, int ymax) {
    float dx = static_cast<float>(p2.x) - static_cast<float>(p1.x);
    float dy = static_cast<float>(p2.y) - static_cast<float>(p1.y);
    float m = 0.0;
    if (dx != 0.0) {
        m = dy / dx;
    }
    float c = static_cast<float>(p1.y) - m * static_cast<float>(p1.x);

    float xMaxYs = m * xmax + c;
    float xMinYs = m * xmin + c;
    float yMaxXs = (static_cast<float>(ymax) - c) / m;
    float yMinXs = (static_cast<float>(ymin) - c) / m;
    vector<Point> result = {};
    if (xMinYs <= ymax && xMinYs >= ymin) {
        Point p = Point(xmin, xMinYs);
        result.push_back(p);
    }
    if (xMaxYs <= ymax && xMaxYs >= ymin) {
        Point p = Point(xmax, xMaxYs);
        result.push_back(p); 
    }
    
    if (yMaxXs >= xmin && yMaxXs <= xmax) {
        Point p = Point(yMaxXs, ymax);
        result.push_back(p);
    }
    if (yMinXs >= xmin && yMinXs <= xmax) {
        Point p = Point(yMinXs, ymin);
        result.push_back(p);
    }
    return result; 
} 



Point getIntersectionPoint(Point p1, Point p2, int xmin, int xmax, int ymin, int ymax) {
    float dx = static_cast<float>(p2.x) - static_cast<float>(p1.x);
    float dy = static_cast<float>(p2.y) - static_cast<float>(p1.y);
    float m = 0.0;
    if (dx != 0.0) {
        m = dy / dx;
    }
    float c = static_cast<float>(p1.y) - m * static_cast<float>(p1.x);

    Point xBorderIntersectionPoint = Point(-1, -1);
    Point yBorderIntersectionPoint = Point(-1, -1);
   
    cout << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;


    if (!(p1.y == p2.y)) {
        if (dx > 0) {
            float y = m * xmax + c;
            xBorderIntersectionPoint = Point(xmax, y);
        }
        else {
            float y = m * xmin + c;
            xBorderIntersectionPoint = Point(xmin, y);
        }
    }
    if (m != 0.0) { // y intersection point 
        if (dy > 0) {
            float xsYMax = (static_cast<float>(ymax) - c) / m;
            yBorderIntersectionPoint = Point(xsYMax, ymax);
        }
        else {
            float xsYMin = (static_cast<float>(ymin) - c) / m;
            yBorderIntersectionPoint = Point(xsYMin, ymin);
        }
    }

            if (xBorderIntersectionPoint.y >= ymin && xBorderIntersectionPoint.y <= ymax) {
                return xBorderIntersectionPoint;
            }   
            if (yBorderIntersectionPoint.x >= xmin && yBorderIntersectionPoint.x <= xmax) {
                return yBorderIntersectionPoint;
            }
        Point temp = Point(-1, -1);
        cout << "Here: " << temp.x << " " << temp.y << endl;
        return temp;   
}

void plot(int x, int y, float r, float g, float b, float c) {
    setPixel(x, y, r / c, g / c, b / c); 
}

float ipart(float x) {
    return floor(x); 
}


int roundToInteger(float num) {
    return (int)(num + 0.5);
}

// fractional part of x
float fpart(float x) {
    if (x < 0) {
        return 1 - (x - (int)x);
    }
    return x - (int)x;
}

float rfpart(float x) {
    return 1 - fpart(x);
}

void drawLine(int x0, int y0, int x1, int y1) {
    boolean steep = abs(y1 - y0) > abs(x1 - x0) ? true : false; 

        if (steep) {
            swap(x0, y0);
            swap(x1, y1);
        }
            if (x0 > x1) {
                swap(x0, x1);
                swap(y0, y1); 
            }

            float dx = x1 - x0;
            float dy = y1 - y0; 
            float gradient; 

                if (dx == 0.0) {
                    gradient = 1.0;
                }
                else {
                    gradient = dy / dx; 
                }
                    // handle first endpoint
                float xend = roundToInteger(x0);
                float yend = y0 + gradient * (xend - x0);
                float xgap = rfpart(x0 + 0.5);
                float xpxl1 = xend; // this will be used in the main loop
                float ypxl1 = ipart(yend); 
                if (steep) {
                    plot(ypxl1, xpxl1, 255, 0, 0, rfpart(yend) * xgap);
                    plot(ypxl1 + 1, xpxl1, 255, 0, 0, fpart(yend) * xgap);
                }
                else {
                    plot(xpxl1, ypxl1, 255, 0, 0, rfpart(yend) * xgap);
                    plot(xpxl1, ypxl1 + 1, 255, 0, 0, fpart(yend) * xgap);
                }
                float intery = yend + gradient; // first y-intersection for the main loop

                        // handle second endpoint
                xend = roundToInteger(x1);
                yend = y1 + gradient * (xend - x1);
                xgap = fpart(x1 + 0.5);
                float xpxl2 = xend; //this will be used in the main loop
                float ypxl2 = ipart(yend); 
                if (steep) {
                    plot(ypxl2, xpxl2, 255, 0, 0, rfpart(yend) * xgap);
                    plot(ypxl2 + 1, xpxl2, 255, 0, 0, fpart(yend) * xgap);
                }
                else {
                    plot(xpxl2, ypxl2, 255, 0, 0, rfpart(yend) * xgap);
                    plot(xpxl2, ypxl2 + 1, 255, 0, 0, fpart(yend) * xgap);
                }

                            // main loop
                if (steep) {
                    for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
                        plot(ipart(intery), x, 255, 0, 0, rfpart(intery));
                        plot(ipart(intery) + 1, x, 255, 0, 0, fpart(intery));
                        intery = intery + gradient;
                    }
                }
                else {
                    for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
                        plot(x, ipart(intery), 255, 0, 0, rfpart(intery));
                        plot(x, ipart(intery) + 1, 255, 0, 0, fpart(intery));
                        intery = intery + gradient;
                    }
                }
}

