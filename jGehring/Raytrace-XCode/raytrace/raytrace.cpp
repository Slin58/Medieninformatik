// raytrace.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "Ray.h"
#include "Color.h"
#include "Image.h"
#include "vector"

const double SCREENWIDTH = 1000;
const double SCREENHEIGHT = 1000;

static int linenum = 1; // counter to be used in yacc/lex file

using namespace std;

vector<Surface> surfaces;
vector<Property> properties;
vector<Objekt> objekte;
vector<Light> lights;
Surface poly_surface;

int Xresolution = 1250;
int Yresolution = 1250;
Vector eye(0, 0, SCREENHEIGHT * 8.0);
Vector lookat(1, 0, 0);
Color background(0, 0, 0);
Vector up(0, 1, 0);
Color ambience(1, 1, 1);
double fovy = 55.0;
double aspect = 1.0;


extern "C" {
extern FILE *yyin;
int yyparse();

void add_resolution(int width, int height){
    fprintf(stderr,"  adding resolution %d %d \n", width, height);
    Xresolution = width;
    Yresolution = height;
};

void add_eyepoint(double x, double y, double z){
    fprintf(stderr, "   adding eyepoint %f %f %f \n", x, y, z);
    eye = Vector(x, y, z);
};

void add_lookat(double x, double y, double z){
    fprintf(stderr, "   adding lookat %f %f %f \n", x, y, z);
    lookat = Vector(x, y, z);
};

void add_background(double r, double g, double b){
    fprintf(stderr, "   adding background %f %f %f \n", r, g, b);
    background = Color(r, g, b);
};

void add_ambience(double r, double g, double b){
    fprintf(stderr, "   adding ambience %f %f %f \n", r, g, b);
    ambience = Color(r, g, b);
}

void add_fovy(double fov){
    fprintf(stderr, "   adding fovy %f \n", fov);
    fovy = fov;
};
void add_up(double x, double y, double z){
    fprintf(stderr, "   adding up %f %f %f \n", x, y, z);
    up = Vector(x, y, z);
};
void add_aspect(double asp){
    fprintf(stderr, "   adding aspect %f \n", asp);
    aspect = asp;
};

void add_light(char *n, double dirx, double diry, double dirz, double colr, double colg, double colb) {
    fprintf(stderr,"  adding light %f %f %f %f %f %f\n", dirx, diry, dirz, colr, colg, colb);
    lights.push_back(Light(Vector(dirx,diry,dirz).normalize(), Color(colr, colg, colb)));
};
void add_quadric(char *n, double a, double b, double c, double d, double e, double f, double g, double h, double j, double k) {
    fprintf(stderr,"  adding quadric %s %f %f %f %f %f %f %f %f %f %f\n", n, a,b,c,d,e,f,g,h,j,k);
    surfaces.push_back(Surface(n, a,b,c,d,e,f,g,h,j,k));
};

void init_polygon_surface(char *n){
    Surface poly_surface(n, {}, {});
    surfaces.push_back(poly_surface);
}

void add_index(int i){
    fprintf(stderr,"  adding index %d \n", i);
    surfaces.back().indices.push_back(i);
};

void add_property(char *n,  double ar, double ag, double ab, double r, double g, double b, double s, double m) {
    fprintf(stderr,"  adding prop %f %f %f %f %f\n", r, g, b, s, m);
    properties.push_back(Property(n, Color(ar, ag, ab), Color(r, g, b), s, m));
};
void add_vertex(double x, double y, double z){
    fprintf(stderr,"  adding vertex %f %f %f \n", x, y, z);
    surfaces.back().vertices.push_back(Vector(x,y,z));
}

void add_objekt(char *sName, char *pName) {
    Surface *s = NULL;
    Property *p = NULL;
    string surfaceName(sName);
    string propertyName(pName);
    
    for(vector<Surface>::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
        if(i->getName() == surfaceName) {
            s = &(*i);
            break;
        }
    for(vector<Property>::iterator i = properties.begin(); i != properties.end(); ++i)
        if(i->getName() == propertyName) {
            p = &(*i);
            break;
        }
    
    if(s == NULL) {
        fprintf(stderr, "Surface not found: %s\n", sName);
        exit(1);
    }
    if(p == NULL) {
        fprintf(stderr, "Property not found: %s\n", pName);
        exit(1);
    }
    
    objekte.push_back(Objekt(s, p));
    fprintf(stderr, "  adding object: surface %s, property %s\n", sName, pName);
};

void add_sphere(char *n, double posX, double posY, double posZ, double radius){
    fprintf(stderr,"  adding sphere %f %f %f %f \n", posX, posY, posZ, radius);
    surfaces.push_back(Surface(n, 1.0, 0, 0, -2.0*posX, 1.0, 0, -2.0*posY, -2.0*posZ, 0.0, (posX*posX)+(posY*posY)+(posZ*posZ)-(radius*radius)));
};
}

int main(int argc, char* argv[])
{
    /* parse the input file */
    yyin = fopen("data/scene.data","r");
    if(yyin == NULL) {
        fprintf(stderr, "Error: Konnte Datei nicht oeffnen\n");
        return 1;
    }
    yyparse();
    fclose (yyin);
    
    Vector VPN=eye.vsub(lookat);
    double height = 2*tan(0.5*fovy*M_PI/180)*VPN.veclength();  /* Winkel werden in Grad angegeben! */
    double width = height*aspect;
    VPN = VPN.normalize();
    Vector u_Vec=up.cross(VPN).normalize();
    Vector v_Vec=u_Vec.cross(VPN).normalize();
    
    double dx = width / (double)Xresolution;
    double dy = height / (double)Yresolution;
    double y = -0.5 * height;
    Ray ray(Vector(1,0,0), eye, background, ambience, 0);
    
    Image bild(Xresolution, Yresolution);
    
    for (int scanline=0; scanline < Yresolution; scanline++) {
        
        //        printf("%4d\r", Yresolution-scanline);
        y += dy;
        double x = -0.5 * width;
        
        for (int sx=0; sx < Xresolution; sx++) {
            // ray.setDirection(Vector(x, y, 0.0).vsub(ray.getOrigin()).normalize());
            ray.setDirection(lookat.vadd(u_Vec.svmpy(x).vadd(v_Vec.svmpy(y))).vsub(ray.getOrigin()).normalize());
            x += dx;
            //            Color color = ray.shade(objekte, lights, sx, scanline);
            Color color = ray.shade(objekte, lights);
            
            
            bild.set(sx, scanline,
                     color.r > 1.0 ? 255 : int(255 * color.r),
                     color.g > 1.0 ? 255 : int(255 * color.g),
                     color.b > 1.0 ? 255 : int(255 * color.b));
        }
    }
    
    char name[] = "raytrace-bild.ppm";
    bild.save(name);
    
    return 0;
}

