/******************************************************************************/
/*        Grammatik Raytracer-Beschreibungsfile                               */
/******************************************************************************/

%union
{
    int intval;
    double floatval;
    char *stringval;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Fix compatibility warnings
#pragma warning(disable: 4996)

// Fix linker warning
//extern __declspec(dllimport) void *malloc(size_t);
//extern __declspec(dllimport) void free(void*);
#define YYMALLOC
#define YYFREE

// so that we can keep track if a particular parameter appeared in our input or not
static int resolution_seen = 0;
static int aspect_seen = 0;
static int up_seen = 0;
static int eyepoint_seen = 0;
static int lookat_seen = 0;

extern int linenum = 1;
static int yywarn(char *s)
{
    fprintf(stderr,"warning: line %d: %s\n", linenum, s);
    return 0;
}

static int yyerror(char *s)
{
    fprintf(stderr,"error: line %d: %s\n", linenum, s);
    exit(1);
}

struct {
	double ar,ag,ab, r, g, b, s, m;
	} prop;

struct {
	double dirx, diry, dirz, colr, colg, colb;
	} light;

int yylex(void);
extern void add_resolution(int width, int height);
extern void add_eyepoint(double x, double y, double z);
extern void add_lookat(double x, double y, double z);
extern void add_background(double r, double g, double b);
extern void add_ambience(double r, double g, double b);
extern void add_fovy(double fov);
extern void add_up(double x, double y, double z);
extern void add_aspect(double aspect);
extern void add_sphere(char *n, double posX, double posY, double posZ, double radius);
extern void add_quadric(char *n, double a, double b, double c, double d, double e, double f, double g, double h, double j, double k);
extern void add_property(char *n, double ar, double ag, double ab, double r, double g, double b, double s, double m);
extern void add_objekt(char *sName, char *pName, double scaling);
extern void add_light(char *n, double dirx, double diry, double dirz, double colr, double colg, double colb);
extern void add_vertex(double x, double y, double z);
extern void init_polygon_surface(char *n);
extern void add_index(int i);
extern int transformToInt(char* n);
extern void reset_polycount(void);
extern void add_path(char *path);
%}



%type <intval> index
%type <floatval> colorVal realVal angleVal zeroToOneVal

%token <intval> INTEGER
%token <floatval> FLOAT
%token <stringval> STRING
%token RESOLUTION EYEPOINT LOOKAT UP FOVY ASPECT
%token OBJECT QUADRIC SPHERE POLY PATH
%token VERTEX
%token PROPERTY AMBIENT DIFFUSE SPECULAR MIRROR
%token AMBIENCE BACKGROUND
%token LIGHT DIRECTION COLOR

%%

scene 
    : picture_parameters some_viewing_parameters global_lighting geometry
    |
    ;

some_viewing_parameters
    :
      {
	fprintf(stderr,"processing viewing parameters...\n");
      }
    viewing_parameters
      {
	if (!lookat_seen)
	  yywarn("LOOKAT missing from viewing parameters");
	if (!up_seen)
	  yywarn("UP missing from viewing parameters");
	if (!aspect_seen)
	  yywarn("ASPECT missing from viewing parameters");
	if (!eyepoint_seen)
	  yywarn("EYEPOINT missing from viewing parameters");
	if (!resolution_seen)
	  yywarn("RESOLUTION missing from viewing parameters");
      }
    ;


/* the following allows you to specify the viewing parameters in any order */

viewing_parameters 
    : viewing_parameters viewing_parameter
    | /* empty */
    ;

picture_parameters
    : picture_parameters picture_parameter
    | picture_parameter 
    ;

picture_parameter
    : {
    fprintf(stderr, "processing resolution...\n");
    }
    resolution
    | {fprintf(stderr, "processing background ...\n");}
    background
    ;

viewing_parameter
    : eyepoint
    | lookat
    | fovy
    | aspect
    | up
    ;

resolution
    : RESOLUTION index index
      { printf("resolution %d %d\n", $2, $3 );
          add_resolution($2, $3);
          resolution_seen = 1;
      }
    ;

background
    : BACKGROUND colorVal colorVal colorVal
      { printf("background %f %f %f\n", $2, $3, $4);
          add_background($2, $3, $4);
      }
    ;


eyepoint
    : EYEPOINT realVal realVal realVal
      { printf("eyepoint %f %f %f\n", $2, $3, $4 );
          add_eyepoint($2, $3, $4);
          eyepoint_seen = 1;
      }
    ;

lookat
    : LOOKAT realVal realVal realVal
      { printf("lookat %f %f %f\n", $2, $3, $4 );
          add_lookat($2, $3, $4);
          lookat_seen = 1;
      }
    ;

up
    : UP realVal realVal realVal
      { printf("up %f %f %f\n", $2, $3, $4);
          add_up($2, $3, $4);
          up_seen = 1;
      }
    ;

fovy
    : FOVY realVal
      { printf("fovy %f\n", $2);
          add_fovy($2);
      }
    ;

aspect
    : ASPECT realVal
      { printf("aspect %f\n", $2 );
          add_aspect($2);
          aspect_seen = 1;
      }
    ;

