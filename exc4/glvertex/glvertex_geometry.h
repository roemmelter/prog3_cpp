// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL geometry classes (with lglVBO as base class)
#ifndef GLVERTEX_GEOMETRY_H
#define GLVERTEX_GEOMETRY_H

#include "glvertex.h"
#include "glvertex_bezier.h"
#include "glvertex_objformat.h"
#include "glvertex_vectortext.h"

//! unit cube vbo
//!
//! the lglCube class provides a pre-defined vbo, which contains a single cube with unit size
//! * the origin of the cube's coordinate system corresponds to its barycenter
//! * as it is unit sized, the extents range from -0.5 to 0.5
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglCube: public lglVBO
{
public:

   lglCube(bool color = false)
      : lglVBO("cube")
   {
      addQuads(this, color);
   }

   static void addQuads(lglVBO *vbo, bool color = false, double dx = 1, double dy = 1, double dz = 1, bool facecolors = true)
   {
      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUADS);

         if (color && !facecolors) vbo->lglColor(col);

         // left quad
         if (color && facecolors) vbo->lglColor(1,0,0);
         vbo->lglNormal(-1,0,0);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(-0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(-0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(-1,1);
         vbo->lglVertex(-0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(-1,0);
         vbo->lglVertex(-0.5*dx,0.5*dy,-0.5*dz);

         // right quad
         if (color && facecolors) vbo->lglColor(0,1,1);
         vbo->lglNormal(1,0,0);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(0.5*dx,0.5*dy,-0.5*dz);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(0.5*dx,0.5*dy,0.5*dz);

         // front quad
         if (color && facecolors) vbo->lglColor(0,1,0);
         vbo->lglNormal(0,0,1);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(-0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,-1);
         vbo->lglVertex(0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(0,-1);
         vbo->lglVertex(-0.5*dx,0.5*dy,0.5*dz);

         // back quad
         if (color && facecolors) vbo->lglColor(1,0,1);
         vbo->lglNormal(0,0,-1);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(-0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,2);
         vbo->lglVertex(-0.5*dx,0.5*dy,-0.5*dz);
         vbo->lglTexCoord(1,2);
         vbo->lglVertex(0.5*dx,0.5*dy,-0.5*dz);

         // bottom quad
         if (color && facecolors) vbo->lglColor(0,0,1);
         vbo->lglNormal(0,-1,0);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(-0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(-0.5*dx,-0.5*dy,0.5*dz);

         // top quad
         if (color && facecolors) vbo->lglColor(1,1,0);
         vbo->lglNormal(0,1,0);
         vbo->lglTexCoord(3,0);
         vbo->lglVertex(-0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(0.5*dx,0.5*dy,-0.5*dz);
         vbo->lglTexCoord(3,1);
         vbo->lglVertex(-0.5*dx,0.5*dy,-0.5*dz);

      vbo->lglEnd();
   }

   static void addTo(lglVBO *vbo, bool color = false, double dx = 1, double dy = 1, double dz = 1, bool facecolors = true)
   {
      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color && !facecolors) vbo->lglColor(col);

         // left quad
         if (color && facecolors) vbo->lglColor(1,0,0);
         vbo->lglNormal(-1,0,0);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(-0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(-0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(-1,0);
         vbo->lglVertex(-0.5*dx,0.5*dy,-0.5*dz);
         vbo->lglTexCoord(-1,1);
         vbo->lglVertex(-0.5*dx,0.5*dy,0.5*dz);

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         // right quad
         if (color && facecolors) vbo->lglColor(0,1,1);
         vbo->lglNormal(1,0,0);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(0.5*dx,0.5*dy,-0.5*dz);

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         // front quad
         if (color && facecolors) vbo->lglColor(0,1,0);
         vbo->lglNormal(0,0,1);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(-0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(0,-1);
         vbo->lglVertex(-0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,-1);
         vbo->lglVertex(0.5*dx,0.5*dy,0.5*dz);

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         // back quad
         if (color && facecolors) vbo->lglColor(1,0,1);
         vbo->lglNormal(0,0,-1);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(-0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(1,2);
         vbo->lglVertex(0.5*dx,0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,2);
         vbo->lglVertex(-0.5*dx,0.5*dy,-0.5*dz);

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         // bottom quad
         if (color && facecolors) vbo->lglColor(0,0,1);
         vbo->lglNormal(0,-1,0);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(-0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(0.5*dx,-0.5*dy,-0.5*dz);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(-0.5*dx,-0.5*dy,0.5*dz);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(0.5*dx,-0.5*dy,0.5*dz);

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         // top quad
         if (color && facecolors) vbo->lglColor(1,1,0);
         vbo->lglNormal(0,1,0);
         vbo->lglTexCoord(3,0);
         vbo->lglVertex(-0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(0.5*dx,0.5*dy,0.5*dz);
         vbo->lglTexCoord(3,1);
         vbo->lglVertex(-0.5*dx,0.5*dy,-0.5*dz);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(0.5*dx,0.5*dy,-0.5*dz);

      vbo->lglEnd();
   }

};

//! rectangular box vbo
//!
//! the lglBox class provides a pre-defined vbo, which contains a single rectangular box with pre-defined edge lengths
//! * the origin of the box' coordinate system corresponds to its barycenter
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglBox: public lglVBO
{
public:

   lglBox(double dx, double dy, double dz,
          bool color = false)
      : lglVBO("box")
   {
      lglCube::addQuads(this, color, dx, dy, dz);
   }

   lglBox(vec3 size,
          bool color = false)
      : lglVBO("box")
   {
      lglCube::addQuads(this, color, size.x, size.y, size.z);
   }

   static void addQuads(lglVBO *vbo, bool color = false, double dx = 1, double dy = 1, double dz = 1, bool facecolors = true)
   {
      lglCube::addQuads(vbo, color, dx, dy, dz, facecolors);
   }

   static void addTo(lglVBO *vbo, bool color = false, double dx = 1, double dy = 1, double dz = 1, bool facecolors = true)
   {
      lglCube::addTo(vbo, color, dx, dy, dz, facecolors);
   }

};

//! unit tetrahedron vbo
//!
//! the lglTet class provides a pre-defined vbo, which contains a single tetrahedron with unit size
//! * the origin of the tetrahedron's coordinate system corresponds to its barycenter
//! * as it is unit sized, the extents range from -0.5 to 0.5 along the x-coordinate
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglTet: public lglVBO
{
public:

   lglTet(bool color = false)
      : lglVBO("tetrahedron")
   {
      addTriangles(this, color);
   }

   static void addTriangles(lglVBO *vbo, bool color = false, bool facecolors = true)
   {
      static const double c1 = sqrt(3.0)/6;
      static const double c2 = sqrt(6.0)/6;
      static const double c3 = sqrt(3.0)/3;

      static const vec3 a(-0.5,-c2,c1);
      static const vec3 b(0.5,-c2,c1);
      static const vec3 c(0,-c2,-c3);
      static const vec3 d(0,c2,0);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_TRIANGLES);

         if (color && !facecolors) vbo->lglColor(col);

         // bottom triangle
         if (color && facecolors) vbo->lglColor(1,1,1);
         vbo->lglNormal((c-b).cross(b-a).normalize());
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(a);

         // side triangle
         if (color && facecolors) vbo->lglColor(1,0,0);
         vbo->lglNormal((a-b).cross(b-d).normalize());
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);

         // side triangle
         if (color && facecolors) vbo->lglColor(0,1,0);
         vbo->lglNormal((b-c).cross(c-d).normalize());
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);

         // side triangle
         if (color && facecolors) vbo->lglColor(0,0,1);
         vbo->lglNormal((c-a).cross(a-d).normalize());
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);

      vbo->lglEnd();
   }

};

//! unit pyramid vbo
//!
//! the lglPyramid class provides a pre-defined vbo, which contains a single pyramid with unit size
//! * the origin of the pyramid's coordinate system corresponds to its center (but not its barycenter)
//! * as it is unit sized, the extents range from -0.5 to 0.5
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglPyramid: public lglVBO
{
public:

   lglPyramid(bool color = false)
      : lglVBO("pyramid")
   {
      addTriangles(this, color);
   }

   static void addTriangles(lglVBO *vbo, bool color = false, bool facecolors = true)
   {
      static const vec3 a(-0.5,-0.5,0.5);
      static const vec3 b(0.5,-0.5,0.5);
      static const vec3 c(0.5,-0.5,-0.5);
      static const vec3 d(-0.5,-0.5,-0.5);
      static const vec3 e(0,0.5,0);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_TRIANGLES);

         if (color && !facecolors) vbo->lglColor(col);

         // bottom triangles
         if (color && facecolors) vbo->lglColor(1,1,1);
         vbo->lglNormal((d-c).cross(c-b).normalize());
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(c);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(b);
         vbo->lglVertex(b);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);

         // front triangle
         if (color && facecolors) vbo->lglColor(1,1,0);
         vbo->lglNormal((a-b).cross(b-e).normalize());
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(1.5,0.5);
         vbo->lglVertex(e);

         // right triangle
         if (color && facecolors) vbo->lglColor(0,1,1);
         vbo->lglNormal((b-c).cross(c-e).normalize());
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(c);
         vbo->lglTexCoord(1.5,0.5);
         vbo->lglVertex(e);

         // back triangle
         if (color && facecolors) vbo->lglColor(0,0,1);
         vbo->lglNormal((c-d).cross(d-e).normalize());
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(c);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);
         vbo->lglTexCoord(1.5,0.5);
         vbo->lglVertex(e);

         // left triangle
         if (color && facecolors) vbo->lglColor(1,0,0);
         vbo->lglNormal((d-a).cross(a-e).normalize());
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1.5,0.5);
         vbo->lglVertex(e);

      vbo->lglEnd();
   }

};

//! pyramid base vbo
//!
//! the lglPyramidBase class provides a pre-defined vbo, which contains a single pyramid base
//! * the origin of the pyramid base's coordinate system corresponds to its center (but not its barycenter)
//! * by default the pyramid base is cut off at half height (base = 0.5)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglPyramidBase: public lglVBO
{
public:

   lglPyramidBase(double size = 1, double height = 1, double base = 0.5, bool color = false)
      : lglVBO()
   {
      std::string name = "pyramid_base";
      name += std::string("(");
      name += glslmath::to_string(size) + ", ";
      name += glslmath::to_string(height) + ", ";
      name += glslmath::to_string(base);
      name += std::string(")");
      setName(name);

      addTriangles(this, size, height, base, color);
   }

   static void addTriangles(lglVBO *vbo, double size = 1, double height = 1, double base = 0.5, bool color = false)
   {
      const vec3 a(-0.5*size,-0.5*base,0.5*size);
      const vec3 b(0.5*size,-0.5*base,0.5*size);
      const vec3 c(0.5*size,-0.5*base,-0.5*size);
      const vec3 d(-0.5*size,-0.5*base,-0.5*size);
      const vec3 e(0,-0.5*base+height,0);

      double w = base/height;
      const vec3 f = (1-w)*a + w*e;
      const vec3 g = (1-w)*b + w*e;
      const vec3 h = (1-w)*c + w*e;
      const vec3 i = (1-w)*d + w*e;

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUADS);

         if (color) vbo->lglColor(col);

         // bottom quad
         vbo->lglNormal((d-c).cross(c-b).normalize());
         vbo->lglTexCoord(0,-1);
         vbo->lglVertex(d);
         vbo->lglTexCoord(1,-1);
         vbo->lglVertex(c);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(a);

         // front quad
         vbo->lglNormal((a-b).cross(b-e).normalize());
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(g);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(f);

         // right quad
         vbo->lglNormal((b-c).cross(c-e).normalize());
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(h);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(g);

         // back quad
         vbo->lglNormal((c-d).cross(d-e).normalize());
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(3,0);
         vbo->lglVertex(d);
         vbo->lglTexCoord(3,1);
         vbo->lglVertex(i);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(h);

         // left quad
         vbo->lglNormal((d-a).cross(a-e).normalize());
         vbo->lglTexCoord(-1,0);
         vbo->lglVertex(d);
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(f);
         vbo->lglTexCoord(-1,1);
         vbo->lglVertex(i);

         // top quad
         if (w < 1)
         {
            vbo->lglNormal((f-g).cross(g-h).normalize());
            vbo->lglTexCoord(0,1);
            vbo->lglVertex(f);
            vbo->lglTexCoord(1,1);
            vbo->lglVertex(g);
            vbo->lglTexCoord(1,2);
            vbo->lglVertex(h);
            vbo->lglTexCoord(0,2);
            vbo->lglVertex(i);
         }

      vbo->lglEnd();
   }

};

//! unit prism vbo
//!
//! the lglPrism class provides a pre-defined vbo, which contains a single slanted prism with unit size
//! * the origin of the prism's coordinate system corresponds to its center (but not its barycenter)
//! * as it is unit sized, the extents range from -0.5 to 0.5
//! * negative slanting values mean that the prism is slanted to the left
//! * positive slanting values mean that the prism is slanted to the right
//! * a slanting value of -0.5 means that the prism is a wedge with a right angle at the bottom left edge
//! * a slanting value of 0.5 means that the prism is a wedge with a right angle at the bottom right edge
//! * by default the prism is not slanted to either side (slanting = 0)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglPrism: public lglVBO
{
public:

   lglPrism(double slanting = 0, bool color = false)
      : lglVBO("prism")
   {
      addTriangles(this, slanting, color);
   }

   static void addTriangles(lglVBO *vbo, double slanting = 0, bool color = false, bool facecolors = true)
   {
      static const vec3 a(-0.5,-0.5,0.5);
      static const vec3 b(0.5,-0.5,0.5);
      static const vec3 c(slanting,0.5,0.5);
      static const vec3 d(-0.5,-0.5,-0.5);
      static const vec3 e(0.5,-0.5,-0.5);
      static const vec3 f(slanting,0.5,-0.5);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_TRIANGLES);

         if (color && !facecolors) vbo->lglColor(col);

         // front triangle
         if (color && facecolors) vbo->lglColor(1,1,0);
         vbo->lglNormal((a-b).cross(b-c).normalize());
         vbo->lglTexCoord(0,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(c);

         // back triangle
         if (color && facecolors) vbo->lglColor(0,1,1);
         vbo->lglNormal((f-e).cross(e-d).normalize());
         vbo->lglTexCoord(0,1);
         vbo->lglVertex(f);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(e);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);

         // left triangles
         if (color && facecolors) vbo->lglColor(1,0,0);
         vbo->lglNormal((f-d).cross(d-a).normalize());
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(f);
         vbo->lglTexCoord(1,1);
         vbo->lglVertex(d);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(1,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(f);

         // right triangles
         if (color && facecolors) vbo->lglColor(0,1,0);
         vbo->lglNormal((e-f).cross(f-c).normalize());
         vbo->lglTexCoord(3,1);
         vbo->lglVertex(e);
         vbo->lglTexCoord(2,1);
         vbo->lglVertex(f);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(2,0);
         vbo->lglVertex(c);
         vbo->lglTexCoord(3,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(3,1);
         vbo->lglVertex(e);

         // bottom triangles
         if (color && facecolors) vbo->lglColor(0,0,1);
         vbo->lglNormal((d-e).cross(e-b).normalize());
         vbo->lglTexCoord(4,1);
         vbo->lglVertex(d);
         vbo->lglTexCoord(3,1);
         vbo->lglVertex(e);
         vbo->lglTexCoord(3,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(3,0);
         vbo->lglVertex(b);
         vbo->lglTexCoord(4,0);
         vbo->lglVertex(a);
         vbo->lglTexCoord(5,1);
         vbo->lglVertex(d);

      vbo->lglEnd();
   }

};

//! unit sphere vbo
//!
//! the lglSphere class provides a pre-defined vbo, which contains a single sphere with unit size
//! * the origin of the sphere's coordinate system corresponds to its barycenter
//! * as it is unit sized, the extents range from -0.5 to 0.5
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglSphere: public lglVBO
{
public:

   lglSphere(bool color = false, int tessel = 8)
      : lglVBO("sphere")
   {
      addTo(this, color, tessel);
   }

   static void addTo(lglVBO *vbo, bool color = false, int tessel = 8)
   {
      int i, j;

      double u, v;
      double alpha, beta;
      vec3 p;

      int alpha_steps = 4*tessel;
      int beta_steps = tessel;

      vec4 col = vbo->lglGetColor();

      for (j=-beta_steps; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)j/beta_steps;

               alpha = u*2*PI;
               beta = v*PI/2;

               p = vec3(sin(alpha)*cos(beta),
                        cos(alpha)*cos(beta),
                        sin(beta));

               vbo->lglNormal(p);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p/2);

               v = (double)(j+1)/beta_steps;
               beta = v*PI/2;

               p = vec3(sin(alpha)*cos(beta),
                        cos(alpha)*cos(beta),
                        sin(beta));

               vbo->lglNormal(p);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p/2);
            }

         vbo->lglEnd();
      }
   }

};

