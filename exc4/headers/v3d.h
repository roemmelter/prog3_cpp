// (c) by Stefan Roettger, licensed under MIT license
// v3d: header-only 3D vector class

#ifndef V3D_H
#define V3D_H

#include <math.h>
#include <iostream>

// 3D double vector
//  definition of components via constructor v3d(x,y,z)
//  access to components x/y/z via . component selector
//  supplies vector operators + - * dot and cross product
//  supplies getters for length and normalization
class v3d
   {
   public:

   // default constructor
   v3d() {}

   // copy constructor
   v3d(const v3d &v) {x=v.x; y=v.y; z=v.z;}

   // component-wise constructor
   v3d(const double vx,const double vy,const double vz) {x=vx; y=vy; z=vz;}

   // destructor
   ~v3d() {}

   // get vector length
   double length() const {return(sqrt(x*x+y*y+z*z));}

   // get squared vector length
   double length2() const {return(x*x+y*y+z*z);}

   // normalization to unit length
   v3d normalize() const;

   // vector components
   double x,y,z;
   };

// addition of two vectors
inline v3d operator + (const v3d &a,const v3d &b)
   {return(v3d(a.x+b.x,a.y+b.y,a.z+b.z));}

// subtraction of two vectors
inline v3d operator - (const v3d &a,const v3d &b)
   {return(v3d(a.x-b.x,a.y-b.y,a.z-b.z));}

// negation of a vector
inline v3d operator - (const v3d &v)
   {return(v3d(-v.x,-v.y,-v.z));}

// left-hand side scalar multiplication
inline v3d operator * (const double a,const v3d &b)
   {return(v3d(a*b.x,a*b.y,a*b.z));}

// right-hand side scalar multiplication
inline v3d operator * (const v3d &a,const double b)
   {return(v3d(a.x*b,a.y*b,a.z*b));}

// right-hand side scalar division
inline v3d operator / (const v3d &a,const double b)
   {return(v3d(a.x/b,a.y/b,a.z/b));}

// dot product
inline double operator * (const v3d &a,const v3d &b)
   {return(a.x*b.x+a.y*b.y+a.z*b.z);}

// cross product (0,0,-1)/(-1,0,0)=(0,1,0)
inline v3d operator / (const v3d &a,const v3d &b)
   {return(v3d(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x));}

// comparison
inline int operator == (const v3d &a,const v3d &b)
   {return(a.x==b.x && a.y==b.y && a.z==b.z);}

// negated comparison
inline int operator != (const v3d &a,const v3d &b)
   {return(a.x!=b.x || a.y!=b.y || a.z!=b.z);}

// normalization to unit length
inline v3d v3d::normalize() const
   {
   double l2=length2();
   if (l2>0.0 && l2!=1.0) return(*this/sqrt(l2));
   return(*this);
   }

// output operator
inline std::ostream& operator << (std::ostream &out,const v3d &v)
   {return(out << "(" << v.x << "," << v.y << "," << v.z << ")");}

#endif