global_lighting
    : AMBIENCE colorVal colorVal colorVal
      { printf("ambience %f %f %f\n", $2, $3, $4);
          add_ambience($2, $3, $4);
      }
    ;

geometry 
    : 
      {
	fprintf(stderr, "processing surfaces...\n");
      }
    surface_section 
      {
	fprintf(stderr, "processing properties...\n");
      }
    property_section 
      {
	fprintf(stderr, "processing lighting...\n");
      }
    lighting_section
      {
	fprintf(stderr, "processing objects...\n");
      }
    object_section 
    ;

surface_section
    : surfaces
    ;

surfaces 
    : surfaces one_surface
    | one_surface
    ;

one_surface
    : quadric_surface
    | polygon_surface
    | sphere_surface
    | path_surface
    ;

quadric_surface
    : OBJECT STRING QUADRIC realVal realVal realVal realVal realVal 
      realVal realVal realVal realVal realVal
      {
		  add_quadric($2, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);
		  free($2);
      }
    ;

sphere_surface
: OBJECT STRING SPHERE realVal realVal realVal realVal
{
    printf("sphere added %s %f %f %f %f \n", $2, $4, $5, $6, $7);
    add_sphere($2, $4, $5, $6, $6);
}
;
    
polygon_surface
    : OBJECT STRING POLY 
      {
          printf("add polygon surface\n");
          init_polygon_surface($2);
      }
    vertex_section polygon_section
    ;
    
path_surface
:
OBJECT STRING POLY
{
    printf("add polygon surface\n");
    init_polygon_surface($2);
} path_section
;

path_section
: PATH STRING
{
    printf("Path found! %s\n", $2);
    add_path($2);
}
;

vertex_section
    : vertices
    ;

vertices
    : vertices one_vertex
    | one_vertex
    ;

one_vertex
    : VERTEX realVal realVal realVal
      { printf("vertex %f %f %f\n", $2, $3, $4);
          add_vertex($2, $3, $4);
      }
    ;

polygon_section
    : polygons
    ;

polygons 
    : polygons one_polygon
    | one_polygon
    ;

one_polygon
    : POLY  
    { printf("polygon\n"); reset_polycount();}
      indices
      { printf("\n"); }
    ;

indices
    : indices one_index
    | one_index
    ;

one_index
	: index
	{ add_index($1); }
	;

property_section
    : properties
    ;

properties
    : properties one_property
    | one_property
    ;

one_property
    : PROPERTY STRING ambient diffuse specular mirror
	{
		add_property($2, prop.ar, prop.ag, prop.ab, prop.r, prop.g, prop.b, prop.s, prop.m); 
		free($2);
	}
    ;

ambient
    : AMBIENT zeroToOneVal zeroToOneVal zeroToOneVal
      { 
		prop.ar = $2;
		prop.ag = $3;
		prop.ab = $4;
      }
    ;

diffuse
    : DIFFUSE zeroToOneVal zeroToOneVal zeroToOneVal
      { 
		prop.r = $2;
		prop.g = $3;
		prop.b = $4;
      }
    ;

specular
    : SPECULAR  zeroToOneVal /* realVal */
      { 
		prop.s = $2;
      }
    ;

mirror
    : MIRROR zeroToOneVal
      { 
		prop.m = $2;
      }
    ;

object_section
    : objects
    ;

objects 
    : objects one_object
    | one_object
    ;

one_object
    : OBJECT STRING STRING realVal
      {
		add_objekt($2, $3, $4);
		free($2);
		free($3);
      }
    ;

lighting_section
    : lights
    ;

lights 
    : lights one_light
    | one_light
    ;

one_light : LIGHT STRING direction color
    {
		add_light($2, light.dirx, light.diry, light.dirz, light.colr, light.colg, light.colb);
		free($2);  
    }
    ;

direction : DIRECTION realVal realVal realVal
    {
		light.dirx = $2;
		light.diry = $3;
		light.dirz = $4;
    }
    ;

color : COLOR colorVal colorVal colorVal
    {
		light.colr = $2;
		light.colg = $3;
		light.colb = $4;
    }
    ;

// some minor type conversions and range checks...

colorVal : zeroToOneVal ;

zeroToOneVal : realVal
    {
	if ( $1 < 0.0 || $1 > 1.0 ) {
	    yyerror("value out of range (only 0 to 1 allowed)");
	} /* if */

	/* pass that value up the tree */
	$$ = $1;
    }
    ;

angleVal : realVal
    {
	if ( $1 < 0.0 || $1 > 180.0 ) {
	    yyerror("value out of range (only 0 to 180 degrees allowed)");
	} /* if */

	/* pass that value up the tree */
	$$ = $1;
    }
    ;

realVal 
    : FLOAT
      { $$ = $1; }
    | INTEGER
      { $$ = (float) $1; /* conversion from integers */ }
    ;

index : INTEGER | STRING
      {
          int val = transformToInt($1);
    	if ( $1 < 1 ) {
	    yyerror("index out of range (only 1 or more allowed)");
	} /* if */

	/* pass that value up the tree */
	$$ = val;
      }
    ;
    
%%