//! unit hemisphere vbo
//!
//! the lglHemisphere class provides a pre-defined vbo, which contains a single hemisphere with unit size
//! * it is the same as a sphere except for being just the top half
class lglHemisphere: public lglVBO
{
public:

   lglHemisphere(bool color = false, int tessel = 8)
      : lglVBO("hemisphere")
   {
      addTo(this, color, tessel);
   }

   static void addTo(lglVBO *vbo, bool color = false, int tessel = 8)
   {
      int i, j;

      double u, v;
      double alpha, beta;
      vec3 p;

      int alpha_steps = 4*tessel;
      int beta_steps = tessel;

      vec4 col = vbo->lglGetColor();

      for (j=0; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)j/beta_steps;

               alpha = u*2*PI;
               beta = v*PI/2;

               p = vec3(sin(alpha)*cos(beta),
                        sin(beta),
                        -cos(alpha)*cos(beta));

               vbo->lglNormal(p);
               vbo->lglTexCoord(1.0f-(float)u,1.0f-(float)v);
               vbo->lglVertex(p/2);

               v = (double)(j+1)/beta_steps;
               beta = v*PI/2;

               p = vec3(sin(alpha)*cos(beta),
                        sin(beta),
                        -cos(alpha)*cos(beta));

               vbo->lglNormal(p);
               vbo->lglTexCoord(1.0f-(float)u,1.0f-(float)v);
               vbo->lglVertex(p/2);
            }

         vbo->lglEnd();
      }
   }

};

