//-----------------------------------------------------------------------------
//           Name: geometry.h
//         Author: Freeglut
//  Last Modified: 01/27/05
//    Description: Data and utility functions for rendering several useful 
//                 geometric shapes. This code is a modified version of the 
//                 code found in "freeglut_teapot.c" and "freeglut_geometry.c", 
//                 which is part of the open source project, Freegut.
//                 http://freeglut.sourceforge.net/
//
//                 See the text below this comment for all the legal licensing 
//                 mumbo-jumbo.
//
// The following functions are defined here:
//
// void renderWireTeapot(GLdouble size);
// void renderSolidTeapot(GLdouble size);
// void renderWireCube(GLdouble size);
// void renderSolidCube(GLdouble size);
// void renderWireSphere(GLdouble radius, GLint slices, GLint stacks);
// void renderSolidSphere(GLdouble radius, GLint slices, GLint stacks);
// void renderWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
// void renderSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
// void renderWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
// void renderSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
// void renderWireDodecahedron(void);
// void renderSolidDodecahedron(void);
// void renderWireOctahedron(void);
// void renderSolidOctahedron(void);
// void renderWireTetrahedron(void);
// void renderSolidTetrahedron(void);
// void renderWireIcosahedron(void);
// void renderSolidIcosahedron(void);
// void renderWireSierpinskiSponge(int num_levels, GLdouble offset[3], GLdouble scale);
// void renderSolidSierpinskiSponge(int num_levels, GLdouble offset[3], GLdouble scale);
//-----------------------------------------------------------------------------

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
// Do this to access M_PI, which is not officially part of the C/C++ standard.
#define _USE_MATH_DEFINES 
#include <math.h>
#include <GL/gl.h>

void renderWireTeapot(GLdouble size);
void renderSolidTeapot(GLdouble size);
void renderWireCube(GLdouble size);
void renderSolidCube(GLdouble size);
void renderWireSphere(GLdouble radius, GLint slices, GLint stacks);
void renderSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void renderWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void renderSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void renderWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
void renderSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
void renderWireDodecahedron(void);
void renderSolidDodecahedron(void);
void renderWireOctahedron(void);
void renderSolidOctahedron(void);
void renderWireTetrahedron(void);
void renderSolidTetrahedron(void);
void renderWireIcosahedron(void);
void renderSolidIcosahedron(void);
void renderWireSierpinskiSponge(int num_levels, GLdouble offset[3], GLdouble scale);
void renderSolidSierpinskiSponge(int num_levels, GLdouble offset[3], GLdouble scale);
void renderWireCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
#endif // _GEOMETRY_H_
