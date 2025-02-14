#pragma once
#include "Vector.h"
#include "Color.h"
#include "Light.h"
#include "Objekt.h"
#include "vector"

class Ray
{
	Vector direction, origin;
	int	 depth;

	Color shaded_color(Light *light, Ray &reflectedray, Vector &normal, Objekt *obj);

public:
	Ray(void) : direction(), origin(), depth (0) {};
	Ray(const Vector &dir, const Vector &orig, int d) : direction(dir), origin(orig), depth (d) {};
	Color shade(std::vector<Objekt> &, std::vector<Light> &);

	Vector getDirection() { return direction; };
	Vector getOrigin() { return origin; };
	int getDepth() { return depth; };

	void setDirection(const Vector &v) { direction=v; };
	void setOrigin(const Vector &v) { origin=v; };
	void setDepth(int d) { depth=d; };

	Ray reflect(Vector &origin, Vector &normal);
	double intersect(Objekt &obj);

};