//! unit cylinder vbo
//!
//! the lglCylinder class provides a pre-defined vbo, which contains a single cylinder with unit size
//! * the rotation axis of the cylinder corresponds to the z-axis
//! * as it is unit sized, the extents range from -0.5 to 0.5
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglCylinder: public lglVBO
{
public:

   lglCylinder(bool caps = false, bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "cylinder";
      name += std::string("(");
      name += std::string(caps?"caps":"nocaps");
      name += std::string(")");
      setName(name);

      addTo(this, caps, color, tessel);
   }

   static void addTo(lglVBO *vbo, bool caps = false, bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = 4*tessel;

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = u*2*PI;

            double s = sin(alpha);
            double c = cos(alpha);

            vbo->lglNormal(vec3(s,c,0));
            vbo->lglTexCoord(1.0f-(float)u,0);
            vbo->lglVertex(vec3(s,c,-1)/2);
            vbo->lglTexCoord(1.0f-(float)u,1);
            vbo->lglVertex(vec3(s,c,1)/2);
         }

      vbo->lglEnd();

      if (caps)
      {
         steps = 2*tessel;

         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            vbo->lglNormal(0,0,1);

            for (i=0; i<=steps; i++)
            {
               u = (double)i/steps;
               alpha = u*PI;

               double s = sin(alpha)/2;
               double c = cos(alpha)/2;

               vbo->lglTexCoord(1.5f+(float)s,0.5f+(float)c);
               vbo->lglVertex(vec3(s,c,0.5));

               vbo->lglTexCoord(1.5f-(float)s,0.5f+(float)c);
               vbo->lglVertex(vec3(-s,c,0.5));
            }

         vbo->lglEnd();

         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            vbo->lglNormal(0,0,-1);

            for (i=0; i<=steps; i++)
            {
               u = (double)i/steps;
               alpha = u*PI;

               double s = sin(alpha)/2;
               double c = cos(alpha)/2;

               vbo->lglTexCoord(1.5f+(float)s,0.5f+(float)c);
               vbo->lglVertex(vec3(-s,c,-0.5));

               vbo->lglTexCoord(1.5f-(float)s,0.5f+(float)c);
               vbo->lglVertex(vec3(s,c,-0.5));
            }

         vbo->lglEnd();
      }
   }

};

//! unit hemi-cylinder vbo
//!
//! the lglHemiCylinder class provides a pre-defined vbo, which contains a single hemi-cylinder with unit size
//! * it is the same as a cylinder except for being just the top half
class lglHemiCylinder: public lglVBO
{
public:

   lglHemiCylinder(bool caps = false, double angle = 180, bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "hemi_cylinder";
      name += std::string("(");
      name += std::string(caps?"caps":"nocaps") + ", ";
      name += glslmath::to_string(angle);
      name += std::string(")");
      setName(name);

      addTo(this, caps, angle, color, tessel);
   }

   static void addTo(lglVBO *vbo, bool caps = false, double angle = 180, bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = (int)ceil(angle/360*4*tessel);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = (u*angle/360-(angle-180)/2)*2*PI;

            double s = sin(alpha);
            double c = cos(alpha);

            vbo->lglNormal(vec3(-c,s,0));
            vbo->lglTexCoord(1.0f-(float)u,0);
            vbo->lglVertex(vec3(-c,s,-1)/2);
            vbo->lglTexCoord(1.0f-(float)u,1);
            vbo->lglVertex(vec3(-c,s,1)/2);
         }

      vbo->lglEnd();

      if (caps)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            vbo->lglNormal(0,0,1);

            for (i=0; i<=steps; i++)
            {
               u = (double)i/steps;
               alpha = u*PI;

               double s = sin(alpha)/2;
               double c = cos(alpha)/2;

               vbo->lglTexCoord(0.5f-(float)c,1);
               vbo->lglVertex(vec3(-c,0,0.5));

               vbo->lglTexCoord(0.5f-(float)c,1.0f-(float)s*2);
               vbo->lglVertex(vec3(-c,s,0.5));
            }

         vbo->lglEnd();

         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            vbo->lglNormal(0,0,-1);

            for (i=0; i<=steps; i++)
            {
               u = (double)i/steps;
               alpha = u*PI;

               double s = sin(alpha)/2;
               double c = cos(alpha)/2;

               vbo->lglTexCoord(1.5f+(float)c,1.0f-(float)s*2);
               vbo->lglVertex(vec3(-c,s,-0.5));

               vbo->lglTexCoord(1.5f+(float)c,1);
               vbo->lglVertex(vec3(-c,0,-0.5));
            }

         vbo->lglEnd();
      }
   }

};

//! unit disc vbo
//!
//! the lglDisc class provides a pre-defined vbo, which contains a single disc with unit size
//! * the origin of the disc's coordinate system corresponds to its barycenter
//! * as the radius is unit sized, the extents range from -1 to 1
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglDisc: public lglVBO
{
public:

   lglDisc(bool color = false, int tessel = 8)
      : lglVBO("disc")
   {
      addTo(this, color, tessel);
   }

   static void addTo(lglVBO *vbo, bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = 2*tessel;

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         vbo->lglNormal(0,0,1);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = u*PI;

            double s = sin(alpha)/2;
            double c = cos(alpha)/2;

            vbo->lglTexCoord(0.5f+(float)s,0.5f+(float)c);
            vbo->lglVertex(vec3(s,c,0));

            vbo->lglTexCoord(0.5f-(float)s,0.5f+(float)c);
            vbo->lglVertex(vec3(-s,c,0));
         }

      vbo->lglEnd();
   }

};

//! unit hemi-disc vbo
//!
//! the lglHemiDisc class provides a pre-defined vbo, which contains a single hemi-disc with unit size
//! * it is the same as a disc except for being just the top half
class lglHemiDisc: public lglVBO
{
public:

   lglHemiDisc(double angle = 180, bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "hemi_disc";
      name += std::string("(");
      name += glslmath::to_string(angle);
      name += std::string(")");
      setName(name);

      addTo(this, angle, color, tessel);
   }

   static void addTo(lglVBO *vbo, double angle = 180, bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = (int)ceil(angle/360*4*tessel);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         vbo->lglNormal(0,0,1);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = (u*angle/360-(angle-180)/2)*2*PI;

            double s = sin(alpha)/2;
            double c = cos(alpha)/2;

            if (angle > 180) vbo->lglTexCoord(0.5f,0.5f);
            else vbo->lglTexCoord(0.5f,1);
            vbo->lglVertex(vec3(0,0,0));

            if (angle > 180) vbo->lglTexCoord(0.5f-(float)c,0.5f-(float)s);
            else vbo->lglTexCoord(0.5f-(float)c,1.0f-(float)s*2);
            vbo->lglVertex(vec3(-c,s,0));
         }

      vbo->lglEnd();
   }

};

//! unit cone vbo
//!
//! the lglCone class provides a pre-defined vbo, which contains a single cone with unit size
//! * the origin of the cones's coordinate system corresponds to its center (but not its barycenter)
//! * as it is unit sized, the extents range from -0.5 to 0.5
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglCone: public lglVBO
{
public:

   lglCone(bool color = false, int tessel = 8)
      : lglVBO("cone")
   {
      addTo(this, color, tessel);
   }

   static void addTo(lglVBO *vbo, bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = 4*tessel;

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = u*2*PI;

            double s = sin(alpha);
            double c = cos(alpha);

            vbo->lglNormal(vec3(s,c,0.5));
            vbo->lglTexCoord(0.5f+(float)s/2,0.5f+(float)c/2);
            vbo->lglVertex(vec3(s,c,-1)/2);

            vbo->lglNormal(vec3(0,0,0));
            vbo->lglTexCoord(0.5f,0.5f);
            vbo->lglVertex(vec3(0,0,0.5));
         }

      vbo->lglEnd();
   }

};

//! cone base vbo
//!
//! the lglConeBase class provides a pre-defined vbo, which contains a single cone base
//! * the origin of the cone base's coordinate system corresponds to its center (but not its barycenter)
//! * by default the cone base is cut off at half height (base = 0.5)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglConeBase: public lglVBO
{
public:

   lglConeBase(double size = 1, double height = 1, double base = 0.5, bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "cone_base";
      name += std::string("(");
      name += glslmath::to_string(size) + ", ";
      name += glslmath::to_string(height) + ", ";
      name += glslmath::to_string(base);
      name += std::string(")");
      setName(name);

      addTo(this, size, height, base, color, tessel);
   }

   static void addTo(lglVBO *vbo,
                     double size = 1, double height = 1, double base = 0.5,
                     bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = 4*tessel;

      double w = base/height;
      vec3 t = vec3(0,-base/2+height,0);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = u*2*PI;

            double s = sin(alpha)*size/2;
            double c = cos(alpha)*size/2;
            vec3 v = vec3(s,-base/2,c);

            vbo->lglNormal(vec3(s,size*size/height/4,c));
            vbo->lglTexCoord((float)u,0.5f);
            vbo->lglVertex((1-w)*v+w*t);
            vbo->lglTexCoord((float)u,1);
            vbo->lglVertex(v);
         }

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = -u*2*PI;

            double s = sin(alpha)*size/2;
            double c = cos(alpha)*size/2;
            vec3 v = vec3(s,-base/2,c);

            vbo->lglNormal(vec3(0,-1,0));
            vbo->lglTexCoord((float)u,0);
            vbo->lglVertex(vec3(0,-base/2,0));
            vbo->lglTexCoord((float)u,0.5f);
            vbo->lglVertex(v);
         }

      vbo->lglEnd();

      if (w < 1)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=steps; i++)
            {
               u = (double)i/steps;
               alpha = u*2*PI;

               double s = sin(alpha)*size/2;
               double c = cos(alpha)*size/2;
               vec3 v = vec3(s,-base/2,c);

               vbo->lglNormal(vec3(0,1,0));
               vbo->lglTexCoord((float)u,0);
               vbo->lglVertex(vec3(0,base/2,0));
               vbo->lglTexCoord((float)u,0.5f);
               vbo->lglVertex((1-w)*v+w*t);
            }

         vbo->lglEnd();
      }
   }

};

//! ring vbo
//!
//! the lglRing class provides a pre-defined vbo, which contains a single ring
//! * the origin of the ring's coordinate system corresponds to its barycenter
//! * the first parameter of the ctor defines the outer radius (with standard radius 1)
//! * the second parameter of the ctor defines the inner radius (with standard radius 0)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglRing: public lglVBO
{
public:

   lglRing(double outer = 1.0, double inner = 0.0, double displace = 0.0,
           bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "ring";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(displace);
      name += std::string(")");
      setName(name);

      addTo(this, outer, inner, displace, color, tessel);
   }

   static void addTo(lglVBO *vbo,
                     double outer = 1.0, double inner = 0.0, double displace = 0.0,
                     bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = 4*tessel;

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = u*2*PI;

            double s = sin(alpha);
            double c = cos(alpha);

            double d = displace/(outer-inner);
            vbo->lglNormal(vec3(d*s,d*c,1));

            vbo->lglTexCoord((float)u,1);
            vbo->lglVertex(vec3(outer*s,outer*c,0));

            vbo->lglTexCoord((float)u,0);
            vbo->lglVertex(vec3(inner*s,inner*c,displace));
         }

      vbo->lglEnd();
   }

};

