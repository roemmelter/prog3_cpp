// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL tetrahedron slicer
#ifndef GLVERTEX_SLICER_H
#define GLVERTEX_SLICER_H

#include "glvertex.h"

// extract 1 triangle from a tetrahedron
//  v0 is the cutaway vertex
//  d is distance of the respective point to the cutting plane
inline void lglSlice1Tri(const vec3 &v0,const double d0,
                         const vec3 &v1,const double d1,
                         const vec3 &v2,const double d2,
                         const vec3 &v3,const double d3)
{
   vec3 p0,p1,p2;

   p0=(d1*v0+d0*v1)/(d0+d1);
   p1=(d2*v0+d0*v2)/(d0+d2);
   p2=(d3*v0+d0*v3)/(d0+d3);

   lglBegin(LGL_TRIANGLES);
      lglTexCoord(p0);
      lglVertex(p0);
      lglTexCoord(p1);
      lglVertex(p1);
      lglTexCoord(p2);
      lglVertex(p2);
   lglEnd();
}

// extract 2 triangles (a quad) from a tetrahedron
//  v0 is the start point of the cutaway line segment
//  v1 is the end point of the cutaway line segment
//  d is distance of the respective point to the cutting plane
inline void lglSlice2Tri(const vec3 &v0,const double d0,
                         const vec3 &v1,const double d1,
                         const vec3 &v2,const double d2,
                         const vec3 &v3,const double d3)
{
   vec3 p0,p1,p2,p3;

   p0=(d2*v0+d0*v2)/(d0+d2);
   p1=(d2*v1+d1*v2)/(d1+d2);
   p2=(d3*v0+d0*v3)/(d0+d3);
   p3=(d3*v1+d1*v3)/(d1+d3);

   lglBegin(LGL_QUADS);
      lglTexCoord(p0);
      lglVertex(p0);
      lglTexCoord(p1);
      lglVertex(p1);
      lglTexCoord(p3);
      lglVertex(p3);
      lglTexCoord(p2);
      lglVertex(p2);
   lglEnd();
}

//! extract a slice from a tetrahedron
//! * generates the rendering commands for a sliced tetrahedron
//! * 2 cases: slice geometry consists of either 1 or 2 triangles
//!
//! \param v0 is the first of four corners of a tetrahedron
//! \param v1 is the second of four corners of a tetrahedron
//! \param v2 is the third of four corners of a tetrahedron
//! \param v3 is the fourth of four corners of a tetrahedron
//! \param o is a point on the slicing plane
//! \param n is the normal of the slicing plane
inline void lglSlice(const vec3 &v0, // vertex v0
                     const vec3 &v1, // vertex v1
                     const vec3 &v2, // vertex v2
                     const vec3 &v3, // vertex v3
                     const vec3 &o,  // origin of cutting plane
                     const vec3 &n)  // normal of cutting plane
{
   double d0,d1,d2,d3;

   int ff;

   d0=(v0-o).dot(n);
   d1=(v1-o).dot(n);
   d2=(v2-o).dot(n);
   d3=(v3-o).dot(n);

   ff=0;

   if (d0<0.0) ff|=1;
   if (d1<0.0) ff|=2;
   if (d2<0.0) ff|=4;
   if (d3<0.0) ff|=8;

   switch (ff)
   {
      // 1 triangle
      case 1: case 14: lglSlice1Tri(v0,fabs(d0),
                                    v1,fabs(d1),
                                    v2,fabs(d2),
                                    v3,fabs(d3)); break;
      case 2: case 13: lglSlice1Tri(v1,fabs(d1),
                                    v0,fabs(d0),
                                    v2,fabs(d2),
                                    v3,fabs(d3)); break;
      case 4: case 11: lglSlice1Tri(v2,fabs(d2),
                                    v0,fabs(d0),
                                    v1,fabs(d1),
                                    v3,fabs(d3)); break;
      case 8: case 7:  lglSlice1Tri(v3,fabs(d3),
                                    v0,fabs(d0),
                                    v1,fabs(d1),
                                    v2,fabs(d2)); break;

      // 2 triangles
      case 3:  lglSlice2Tri(v0,fabs(d0),
                            v1,fabs(d1),
                            v2,fabs(d2),
                            v3,fabs(d3)); break;
      case 5:  lglSlice2Tri(v0,fabs(d0),
                            v2,fabs(d2),
                            v1,fabs(d1),
                            v3,fabs(d3)); break;
      case 6:  lglSlice2Tri(v1,fabs(d1),
                            v2,fabs(d2),
                            v0,fabs(d0),
                            v3,fabs(d3)); break;
      case 9:  lglSlice2Tri(v0,fabs(d0),
                            v3,fabs(d3),
                            v1,fabs(d1),
                            v2,fabs(d2)); break;
      case 10: lglSlice2Tri(v1,fabs(d1),
                            v3,fabs(d3),
                            v0,fabs(d0),
                            v2,fabs(d2)); break;
      case 12: lglSlice2Tri(v2,fabs(d2),
                            v3,fabs(d3),
                            v0,fabs(d0),
                            v1,fabs(d1)); break;
   }
}

#endif
