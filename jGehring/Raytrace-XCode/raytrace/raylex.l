%{ 
#include "y.tab.h"

extern int linenum;
int yywrap(void)
{
return 1; // EOF
} 
   
%} 

ziffern  ([0-9]+)
vorz     ([+-]?)
ganzz    ({vorz}{ziffern})
kommaz   ({vorz}{ziffern}*[.]{ziffern})
expo	 ([eE]{vorz}{ziffern})
zahl	 (({kommaz}{expo}?)|({ganzz}{expo}))
string   ([a-zA-Z0-9+-_]+)

%%
    
#.* { /*printf("COMMENT: '%s'\n", yytext);*/ }
resolution { return(RESOLUTION); }
eyepoint { return(EYEPOINT); }
lookat { return(LOOKAT); }
fovy { return(FOVY); }
aspect { return(ASPECT); }
object { return(OBJECT); }
poly { return(POLY); }
polygon { return(POLY); } /* equivalent spelling */
vertex { return(VERTEX); }
sphere { return(SPHERE); }
path { return (PATH); }
quadric { return(QUADRIC); }
property { return(PROPERTY); }
ambient { return(AMBIENT); }
diffuse { return(DIFFUSE); }
specular { return(SPECULAR); }
mirror { return(MIRROR); }
ambience { return(AMBIENCE); }
background { return(BACKGROUND); }
light { return(LIGHT); }
f {return(POLY); }
v {return (VERTEX);}
vn.* {}
direction { return(DIRECTION); }
color { return(COLOR); } /* equivalent spelling */
up { return(UP); } /* equivalent spelling */
{ganzz}	        {  yylval.intval=atoi(yytext); return(INTEGER); }
{zahl}		{  yylval.floatval=atof(yytext); return(FLOAT);  }
{string}	{ yylval.stringval=(char *)strdup(yytext); return(STRING); }
\n              { linenum++; }
.    ;