//! hemi-ring vbo
//!
//! the lglHemiRing class provides a pre-defined vbo, which contains a single hemi-ring
//! * it is the same as a ring except for being just the top half
class lglHemiRing: public lglVBO
{
public:

   lglHemiRing(double outer = 1.0, double inner = 0.0, double displace = 0.0,
               double angle = 180, bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "hemi_ring";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(displace) + ", ";
      name += glslmath::to_string(angle);
      name += std::string(")");
      setName(name);

      addTo(this, outer, inner, displace, angle, color, tessel);
   }

   static void addTo(lglVBO *vbo,
                     double outer = 1.0, double inner = 0.0, double displace = 0.0,
                     double angle = 180, bool color = false, int tessel = 8)
   {
      int i;

      double u;
      double alpha;

      int steps = (int)ceil(angle/360*4*tessel);

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         for (i=0; i<=steps; i++)
         {
            u = (double)i/steps;
            alpha = (u*angle/360-(angle-180)/2)*2*PI;

            double s = sin(alpha);
            double c = cos(alpha);

            double d = displace/(outer-inner);
            vbo->lglNormal(vec3(-d*c,d*s,1));

            vbo->lglTexCoord((float)u,1);
            vbo->lglVertex(vec3(-outer*c,outer*s,0));

            vbo->lglTexCoord((float)u,0);
            vbo->lglVertex(vec3(-inner*c,inner*s,displace));
         }

      vbo->lglEnd();
   }

};

//! arc vbo
//!
//! the lglArc class provides a pre-defined vbo, which contains a single arc
class lglArc: public lglVBO
{
public:

   lglArc(double outer = 1.0, double inner = 0.0,
          double angle = 180, bool color = false, int tessel = 8)
      : lglVBO()
   {
      std::string name = "arc";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(angle);
      name += std::string(")");
      setName(name);

      addTo(this, outer, inner, angle, color, tessel);
   }

   static void addTo(lglVBO *vbo,
                     double outer = 1.0, double inner = 0.0,
                     double angle = 180, bool color = false, int tessel = 8)
   {
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPushMatrix();
      vbo->lglScale(outer);
      lglHemiCylinder::addTo(vbo, false, angle, color, tessel);
      vbo->lglPopMatrix();
      vbo->lglPushMatrix();
      vbo->lglScale(-inner);
      vbo->lglRotateZ(180);
      lglHemiCylinder::addTo(vbo, false, angle, color, tessel);
      vbo->lglPopMatrix();
      vbo->lglPushMatrix();
      vbo->lglTranslate(0,0,0.5);
      lglHemiRing::addTo(vbo, outer, inner, 0, angle, color, tessel);
      vbo->lglPopMatrix();
      vbo->lglPushMatrix();
      vbo->lglTranslate(0,0,-0.5);
      vbo->lglScale(0,0,-1);
      lglHemiRing::addTo(vbo, outer, inner, 0, angle, color, tessel);
      vbo->lglPopMatrix();
      vbo->lglMatrixMode(LGL_MODELVIEW);
   }

};

//! unit obj vbo
//!
//! the lglObj class provides a vbo that is pre-loaded from an OBJ file
//! * it is scaled to be unit sized (by a factor of 1/lglGetMaxAbsCoord)
class lglObj: public lglVBO
{
public:

   lglObj()
      : lglVBO(),
        maxcoord(0)
   {}

