#pragma once
#include "Vector.h"
#include "Color.h"

class Light
{
	Vector	 direction;
	Color	 color;
public:
	Light(void): direction(), color() {};
	Light(const Vector &v, const Color &c): direction(v), color(c) {};

	Vector getDirection() { return direction; };
	Color getColor() { return color; };
};


