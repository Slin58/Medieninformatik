#pragma once

#include "string"
#include "Vector.h"
using namespace::std;

class Surface
{
	std::string name;	
public:
	double a, b, c, d, e, f, g, h, j, k;
    vector<Vector> vertices;
    vector<int> indices;
    bool isQuadricSurface;

	Surface(void) : name(""), a(0.0), b(0.0), c(0.0), d(0.0), e(0.0), f(0.0), g(0.0), h(0.0), j(0.0), k(0.0), isQuadricSurface(false) {};
	Surface(char *n, double _a, double _b, double _c, double _d, double _e, double _f, double _g, double _h, double _j, double _k) : 
	name(n), a(_a), b(_b), c(_c), d(_d), e(_e), f(_f), g(_g), h(_h), j(_j), k(_k), vertices(NULL), isQuadricSurface(true) {};
    Surface(char *n, vector<Vector> v, vector<int> i): name(n), vertices(v), indices(i), isQuadricSurface(false){};

	std::string getName() { return name; };

};