   lglObj(const char *path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
      : lglVBO(),
        maxcoord(0)
   {
      load(path, silent, texgen);
   }

   lglObj(std::string path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
      : lglVBO(),
        maxcoord(0)
   {
      load(path, silent, texgen);
   }

   bool load(const char *path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
   {
      bool ok = lglLoadObj(this, path, silent, texgen);
      if (ok) maxcoord = lglGetMaxAbsCoord();
      return(ok);
   }

   bool load(std::string path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
   {
      return(load(path.c_str(), silent, texgen));
   }

   ~lglObj()
   {}

   virtual void lglRender(const LGL_VBO_TYPE *vbo = NULL)
   {
      if (maxcoord > 0)
      {
         lglPushMatrix();
            lglScale(0.5/maxcoord);
            lglVBO::lglRender(vbo);
         lglPopMatrix();
      }
   }

protected:

   double maxcoord;
};

//! unit teapot vbo
//!
//! the lglTeapot class provides a teapot as pre-defined vbo
//! * it is known as the so called Utah Teapot
//! * this teapot was modeled after a Melitta Teapot manufactored by Friesland Co., Germany
//! * it is unit sized
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglTeapot: public lglObj
{
public:

   lglTeapot(lgl_texgenmode_enum texgen = LGL_TEXGEN_MIXED_HEDGEHOG)
      : lglObj("teapot.obj", true, texgen)
   {}

};

//! rotational sheet geometry (base class)
class lglSheet: public lglVBO
{
public:

   lglSheet(const std::string &name = "sheet")
      : lglVBO(name)
   {}

   void create(int tessel = 8, double size = 1.0, bool color = false)
   {
      clear();
      addTo(this, tessel, size, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, double size = 1.0, bool color = false)
   {
      double u;
      vec2 op, ip, n;
      bool align;

      int steps = 4*tessel;

      vec4 col = vbo->lglGetColor();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         align = true;

         for (u=outer_first(steps, &align);
              u<outer_last(steps);
              u=outer_next(u, steps, &align))
         {
            op = outer_point(u);

            if (align)
            {
               n = outer_normal(u);

               vbo->lglNormal(vec3(n,0));
               vbo->lglTexCoord(op);
               vbo->lglVertex(vec3(op,-0.5*size));
               vbo->lglVertex(vec3(op,0.5*size));
            }
            else
            {
               n = outer_normal(u, -1);

               vbo->lglNormal(vec3(n,0));
               vbo->lglTexCoord(op);
               vbo->lglVertex(vec3(op,-0.5*size));
               vbo->lglVertex(vec3(op,0.5*size));

               n = outer_normal(u, +1);

               vbo->lglNormal(vec3(n,0));
               vbo->lglTexCoord(op);
               vbo->lglVertex(vec3(op,-0.5*size));
               vbo->lglVertex(vec3(op,0.5*size));
            }
         }

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         align = true;

         for (u=inner_first(steps, &align);
              u<inner_last(steps);
              u=inner_next(u, steps, &align))
         {
            ip = inner_point(u);

            if (align)
            {
               n = inner_normal(u);

               vbo->lglNormal(vec3(n,0));
               vbo->lglTexCoord(ip);
               vbo->lglVertex(vec3(ip,0.5*size));
               vbo->lglVertex(vec3(ip,-0.5*size));
            }
            else
            {
               n = inner_normal(u, -1);

               vbo->lglNormal(vec3(n,0));
               vbo->lglTexCoord(ip);
               vbo->lglVertex(vec3(ip,0.5*size));
               vbo->lglVertex(vec3(ip,-0.5*size));

               n = inner_normal(u, +1);

               vbo->lglNormal(vec3(n,0));
               vbo->lglTexCoord(ip);
               vbo->lglVertex(vec3(ip,0.5*size));
               vbo->lglVertex(vec3(ip,-0.5*size));
            }
         }

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         vbo->lglNormal(vec3(0,0,-1));
         for (u=0; u<1+0.5/steps; u=next(u, steps))
         {
            op = outer_point(u);
            ip = inner_point(u);

            vbo->lglTexCoord(ip);
            vbo->lglVertex(vec3(ip,-0.5*size));
            vbo->lglTexCoord(op);
            vbo->lglVertex(vec3(op,-0.5*size));
         }

      vbo->lglEnd();

      vbo->lglBegin(LGL_QUAD_STRIP);

         if (color) vbo->lglColor(col);

         vbo->lglNormal(vec3(0,0,1));
         for (u=0; u<1+0.5/steps; u=next(u, steps))
         {
            op = outer_point(u);
            ip = inner_point(u);

            vbo->lglTexCoord(op);
            vbo->lglVertex(vec3(op,0.5*size));
            vbo->lglTexCoord(ip);
            vbo->lglVertex(vec3(ip,0.5*size));
         }

      vbo->lglEnd();
   }

protected:

   virtual vec2 outer_point(double u) = 0;

   virtual vec2 inner_point(double u)
   {
      return(vec2(0,0));
   }

   virtual vec2 outer_normal(double u, int mode = 0, double d = 0.001)
   {
      int k = outer_tessel();

      double maxd = 1;
      if (k > 0) maxd = 0.01/k;
      if (maxd < d) d = maxd;

      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = outer_point(u-ld);
      vec2 p2 = outer_point(u+rd);

      vec2 df = p2-p1;
      vec2 n = vec2(-df.y, df.x);

      return(n.normalize());
   }

   virtual vec2 inner_normal(double u, int mode = 0, double d = 0.001)
   {
      int k = inner_tessel();

      double maxd = 1;
      if (k > 0) maxd = 0.01/k;
      if (maxd < d) d = maxd;

      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = inner_point(u-ld);
      vec2 p2 = inner_point(u+rd);

      vec2 df = p2-p1;
      vec2 n = vec2(df.y, -df.x);

      return(n.normalize());
   }

   virtual double outer_first(int steps, bool *align = NULL)
   {
      return(0);
   }

   virtual double inner_first(int steps, bool *align = NULL)
   {
      return(0);
   }

   virtual double outer_last(int steps)
   {
      return(1+0.5/steps);
   }

   virtual double inner_last(int steps)
   {
      return(1+0.5/steps);
   }

   virtual double outer_next(double u, int steps, bool *align = NULL)
   {
      return(u+1.0/steps);
   }

   virtual double inner_next(double u, int steps, bool *align = NULL)
   {
      return(u+1.0/steps);
   }

   virtual double next(double u, int steps)
   {
      double v = inner_next(u, steps);
      u = outer_next(u, steps);
      if (v < u) u = v;
      return(u);
   }

   virtual int outer_tessel() {return(1);}
   virtual int inner_tessel() {return(1);}
};

//! rotational bezier sheet geometry
//!
//! the lglSheetBezier class provides a pre-defined vbo, which contains rotational geometry
//! * the shape of the sheet is determined by an outer and inner bezier curve in the x- and y-plane
//! * the thickness of the sheet is constant along the z-coordinate (with default thickness 1)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglSheetBezier: public lglSheet
{
public:

   lglSheetBezier(const lgl_BezierPath2D &outer = lgl_BezierPath2D(),
                  const lgl_BezierPath2D &inner = lgl_BezierPath2D())
      : lglSheet("sheet_bezier"),
        outer_(outer), inner_(inner)
   {}

   void setOuterPath(const lgl_BezierPath2D &outer)
   {
      outer_ = outer;
   }

   void setInnerPath(const lgl_BezierPath2D &inner)
   {
      inner_ = inner;
   }

   void addOuterPoint(vec2 point, bool align = true)
   {
      outer_.addPoint(point, align);
   }

   void addInnerPoint(vec2 point, bool align = true)
   {
      inner_.addPoint(point, align);
   }

   void outerStraighten()
   {
      outer_.straighten();
   }

   void innerStraighten()
   {
      inner_.straighten();
   }

   void outerClose()
   {
      outer_.close();
   }

   void innerClose()
   {
      inner_.close();
   }

   void outerAlign()
   {
      outer_.alignCurves();
   }

   void innerAlign()
   {
      inner_.alignCurves();
   }

   void release()
   {
      inner_.clear();
      outer_.clear();
   }

protected:

   lgl_BezierPath2D outer_, inner_;

   virtual vec2 outer_point(double u)
   {
      return(outer_.evaluate(u));
   }

   virtual vec2 inner_point(double u)
   {
      return(inner_.evaluate(u));
   }

   virtual double outer_first(int steps, bool *align = NULL)
   {
      return(outer_.first(steps, align));
   }

   virtual double inner_first(int steps, bool *align = NULL)
   {
      return(inner_.first(steps, align));
   }

   virtual double outer_last(int steps)
   {
      return(outer_.last(steps));
   }

   virtual double inner_last(int steps)
   {
      return(inner_.last(steps));
   }

   virtual double outer_next(double u, int steps, bool *align = NULL)
   {
      return(outer_.next(u, steps, align));
   }

   virtual double inner_next(double u, int steps, bool *align = NULL)
   {
      return(inner_.next(u, steps, align));
   }

   virtual int outer_tessel() {return(outer_.size());}
   virtual int inner_tessel() {return(inner_.size());}
};

//! rotational disc sheet geometry
//!
//! the lglSheetDisc class provides a pre-defined vbo, which contains a single disc sheet with unit radius and thickness
//! * the origin of the sheet's coordinate system corresponds to its barycenter
//! * as the radius is unit sized, the extents range from -1 to 1 along the x- and y-coordinate
//! * as the thickness is unit sized, the extents range from -0.5 to 0.5 along the z-coordinate
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglSheetDisc: public lglSheet
{
public:

   lglSheetDisc(int tessel = 8, double size = 1.0, bool color = false)
      : lglSheet()
   {
      std::string name = "sheet_disc";
      name += std::string("(");
      name += glslmath::to_string(size);
      name += std::string(")");
      setName(name);

      addTo(this, tessel, size, color);
   }

protected:

   virtual vec2 outer_point(double u)
   {
      double a = u*2*PI;
      double s = sin(a);
      double c = cos(a);

      return(0.5*vec2(c,-s));
   }

};

//! rotational shim geometry
//!
//! the lglShim class provides a pre-defined vbo, which contains a single shim sheet with unit outer radius and thickness
//! * the origin of the sheet's coordinate system corresponds to its barycenter
//! * as the outer radius is unit sized, the extents range from -1 to 1 along the x- and y-coordinate
//! * as the thickness is unit sized, the extents range from -0.5 to 0.5 along the z-coordinate
//! * the first parameter of the ctor defines the outer radius (with standard radius 1)
//! * the second parameter of the ctor defines the inner radius (with standard radius 0)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglShim: public lglSheet
{
public:

   lglShim(double outer = 1.0, double inner = 0.0,
           int tessel = 8, double size = 1.0, bool color = false)
      : lglSheet(),
        outer_(outer), inner_(inner)
   {
      std::string name = "shim";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(size);
      name += std::string(")");
      setName(name);

      addTo(this, tessel, size, color);
   }

protected:

   double outer_, inner_;

   virtual vec2 outer_point(double u)
   {
      double a = u*2*PI;
      double s = sin(a);
      double c = cos(a);

      return(outer_*vec2(c,-s));
   }

   virtual vec2 inner_point(double u)
   {
      double a = u*2*PI;
      double s = sin(a);
      double c = cos(a);

      return(inner_*vec2(c,-s));
   }

};

//! rotational hemi-shim geometry
//!
//! the lglShim class provides a pre-defined vbo, which contains a single shim sheet with unit outer radius and thickness
//! * it is the same as a shim except for being just the top half
class lglHemiShim: public lglSheet
{
public:

   lglHemiShim(double outer = 1.0, double inner = 0.0,
           int tessel = 8, double size = 1.0, bool color = false)
      : lglSheet(),
        outer_(outer), inner_(inner)
   {
      std::string name = "hemi_shim";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(size);
      name += std::string(")");
      setName(name);

      addTo(this, tessel, size, color);
   }

protected:

   double outer_, inner_;

   virtual vec2 outer_point(double u)
   {
      double a = u*PI;
      double s = sin(a);
      double c = cos(a);

      return(outer_*vec2(s,-c));
   }

   virtual vec2 inner_point(double u)
   {
      double a = u*PI;
      double s = sin(a);
      double c = cos(a);

      return(inner_*vec2(s,-c));
   }

};

//! rotational spiral geometry
//!
//! the lglSpiral class provides a pre-defined vbo, which contains a single spiral sheet with unit outer radius and thickness
//! * the origin of the sheet's coordinate system corresponds to its barycenter
//! * as the outer radius is unit sized, the extents range from -1 to 1 along the x- and y-coordinate
//! * as the thickness is unit sized, the extents range from -0.5 to 0.5 along the z-coordinate
//! * the first parameter of the ctor defines the outer radius (with standard radius 1)
//! * the second parameter of the ctor defines the inner radius (with standard radius 0)
//! * the third parameter of the ctor defines the thickness of the spiral windings
//! * the fourth parameter of the ctor defines the number of spiral windings between the outer and inner radius
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglSpiral: public lglSheet
{
public:

   lglSpiral(double outer = 1.0, double inner = 0.1,
             double thickness = 0.01, double windings = 10.0,
             int tessel = 8, double size = 1.0, bool color = false)
      : lglSheet(),
        outer_(outer), inner_(inner),
        thickness_(thickness), windings_(windings)
   {
      std::string name = "spiral";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(size);
      name += std::string(")");
      setName(name);

      addTo(this, (int)ceil(tessel*windings+0.5), size, color);
   }

protected:

   double outer_, inner_;
   double thickness_, windings_;

   virtual vec2 outer_point(double u)
   {
      static const double p = 5.0;

      double a = u*2*PI*windings_;
      double s = sin(a);
      double c = cos(a);

      u = u/2;
      u = 1-pow(1-u, p);
      u = u/(1-pow(0.5, p));

      double d = (1-u)*(inner_+thickness_)+u*outer_;

      return(d*vec2(c,-s));
   }

   virtual vec2 inner_point(double u)
   {
      static const double p = 5.0;

      double a = u*2*PI*windings_;
      double s = sin(a);
      double c = cos(a);

      u = u/2;
      u = 1-pow(1-u, p);
      u = u/(1-pow(0.5, p));

      double d = (1-u)*(inner_+thickness_)+u*outer_-thickness_;

      return(d*vec2(c,-s));
   }

};

//! rotational gear geometry
//!
//! the lglGear class provides a pre-defined vbo, which contains a single gear sheet with unit thickness
//! * the origin of the sheet's coordinate system corresponds to its barycenter
//! * as the thickness is unit sized, the extents range from -0.5 to 0.5 along the z-coordinate
//! * the first parameter of the ctor defines the outer contact radius (with standard contact radius 1)
//! * the second parameter of the ctor defines the inner radius (with standard radius 0)
//! * the third parameter of the ctor defines the modul of the gear (the distance of two teeth)
//! * the fourth parameter of the ctor defines the contact angle (zero means angle is determined to be minimal)
//! * the fifth parameter of the ctor defines the height of the teeth above the gear's contact radius relative to the gear's modul
//! * the sixth parameter of the ctor defines the height of the teeth's recess relative to the gear's modul
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglGear: public lglSheetBezier
{
public:

   lglGear(double outer = 1.0, double inner = 0.0,
           double modul = 1.0/16, double angle = 0.0,
           double hk = 1.0,
           double hf = 0.167)
      : lglSheetBezier(),
        inner_radius_(inner)
   {
      std::string name = "gear";
      name += std::string("(");
      name += glslmath::to_string(outer) + ", ";
      name += glslmath::to_string(inner) + ", ";
      name += glslmath::to_string(modul) + ", ";
      name += glslmath::to_string(angle) + ", ";
      name += glslmath::to_string(hk) + ", ";
      name += glslmath::to_string(hf);
      name += std::string(")");
      setName(name);

      const int steps = 128;

      int z = (int)floor(2*outer/modul+0.5);
      modul = 2*outer/z;

      double d = outer;
      double df = outer-hk*modul;
      double dk = outer+hk*modul;
      double dg = outer-hk*modul;

      double a = acos(dg/d)*180/PI;
      if (angle > a) dg = d*cos(angle*PI/180);
      else angle = a;

      r_ = d;
      a_ = angle;

      m_ = modul;
      hk_ = hk;
      hf_ = hf;

      double u = -0.25*modul*PI/d;

      for (int i=0; i<z; i++)
      {
         int j = 0;
         double s;

         {
            vec2 p1 = d*vec2(cos(u),-sin(u));

            double l = sqrt(d*d-dg*dg);
            vec2 p2 = construct(vec2(0,0), p1, dg, l);

            double d = -l;
            vec2 p3 = evolve(vec2(0,0), -l, p2, d);

            for (int k=0; k<steps; k++)
            {
               vec2 a = p2;
               vec2 b = p3;
               double v = d;
               p3 = evolve(vec2(0,0), 2*l/steps, p2, d);
               if (p3.norm() > df*df)
               {
                  p2 = a;
                  p3 = b;
                  d = v;
                  break;
               }
            }

            addOuterPoint(p3*(df-hf*modul)/df, false);

            addOuterPoint(p3, false);
            for (; j<steps; j++)
            {
               vec2 p = p2;
               double v = d;
               p3 = evolve(vec2(0,0), 2*l/steps, p2, d);
               if (p3.norm() > dk*dk)
               {
                  p2 = p;
                  d = v;
                  break;
               }
               addOuterPoint(p3);
            }

            outerStraighten();

            s = d+l;
         }

         u += 0.5*modul*PI/d;

         {
            vec2 p1 = d*vec2(cos(u),-sin(u));

            double l = sqrt(d*d-dg*dg);
            vec2 p2 = construct(p1, vec2(0,0), l, dg);

            double d = l;
            vec2 p3 = evolve(vec2(0,0), s, p2, d);

            addOuterPoint(p3, false);
            for (; j>0; j--)
            {
               p3 = evolve(vec2(0,0), 2*l/steps, p2, d);
               addOuterPoint(p3);
            }

            outerStraighten();

            addOuterPoint(p3*(df-hf*modul)/df, false);
         }

         u += 0.5*modul*PI/d;
      }

      outerClose();
      outerAlign();
   }

   double contactRadius()
   {
      return(r_);
   }

   double contactAngle()
   {
      return(a_);
   }

   double toothModul()
   {
      return(m_);
   }

   int teeth()
   {
      return((int)floor(2*r_/m_+0.5));
   }

protected:

   double r_; // contact radius
   double a_; // contact angle

   double m_; // tooth modul
   double hk_; // tooth geometry
   double hf_; // foot geometry

   double inner_radius_;

   vec2 evolve(vec2 center, double delta, vec2 &point, double &dist)
   {
      double radius = (point-center).length();
      double angle = delta/radius*180/PI;

      mat2 R = mat2::rotate(angle);
      point = R * (point - center) + center;
      dist -= delta;

      vec2 d = point - center;
      vec2 n = vec2(d.y,-d.x).normalize();

      return(point + n*dist);
   }

   vec2 construct(vec2 p1, vec2 p2, double d1, double d2)
   {
      vec2 d = p2 - p1;
      double l = d.length();

      double h = d1*d2/l;
      double m = sqrt(d1*d1-h*h);

      vec2 p = p1 + m/l*d;
      vec2 n = vec2(d.y,-d.x)/l;

      return(p + n*h);
   }

   virtual vec2 inner_point(double u)
   {
      return(inner_radius_*outer_point(u).normalize());
   }

   virtual double inner_first(int steps, bool *align = NULL)
   {
      return(outer_first(steps, align));
   }

   virtual double inner_last(int steps)
   {
      return(outer_last(steps));
   }

   virtual double inner_next(double u, int steps, bool *align = NULL)
   {
      return(outer_next(u, steps, align));
   }

};

//! functional geometry (base class)
class lglFunctional: public lglVBO
{
public:

   lglFunctional(const std::string &name = "functional")
      : lglVBO(name)
   {}

protected:

   virtual vec3 point(double u, double v) = 0;

   virtual vec3 normal(double u, double v, double d,
                       bool swapx, bool swapy)
   {
      vec3 p1 = point(u-d, v);
      vec3 p2 = point(u+d, v);
      vec3 p3 = point(u, v-d);
      vec3 p4 = point(u, v+d);

      double dx = swapx? -d : d;
      double dy = swapy? -d : d;

      vec3 p5 = point(u-d, v-d);
      vec3 p6 = point(u+dx, v+dy);
      vec3 p7 = point(u-dx, v+d);
      vec3 p8 = point(u+d, v-dy);

      vec3 du = p2-p1+p6-p5+p8-p7;
      vec3 dv = p4-p3+p7-p8+p6-p5;

      vec3 n = dv.cross(du);

      return(n.normalize());
   }

   virtual vec3 normal(double u, double v, double d = 0.001)
   {
      return(normal(u, v, d, false, false));
   }

};

//! rotational geometry (base class)
class lglRotational: public lglFunctional
{
public:

   lglRotational(const std::string &name = "rotational")
      : lglFunctional(name),
        begin_(0), end_(1)
   {}

   void setRange(double begin, double end)
   {
      begin_ = begin/360;
      end_ = end/360;
   }

   void create(int tessel = 8, bool color = false)
   {
      clear();
      addTo(this, tessel, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, bool color = false)
   {
      int i, j;

      double u, v, w;
      vec3 p, n;

      int alpha_steps = 4*tessel;
      int beta_steps = (int)ceil(tessel*(end_-begin_));

      vec4 col = vbo->lglGetColor();

      for (j=-beta_steps; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)j/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p);

               v = (double)(j+1)/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p);
            }

         vbo->lglEnd();
      }
   }

protected:

   double begin_, end_;

   virtual vec3 point(double u, double v)
   {
      vec2 p = evaluate(u, v);
      vec4 p4 = vec4(0, p.y, p.x);

      double alpha = u*360;
      mat4 R = mat4::rotate(alpha, vec3(0,0,-1));

      return(R * p4);
   }

   virtual vec3 normal(double u, double v, double d = 0.001)
   {
      if (v==-1 || v==1)
         if (vec2(point(u,v)).norm() == 0)
         {
            if (v==-1)
               return(-lglFunctional::normal(u, v, d, true, false));
            else
               return(lglFunctional::normal(u, v, d, true, false));
         }

      return(lglFunctional::normal(u, v, d));
   }

   virtual vec2 evaluate(double u, double v) = 0;
};

//! rotational bezier geometry
//!
//! the lglRotationalBezier class provides a pre-defined vbo, which contains a rotational bezier geometry
//! * the shape of the geometry is determined by a 2D bezier curve that is rotated counter-clockwise about the negative z-axis
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglRotationalBezier: public lglRotational
{
public:

   lglRotationalBezier(vec2 a, vec2 b, vec2 c, vec2 d)
      : lglRotational("rotational_bezier"),
        path_(a, b, c, d)
   {}

   lglRotationalBezier(vec2 a, vec2 b)
      : lglRotational("rotational_bezier"),
        path_(a, b)
   {}

   lglRotationalBezier(const lgl_BezierPath2D &path)
      : lglRotational("rotational_bezier"),
        path_(path)
   {}

   lglRotationalBezier(const lgl_BezierMultiPath2D &path)
      : lglRotational("rotational_bezier"),
        path_(path)
   {}

   void release()
   {
      path_.clear();
   }

protected:

   lgl_BezierMultiPath2D path_;

   virtual vec2 evaluate(double u, double v)
   {
      double w = 0.5*v+0.5;
      return(path_.evaluate(u, w));
   }

};

//! rotational cylindrical geometry (base class)
//!
//! the lglCylindrical class provides a pre-defined vbo, which contains rotational geometry
//! * the shape of the geometry is determined by the distance from the z-axis
//! * the distance from the z-axis is specified by implementing the abstract evaluate() method (returning the distance)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglCylindrical: public lglFunctional
{
public:

   lglCylindrical(const std::string &name = "cylindrical")
      : lglFunctional(name),
        begin_(0), end_(1)
   {}

   void setRange(double begin, double end)
   {
      begin_ = begin/360;
      end_ = end/360;
   }

   void create(int tessel = 8, bool color = false)
   {
      clear();
      addTo(this, tessel, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, bool color = false)
   {
      int i, j;

      double u, v, w;
      vec3 p, n;

      int alpha_steps = 4*tessel;
      int beta_steps = (int)ceil(tessel*(end_-begin_));

      vec4 col = vbo->lglGetColor();

      for (j=-beta_steps; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)j/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(p);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p);

               v = (double)(j+1)/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p);
            }

         vbo->lglEnd();
      }
   }

