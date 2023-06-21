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


//static void saveImage() {
//    std::string fileString("P3 \n256 256\n255\n");
//
//    for (int y = 0; y < RESOLUTION; y++){
//        for (int x = 0; x < RESOLUTION; x++) {
//            fileString += std::to_string((int)(framebuffer[x][y].R)) + " ";
//            fileString += std::to_string((int)(framebuffer[x][y].G)) + " ";
//            fileString += std::to_string((int)(framebuffer[x][y].B)) + "\t";
//        }
//        fileString += "\n";
//    }
//    //std::cout << fileString;
//
//    std::ofstream file("output.ppm");
//    file << fileString;
//}

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

bool inside(hPoint& d, hPoint& a, Vector2f& n){
    Vector2f x = Vector2f(a.vec[0] - d.vec[0], a.vec[1] - d.vec[1]);
    return x.dot(n) > 0;
}

vector<hPoint> clip(vector<hPoint>& polygon, hPoint& a, Vector2f& n){
    vector<hPoint> result = {};
    hPoint d = polygon[polygon.size() - 1];
    bool dIn = inside(d, a, n);
    if(dIn) result.push_back(d);
    
    for(auto point : polygon){
        hPoint c = d;
        bool cIn = dIn;
        d = point;
        dIn = inside(d, a, n);
        
        if(dIn != cIn){
            float t = Vector2f(d.vec[0] - c.vec[0], d.vec[1] - c.vec[1]).norm();
            hPoint intersection = hPoint(d.vec[0]*t + c.vec[0]*(1-t), d.vec[1]*t + c.vec[1]*(1-t), 1);
            result.push_back(intersection);
        }
        if(dIn) result.push_back(d);
    }
    return result;
}

vector<hPoint> sutherlandHodgman(vector<hPoint> &polygon, vector<hPoint> &border){
    for(int i = 1; i < border.size(); i++){
        Vector2f vec = Vector2f(border[i-1].vec[1] - border[i].vec[1], border[i].vec[0] - border[i-1].vec[0]);
        polygon = clip(polygon, border[i-1], vec);
    }
    return polygon;
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
//    std::vector<Point> polygon = {Point(30, 30), Point(70,400), Point(300, 400), Point(340, 30)};
//    drawPolygon(polygon, 255, 0, 0);
//    vector<float> tParams = {0.0, 0.0, 0.0, 0.32, 0.49, 0.75, 1, 1, 1};
//    deBoorClosed(tParams, polygon, 2);
    
    std::vector<hPoint> polygon = {hPoint(530, 30, 1), hPoint(30,400, 1), hPoint(400, 600, 1), hPoint(530, 30, 1)};
    //vector<hPoint> border = {hPoint(0, 0, 1), hPoint(0, RESOLUTION, 1), hPoint(RESOLUTION, RESOLUTION, 1), hPoint(RESOLUTION, 0, 1), hPoint(0, 0, 1)};
    //polygon = sutherlandHodgman(polygon, border);
//    drawPolygon(polygon, 250, 0, 0);
    
    //pythagoras();
    //fillRectangle(40, 40, 300, 300);
//    vector<hPoint> triangle = {hPoint(10, 20), hPoint(40, 30), hPoint(20, 50)};
//    fillTriangle(triangle, Vector3f(.5f, .4f, 0.9f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
    hPoint p0 = hPoint(0, 0, 1);
    hPoint p1 = hPoint(255, 50, 1);  //pythagoras(-5, 250, 350, 2, 2,1.1, 0);
    hPoint p2 = hPoint(255, 130, 1);
    hPoint p3 = hPoint(255, 230, 1);
    wuDrawLine(p0, p2, 250, 9, 0);
    newWu(p0, p1, 0, 250, 0);
    prettyBresenham(p0, p3, 250, 0, 0);
    //vector<hPoint> polygon = {hPoint(10, 10,1), hPoint(60,10, 1), hPoint(60, 60, 1), hPoint(10, 10, 1)};
    //vector<hPoint> polygon = {hPoint(30, 10,1), hPoint(10,50, 1), hPoint(25, 60, 1), hPoint(30, 50, 1), hPoint(35, 60, 1), hPoint(50, 50, 1)};
    //vector<Edge> polygon = {Edge(30, 5, 15, 30), Edge(15, 30, 15, 35), Edge(15, 35, 22, 45), Edge(30, 35, 22, 45), Edge(30, 35, 38, 45), Edge(45, 35, 38, 45), Edge(45, 30, 45, 35), Edge(30, 5, 45, 30)};
    
    //scanline(polygon, 255, 0,0);
    //drawPolygon(polygon, 255, 0, 0);
    //saveImage();
    glutMainLoop();
    
    return 0;
}


