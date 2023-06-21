#pragma once
class Vector
{
public:
	double x,y,z;
public:
	Vector(void) : x(0.0), y(0.0), z(0.0) {};
	Vector(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};

	Vector cross (const Vector &v);
	double dot(const Vector &v);
	Vector normalize(void);
	Vector svmpy(double s);
	Vector vadd(const Vector &v);
	double veclength (void);
	Vector vsub(const Vector &v);
};

