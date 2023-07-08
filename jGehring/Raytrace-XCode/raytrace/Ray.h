#pragma once
#include "Vector.h"
#include "Color.h"
#include "Light.h"
#include "Objekt.h"
#include "vector"

class Ray
{
    Vector direction, origin;
    Color background, ambience;
    int	 depth;
    
    Color shaded_color(Light *light, Ray &reflectedray, Vector &normal, Objekt *obj);
    
public:
    Ray(void) : direction(), origin(),background(), depth (0) {};
    Ray(const Vector &dir, const Vector &orig, const Color &bg, const Color &am, int d) : direction(dir), origin(orig), background(bg), ambience(am), depth (d) {};
    Color shade(std::vector<Objekt> &, std::vector<Light> &);
    
    Vector getDirection() { return direction; };
    Vector getOrigin() { return origin; };
    int getDepth() { return depth; };
    Color getBackground() { return background; };
    Color getAmbience() { return background; };
    
    void setDirection(const Vector &v) { direction=v; };
    void setOrigin(const Vector &v) { origin=v; };
    void setDepth(int d) { depth=d; };
    void setBackground(const Color &b) {background=b; };
    void setAmbience(const Color &a) { ambience=a; };
    
    Ray reflect(Vector &origin, Vector &normal);
    void extracted(Surface *surf, double &t);
    double calculateQuadricT(Surface &surf);
    double intersect(Objekt &obj, Vector &normal);
    
};