protected:

   double begin_, end_;

   virtual vec3 point(double u, double v)
   {
      double alpha = u*2*PI;

      vec2 p = vec2(sin(alpha),
                    cos(alpha));

      double f = evaluate(u, v);

      return(vec3(f*p, v));
   }

   virtual vec3 normal(double u, double v, double d = 0.001)
   {
      if (v==-1 || v==1)
         if (vec2(point(u,v)).norm() == 0)
         {
            if (v==-1)
               return(-lglFunctional::normal(u, v, d, true, false));
            else
               return(lglFunctional::normal(u, v, d, true, false));
         }

      return(lglFunctional::normal(u, v, d));
   }

   virtual double evaluate(double u, double v) = 0;
};

//! rotational spherical geometry (base class)
//!
//! the lglSpherical class provides a pre-defined vbo, which contains rotational geometry
//! * the shape of the geometry is determined by the distance from the origin
//! * the distance from the origin is specified by implementing the abstract evaluate() method (returning the distance)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglSpherical: public lglFunctional
{
public:

   lglSpherical(const std::string &name = "spherical")
      : lglFunctional(name),
        begin_(0), end_(1)
   {}

   void setRange(double begin, double end)
   {
      begin_ = begin/360;
      end_ = end/360;
   }

   void create(int tessel = 8, bool color = false)
   {
      clear();
      addTo(this, tessel, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, bool color = false)
   {
      int i, j;

      double u, v, w;
      vec3 p, n;

      int alpha_steps = 4*tessel;
      int beta_steps = (int)ceil(tessel*(end_-begin_));

      vec4 col = vbo->lglGetColor();

      for (j=-beta_steps; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)j/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p);

               v = (double)(j+1)/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord(1.0f-(float)u,0.5f-(float)v/2);
               vbo->lglVertex(p);
            }

         vbo->lglEnd();
      }
   }

protected:

   double begin_, end_;

   virtual vec3 point(double u, double v)
   {
      double alpha = u*2*PI;
      double beta = v*PI/2;

      vec3 p = vec3(sin(alpha)*cos(beta),
                    cos(alpha)*cos(beta),
                    sin(beta));

      double f = evaluate(u, v);

      return(f*p);
   }

   virtual vec3 normal(double u, double v, double d = 0.001)
   {
      if (v==-1 || v==1)
         if (vec2(point(u,v)).norm() == 0)
         {
            if (v==-1)
               return(-lglFunctional::normal(u, v, d, true, false));
            else
               return(lglFunctional::normal(u, v, d, true, false));
         }

      return(lglFunctional::normal(u, v, d));
   }

   virtual double evaluate(double u, double v) = 0;
};

//! toroidal geometry (base class)
//!
//! the lglToroidal class provides a pre-defined vbo, which contains rotational geometry
//! * the shape of the geometry is determined by the distance from the unit circle
//! * the distance from the unit circle is specified by implementing the abstract evaluate() method (returning twice the distance)
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglToroidal: public lglFunctional
{
public:

   lglToroidal(const std::string &name = "toroidal")
      : lglFunctional(name),
        begin_(0), end_(1),
        diameter_(1)
   {}

   void setRange(double begin, double end)
   {
      begin_ = begin/360;
      end_ = end/360;
   }

   void setDiameter(double diameter)
   {
      diameter_ = diameter;
   }

   void create(int tessel = 8, bool color = false)
   {
      clear();
      addTo(this, tessel, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, bool color = false)
   {
      int i, j;

      double u, v, w;
      vec3 p, n;

      int alpha_steps = 4*tessel;
      int beta_steps = (int)ceil(8*tessel*(end_-begin_));

      vec4 col = vbo->lglGetColor();

      for (j=0; j<=beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)j/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord((float)u,(float)v);
               vbo->lglVertex(p);

               v = (double)(j+1)/beta_steps;

               w = (1-v)*begin_ + v*end_;

               p = point(u, w);
               n = normal(u, w);

               vbo->lglNormal(n);
               vbo->lglTexCoord((float)u,(float)v);
               vbo->lglVertex(p);
            }

         lglEnd();
      }
   }

protected:

   double begin_, end_;
   double diameter_;

   virtual vec3 point(double u, double v)
   {
      double alpha = u*2*PI;
      double beta = v*360;

      double f = evaluate(u, v)/2;

      vec2 p = vec2(sin(alpha),
                    cos(alpha));

      vec4 p4 = vec4(p.x*f-diameter_/2, p.y*f, 0);

      mat4 R = mat4::rotate(beta, vec3(0,1,0));

      return(R * p4);
   }

   virtual double evaluate(double u, double v) = 0;
};

//! torus geometry
//!
//! the lglTorus class provides a pre-defined vbo, which contains a torus
//! * the torus is a horizontal ring with a ring diameter and a tube radius
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglTorus: public lglToroidal
{
public:

   lglTorus(double diameter, double radius)
      : lglToroidal("torus"),
        radius_(radius)
   {
      setDiameter(diameter);
      create();
   }

protected:

   double radius_;

   virtual double evaluate(double u, double v)
   {
      return(radius_);
   }

};

//! hemi-torus geometry
//!
//! the lglHemiTorus class provides a pre-defined vbo, which contains a hemi-torus
//! * it is the same as a torus except for being just the front half
class lglHemiTorus: public lglToroidal
{
public:

   lglHemiTorus(double diameter, double radius,
                double begin = 0, double end = 180)
      : lglToroidal("hemi_torus"),
        radius_(radius)
   {
      setRange(begin, end);
      setDiameter(diameter);
      create();
   }

protected:

   double radius_;

   virtual double evaluate(double u, double v)
   {
      return(radius_);
   }

};

//! extrusion geometry (base class)
class lglExtrusion: public lglFunctional
{
public:

   lglExtrusion(const std::string &name = "extrusion")
      : lglFunctional(name)
   {}

   void create(int tessel = 8, bool color = false)
   {
      clear();
      addTo(this, tessel, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, bool color = false)
   {
      int i, j;

      double u, v;
      vec3 p, n;

      int alpha_steps = tessel * tessel_u();
      int beta_steps = tessel * tessel_v();

      vec4 col = vbo->lglGetColor();

      for (j=0; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)(j+1)/beta_steps;

               p = point(u, v);
               n = normal(u, v);

               vbo->lglNormal(n);
               vbo->lglTexCoord((float)u,1.0f-(float)v);
               vbo->lglVertex(p);

               v = (double)j/beta_steps;

               p = point(u, v);
               n = normal(u, v);

               vbo->lglNormal(n);
               vbo->lglTexCoord((float)u,1.0f-(float)v);
               vbo->lglVertex(p);
            }

         vbo->lglEnd();
      }
   }

protected:

   virtual vec3 point(double u, double v)
   {
      vec2 p = evaluate(u, v);
      return(extrude(p, v));
   }

   virtual vec3 normal(double u, double v, double d = 0.001)
   {
      int i = tessel_u();
      int j = tessel_v();

      double maxdu = 1;
      if (i > 0) maxdu = 0.01/i;
      if (maxdu < d) d = maxdu;

      double maxdv = 1;
      if (j > 0) maxdv = 0.01/j;
      if (maxdv < d) d = maxdv;

      return(-lglFunctional::normal(u, v, d));
   }

   virtual int tessel_u() {return(1);}
   virtual int tessel_v() {return(1);}

   virtual vec2 evaluate(double u, double v) = 0;
   virtual vec3 extrude(vec2 p, double v) = 0;
};

//! extrusion bezier geometry
//!
//! the lglExtrusionBezier class provides a pre-defined vbo, which contains an extruded bezier curve geometry
//! * the shape of the geometry is determined by a 2D bezier curve that is extruded along the path of a 3D bezier curve
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglExtrusionBezier: public lglExtrusion
{
public:

   lglExtrusionBezier(vec2 a, vec2 b, vec2 c, vec2 d,
                      const lgl_BezierPath3D &extrusion)
      : lglExtrusion("extrusion_bezier"),
        path_(a, b, c, d),
        extrusion_(extrusion)
   {}

   lglExtrusionBezier(vec2 a, vec2 b,
                      const lgl_BezierPath3D &extrusion)
      : lglExtrusion("extrusion_bezier"),
        path_(a, b),
        extrusion_(extrusion)
   {}

   lglExtrusionBezier(const lgl_BezierPath2D &path,
                      const lgl_BezierPath3D &extrusion)
      : lglExtrusion("extrusion_bezier"),
        path_(path),
        extrusion_(extrusion)
   {}

   lglExtrusionBezier(const lgl_BezierMultiPath2D &path,
                      const lgl_BezierPath3D &extrusion)
      : lglExtrusion("extrusion_bezier"),
        path_(path),
        extrusion_(extrusion)
   {}

   void release()
   {
      path_.clear();
      extrusion_.clear();
   }

protected:

   lgl_BezierMultiPath2D path_;
   lgl_BezierPath3D extrusion_;

   virtual vec2 evaluate(double u, double v)
   {
      return(path_.evaluate(v, u));
   }

   virtual vec3 extrude(vec2 p, double v)
   {
      static const int steps = 2*tessel_v();

      int n = (int)ceil(v*steps);

      quat q = quat::rotate(vec3(0,0,-1), extrusion_.gradient(0));

      for (int i=0; i<n; i++)
      {
         double w1 = (double)i/n*v;
         double w2 = (double)(i+1)/n*v;

         q *= quat::rotate(extrusion_.gradient(w1), extrusion_.gradient(w2));
      }

      vec3 r = q*vec3(p,0);
      vec3 o = extrusion_.evaluate(v);

      return(o+r);
   }

   virtual int tessel_u() {return(path_.getMaxCurves());}
   virtual int tessel_v() {return(extrusion_.size());}
};

//! replicated geometry
//!
//! the lglReplicatedGeometry class replicates a pre-defined vbo along a 3D bezier path
class lglReplicatedGeometry: public lglVBO
{
public:

   lglReplicatedGeometry(const lglVBO *vbo, const lgl_BezierPath3D &path, int n)
      : lglVBO("replicated_geometry")
   {
      lglMatrixMode(LGL_PREMODEL);
      lglPushMatrix();

      quat q = quat::rotate(vec3(0,0,-1), path.gradient(0));
      q *= quat::rotate(path.gradient(0), path.gradient(0.5/n));

      for (int i=0; i<n; i++)
      {
         double w = (double)(i+0.5)/n;

         lglLoadMatrix(mat4::transform(q, path.evaluate(w)));
         vbo->lglAppendVerticesTo(this);

         q *= quat::rotate(path.gradient(w), path.gradient(w+1.0/n));
      }

      lglPopMatrix();
      lglMatrixMode(LGL_MODELVIEW);
   }

};

//! planar geometry (base class)
class lglPlanar: public lglFunctional
{
public:

   lglPlanar(const std::string &name = "planar")
      : lglFunctional(name)
   {}

   void create(int tessel = 8, bool color = false)
   {
      clear();
      addTo(this, tessel, color);
   }

   void addTo(lglVBO *vbo, int tessel = 8, bool color = false)
   {
      int i, j;

      double u, v;
      vec3 p, n;

      int alpha_steps = tessel * tessel_u();
      int beta_steps = tessel * tessel_v();

      vec4 col = vbo->lglGetColor();

      for (j=0; j<beta_steps; j++)
      {
         vbo->lglBegin(LGL_QUAD_STRIP);

            if (color) vbo->lglColor(col);

            for (i=0; i<=alpha_steps; i++)
            {
               u = (double)i/alpha_steps;
               v = (double)(j+1)/beta_steps;

               p = point(u, v);
               n = normal(u, v);

               vbo->lglNormal(n);
               vbo->lglTexCoord((float)u,1.0f-(float)v);
               vbo->lglVertex(p);

               v = (double)j/beta_steps;

               p = point(u, v);
               n = normal(u, v);

               vbo->lglNormal(n);
               vbo->lglTexCoord((float)u,1.0f-(float)v);
               vbo->lglVertex(p);
            }

         lglEnd();
      }
   }

protected:

   virtual vec3 point(double u, double v)
   {
      return(evaluate(u, v));
   }

   virtual vec3 normal(double u, double v, double d = 0.001)
   {
      int i = tessel_u();
      int j = tessel_v();

      double maxdu = 1;
      if (i > 0) maxdu = 0.01/i;
      if (maxdu < d) d = maxdu;

      double maxdv = 1;
      if (j > 0) maxdv = 0.01/j;
      if (maxdv < d) d = maxdv;

      return(-lglFunctional::normal(u, v, d));
   }

   virtual vec3 evaluate(double u, double v) = 0;

   virtual int tessel_u() {return(1);}
   virtual int tessel_v() {return(1);}
};

//! planar bezier geometry
//!
//! the lglPlanarBezier class provides a pre-defined vbo, which contains planar geometry
//! * the shape of the geometry is determined by evaluating a 3D bezier mesh
//! * by default it contains normal and texture coordinate attributes but no color attribute
class lglPlanarBezier: public lglPlanar
{
public:

   lglPlanarBezier(const lgl_BezierMesh &mesh)
      : lglPlanar("planar_bezier"),
        mesh_(mesh)
   {}

   void release()
   {
      mesh_.clear();
   }

protected:

   lgl_BezierMesh mesh_;

   virtual vec3 evaluate(double u, double v)
   {
      return(mesh_.evaluate(u, v));
   }

   virtual int tessel_u() {return(mesh_.getCols()-1);}
   virtual int tessel_v() {return(mesh_.getRows()-1);}
};

//! unit wireframe cube vbo
//!
//! the lglWireCube class provides a pre-defined vbo, which contains a single cube with unit size
//! * it just consists of a wireframe model of the cube's edges
//! * the origin of the cube's coordinate system corresponds to its barycenter
//! * as it is unit sized, the extents range from -0.5 to 0.5
class lglWireCube: public lglVBO
{
public:

   lglWireCube()
      : lglVBO("wireframe_cube")
   {
      addLines(this);
   }

   static void addLines(lglVBO *vbo)
   {
      vbo->lglBegin(LGL_LINES);

         // vertical edges
         vbo->lglVertex(-0.5,-0.5,0.5);
         vbo->lglVertex(-0.5,0.5,0.5);
         vbo->lglVertex(0.5,-0.5,0.5);
         vbo->lglVertex(0.5,0.5,0.5);
         vbo->lglVertex(0.5,-0.5,-0.5);
         vbo->lglVertex(0.5,0.5,-0.5);
         vbo->lglVertex(-0.5,-0.5,-0.5);
         vbo->lglVertex(-0.5,0.5,-0.5);

         // bottom edges
         vbo->lglVertex(-0.5,-0.5,0.5);
         vbo->lglVertex(0.5,-0.5,0.5);
         vbo->lglVertex(0.5,-0.5,0.5);
         vbo->lglVertex(0.5,-0.5,-0.5);
         vbo->lglVertex(0.5,-0.5,-0.5);
         vbo->lglVertex(-0.5,-0.5,-0.5);
         vbo->lglVertex(-0.5,-0.5,-0.5);
         vbo->lglVertex(-0.5,-0.5,0.5);

         // top edges
         vbo->lglVertex(-0.5,0.5,0.5);
         vbo->lglVertex(0.5,0.5,0.5);
         vbo->lglVertex(0.5,0.5,0.5);
         vbo->lglVertex(0.5,0.5,-0.5);
         vbo->lglVertex(0.5,0.5,-0.5);
         vbo->lglVertex(-0.5,0.5,-0.5);
         vbo->lglVertex(-0.5,0.5,-0.5);
         vbo->lglVertex(-0.5,0.5,0.5);

      vbo->lglEnd();
   }

};

//! unit coordinate system axis vbo
//!
//! the lglCoordSys class provides a pre-defined vbo, which contains the unit sized axis of a coordinate system
//! * the x-axis is a red arrow
//! * the y-axis is a green arrow
//! * the z-axis is a blue arrow
//! * as it is unit sized, the length of the arrows is 1
class lglCoordSys: public lglVBO
{
public:

   lglCoordSys()
      : lglVBO("coordinate_system_axis")
   {
      addLines(this);
   }

   static void addLines(lglVBO *vbo)
   {
      static const double a = 0.1;
      static const double b = 0.3*a;

      vbo->lglBegin(LGL_LINES);

         // x-axis
         vbo->lglColor(1,0,0);
         vbo->lglVertex(0,0,0);
         vbo->lglVertex(1,0,0);
         vbo->lglVertex(1-a,-b,0);
         vbo->lglVertex(1,0,0);
         vbo->lglVertex(1-a,b,0);
         vbo->lglVertex(1,0,0);

         // y-axis
         vbo->lglColor(0,1,0);
         vbo->lglVertex(0,0,0);
         vbo->lglVertex(0,1,0);
         vbo->lglVertex(-b,1-a,0);
         vbo->lglVertex(0,1,0);
         vbo->lglVertex(b,1-a,0);
         vbo->lglVertex(0,1,0);

         // z-axis
         vbo->lglColor(0,0,1);
         vbo->lglVertex(0,0,0);
         vbo->lglVertex(0,0,1);
         vbo->lglVertex(0,-b,1-a);
         vbo->lglVertex(0,0,1);
         vbo->lglVertex(0,b,1-a);
         vbo->lglVertex(0,0,1);

      vbo->lglEnd();

      // X
      vbo->lglBegin(LGL_LINES);
      vbo->lglColor(1,0,0);
      vbo->lglEnd();
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPushMatrix();
      vbo->lglTranslate(0.7f,0.1f,0);
      vbo->lglMatrixMode(LGL_MODELVIEW);
      lglDrawString(0.25f, "X", false, vbo);
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPopMatrix();
      vbo->lglMatrixMode(LGL_MODELVIEW);

      // Y
      vbo->lglBegin(LGL_LINES);
      vbo->lglColor(0,1,0);
      vbo->lglEnd();
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPushMatrix();
      vbo->lglTranslate(0.1f,0.65f,0);
      vbo->lglMatrixMode(LGL_MODELVIEW);
      lglDrawString(0.25f, "Y", false, vbo);
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPopMatrix();
      vbo->lglMatrixMode(LGL_MODELVIEW);

      // Z
      vbo->lglBegin(LGL_LINES);
      vbo->lglColor(0,0,1);
      vbo->lglEnd();
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPushMatrix();
      vbo->lglTranslate(0,0.1f,0.9f);
      vbo->lglRotate(90, 0,1,0);
      vbo->lglMatrixMode(LGL_MODELVIEW);
      lglDrawString(0.25f, "Z", false, vbo);
      vbo->lglMatrixMode(LGL_PREMODEL);
      vbo->lglPopMatrix();
      vbo->lglMatrixMode(LGL_MODELVIEW);
   }

};

//! unit text vbo
//!
//! the lglText class provides a pre-defined vbo, which contains a unit sized text in vector format
//! * as it is unit sized, the width of the text is 1, the height depends on the actual text length
class lglText: public lglVBO
{
public:

   lglText(std::string text)
      : lglVBO("text")
   {
      addLines(this, text);
   }

   static void addLines(lglVBO *vbo, std::string text)
   {
      lglDrawString(1, text.c_str(), false, vbo);
   }

};

//! shared vbo container
class lglSharedVBO: public std::vector<mat4>
{
public:

   lglSharedVBO(lglVBO *vbo = NULL)
      : vbo_(vbo)
   {}

   //! add shared instance in immediate mode fashion
   void push_back()
   {
      lgl_matrixmode_enum mode = vbo_->lglGetMatrixMode();
      vbo_->lglMatrixMode(LGL_PREMODEL);
      push_back(vbo_->lglGetMatrix());
      vbo_->lglMatrixMode(mode);
   }

   //! add shared instance /w transformation matrix
   void push_back(const mat4 &m)
   {
      std::vector<mat4>::push_back(m);
   }

   //! set shared vbo
   void lglSetVBO(lglVBO *vbo)
   {
      vbo_ = vbo;
   }

   //! render shared instances
   void lglRender(lglVBO *vbo = NULL)
   {
      if (vbo_)
      {
         mat4 m = vbo_->lglGetModelMatrix();
         for (size_t i=0; i<size(); i++)
         {
            vbo_->lglModel(at(i));
            vbo_->lglRender(vbo);
         }
         vbo_->lglModel(m);
      }
   }

protected:

   lglVBO *vbo_;
};

#endif
