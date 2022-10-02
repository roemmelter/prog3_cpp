// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL bezier classes
#ifndef GLVERTEX_BEZIER_H
#define GLVERTEX_BEZIER_H

#include "vector"

#include "glslmath.h"

//! 2D bezier curve
class lgl_BezierCurve2D
{
public:

   //! create 2D bezier curve from 2 control points
   lgl_BezierCurve2D(vec2 a, vec2 b,
                     bool lalign = true, bool ralign = true)
   {
      p1 = p2 = a;
      p3 = p4 = b;

      align_left = lalign;
      align_right = ralign;
   }

   //! create 2D bezier curve from 4 control points
   lgl_BezierCurve2D(vec2 a, vec2 b, vec2 c, vec2 d,
                     bool lalign = true, bool ralign = true)
   {
      p1 = a;
      p2 = b;
      p3 = c;
      p4 = d;

      align_left = lalign;
      align_right = ralign;
   }

   ~lgl_BezierCurve2D()
   {}

   //! start point
   vec2 start()
   {
      return(p1);
   }

   //! end point
   vec2 end()
   {
      return(p4);
   }

   //! calculate distance of start and end point
   double distance()
   {
      return((p4-p1).length());
   }

   //! evaluate the 2D bezier curve
   vec2 evaluate(double w) const
   {
      vec2 p12 = (1-w)*p1 + w*p2;
      vec2 p23 = (1-w)*p2 + w*p3;
      vec2 p34 = (1-w)*p3 + w*p4;
      vec2 p123 = (1-w)*p12 + w*p23;
      vec2 p234 = (1-w)*p23 + w*p34;
      vec2 p1234 = (1-w)*p123 + w*p234;

      return(p1234);
   }

   //! evaluate the gradient of the 2D bezier curve
   vec2 gradient(double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = evaluate(w-ld);
      vec2 p2 = evaluate(w+rd);

      return((p2-p1)/(ld+rd));
   }

   //! compute length of curve
   double length(int steps = 100) const
   {
      double l = 0;
      vec2 p = evaluate(0);

      for (int i=1; i<=steps; i++)
      {
         vec2 np = evaluate((double)i/steps);
         l += (np-p).length();
         p = np;
      }

      return(l);
   }

   //! auto-align the left control point of the 2D bezier curve
   void alignLeft(vec2 direction, lgl_BezierCurve2D *left = NULL, double factor = (sqrt(2.0)-1)*4/3)
   {
      p2 = p1 + factor*distance()*direction;

      if (left)
         left->p3 = left->p4 - factor*left->distance()*direction;
   }

   //! auto-align the right control point of the 2D bezier curve
   void alignRight(vec2 direction, lgl_BezierCurve2D *right = NULL, double factor = (sqrt(2.0)-1)*4/3)
   {
      p3 = p4 - factor*distance()*direction;

      if (right)
         right->p2 = right->p1 + factor*right->distance()*direction;
   }

   //! auto-align the control points of the 2D bezier curve
   void alignCurves(lgl_BezierCurve2D *left, lgl_BezierCurve2D *right, double factor = (sqrt(2.0)-1)*4/3)
   {
      if (!align_left)
         left = NULL;

      if (!align_right)
         right = NULL;

      if (left)
         if (left->p4 != p1)
            left->p4 = p1 = (left->p4+p1)/2;

      if (right)
         if (right->p1 != p4)
            right->p1 = p4 = (right->p1+p4)/2;

      double d = distance();

      double ld = d;
      if (left) ld = left->distance();

      double rd = d;
      if (right) rd = right->distance();

      double lf1 = 1;
      if (d > 0) lf1 *= ld/d;
      if (lf1 > 1) lf1 = 1;

      double rf1 = 1;
      if (d > 0) rf1 *= rd/d;
      if (rf1 > 1) rf1 = 1;

      double lf2 = 1;
      if (ld > 0) lf2 *= d/ld;
      if (lf2 > 1) lf2 = 1;

      double rf2 = 1;
      if (rd > 0) rf2 *= d/rd;
      if (rf2 > 1) rf2 = 1;

      if (left)
         if (left->p3 == left->p4)
            left->p3 = left->p4 - factor*lf2*(left->p4-left->p1);

      if (align_left)
         if (p2 == p1)
            p2 = p1 + factor*lf1*(p4-p1);

      if (align_right)
         if (p3 == p4)
            p3 = p4 - factor*rf1*(p4-p1);

      if (right)
         if (right->p2 == right->p1)
            right->p2 = right->p1 + factor*rf2*(right->p4-right->p1);

      if (left)
      {
         vec2 n = nearestPointOnLine(p1, left->p3, p2);
         left->p3 += p1-n;
         p2 += p1-n;
      }

      if (right)
      {
         vec2 n = nearestPointOnLine(p4, p3, right->p2);
         p3 += p4-n;
         right->p2 += p4-n;
      }
   }

   //! translate curve
   void translate(vec2 v)
   {
      p1 += v;
      p2 += v;
      p3 += v;
      p4 += v;
   }

   //! rotate curve
   void rotate(double angle)
   {
      mat2 m = mat2::rotate(angle);

      p1 = m * p1;
      p2 = m * p2;
      p3 = m * p3;
      p4 = m * p4;
   }

   //! scale curve
   void scale(vec2 s)
   {
      p1 *= s;
      p2 *= s;
      p3 *= s;
      p4 *= s;
   }

   vec2 p1, p2, p3, p4;
   bool align_left, align_right;

protected:

   vec2 nearestPointOnLine(vec2 p, vec2 a, vec2 b) const
   {
      vec2 d = (b-a).normalize();
      double l = (p-a).dot(d);
      vec2 n = a+l*d;
      return(n);
   }

};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierCurve2D &c)
   {return(out << "curve2D(" << c.p1 << ", " << c.p2 << ", " << c.p3 << ", " << c.p4 << ")");}

//! 3D bezier curve
class lgl_BezierCurve3D
{
public:

   //! create 3D bezier curve from 2 control points
   lgl_BezierCurve3D(vec3 a, vec3 b,
                     bool lalign = true, bool ralign = true)
   {
      p1 = p2 = a;
      p3 = p4 = b;

      align_left = lalign;
      align_right = ralign;
   }

   //! create 3D bezier curve from 4 control points
   lgl_BezierCurve3D(vec3 a, vec3 b, vec3 c, vec3 d,
                     bool lalign = true, bool ralign = true)
   {
      p1 = a;
      p2 = b;
      p3 = c;
      p4 = d;

      align_left = lalign;
      align_right = ralign;
   }

   ~lgl_BezierCurve3D()
   {}

   //! start point
   vec3 start()
   {
      return(p1);
   }

   //! end point
   vec3 end()
   {
      return(p4);
   }

   //! calculate distance of start and end point
   double distance()
   {
      return((p4-p1).length());
   }

   //! evaluate the 3D bezier curve
   vec3 evaluate(double w) const
   {
      vec3 p12 = (1-w)*p1 + w*p2;
      vec3 p23 = (1-w)*p2 + w*p3;
      vec3 p34 = (1-w)*p3 + w*p4;
      vec3 p123 = (1-w)*p12 + w*p23;
      vec3 p234 = (1-w)*p23 + w*p34;
      vec3 p1234 = (1-w)*p123 + w*p234;

      return(p1234);
   }

   //! evaluate the gradient of the 3D bezier curve
   vec3 gradient(double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec3 p1 = evaluate(w-ld);
      vec3 p2 = evaluate(w+rd);

      return((p2-p1)/(ld+rd));
   }

   //! compute length of curve
   double length(int steps = 100) const
   {
      double l = 0;
      vec3 p = evaluate(0);

      for (int i=1; i<=steps; i++)
      {
         vec3 np = evaluate((double)i/steps);
         l += (np-p).length();
         p = np;
      }

      return(l);
   }

   //! auto-align the left control point of the 3D bezier curve
   void alignLeft(vec3 direction, lgl_BezierCurve3D *left = NULL, double factor = (sqrt(2.0)-1)*4/3)
   {
      p2 = p1 + factor*distance()*direction;

      if (left)
         left->p3 = left->p4 - factor*left->distance()*direction;
   }

   //! auto-align the right control point of the 3D bezier curve
   void alignRight(vec3 direction, lgl_BezierCurve3D *right = NULL, double factor = (sqrt(2.0)-1)*4/3)
   {
      p3 = p4 - factor*distance()*direction;

      if (right)
         right->p2 = right->p1 + factor*right->distance()*direction;
   }

   //! auto-align the control points of the 3D bezier curve
   void alignCurves(lgl_BezierCurve3D *left, lgl_BezierCurve3D *right, double factor = (sqrt(2.0)-1)*4/3)
   {
      if (!align_left)
         left = NULL;

      if (!align_right)
         right = NULL;

      if (left)
         if (left->p4 != p1)
            left->p4 = p1 = (left->p4+p1)/2;

      if (right)
         if (right->p1 != p4)
            right->p1 = p4 = (right->p1+p4)/2;

      double d = distance();

      double ld = d;
      if (left) ld = left->distance();

      double rd = d;
      if (right) rd = right->distance();

      double lf1 = 1;
      if (d > 0) lf1 *= ld/d;
      if (lf1 > 1) lf1 = 1;

      double rf1 = 1;
      if (d > 0) rf1 *= rd/d;
      if (rf1 > 1) rf1 = 1;

      double lf2 = 1;
      if (ld > 0) lf2 *= d/ld;
      if (lf2 > 1) lf2 = 1;

      double rf2 = 1;
      if (rd > 0) rf2 *= d/rd;
      if (rf2 > 1) rf2 = 1;

      if (left)
         if (left->p3 == left->p4)
            left->p3 = left->p4 - factor*lf2*(left->p4-left->p1);

      if (align_left)
         if (p2 == p1)
            p2 = p1 + factor*lf1*(p4-p1);

      if (align_right)
         if (p3 == p4)
            p3 = p4 - factor*rf1*(p4-p1);

      if (right)
         if (right->p2 == right->p1)
            right->p2 = right->p1 + factor*rf2*(right->p4-right->p1);

      if (left)
      {
         vec3 n = nearestPointOnLine(p1, left->p3, p2);
         left->p3 += p1-n;
         p2 += p1-n;
      }

      if (right)
      {
         vec3 n = nearestPointOnLine(p4, p3, right->p2);
         p3 += p4-n;
         right->p2 += p4-n;
      }
   }

   //! translate curve
   void translate(vec3 v)
   {
      p1 += v;
      p2 += v;
      p3 += v;
      p4 += v;
   }

   //! rotate curve
   void rotate(quat q)
   {
      p1 = q * p1;
      p2 = q * p2;
      p3 = q * p3;
      p4 = q * p4;
   }

   //! scale curve
   void scale(vec3 s)
   {
      p1 *= s;
      p2 *= s;
      p3 *= s;
      p4 *= s;
   }

   //! rotate curve
   void rotate(double angle, vec3 axis)
   {
      rotate(quat::rotate(angle, axis));
   }

   vec3 p1, p2, p3, p4;
   bool align_left, align_right;

protected:

   vec3 nearestPointOnLine(vec3 p, vec3 a, vec3 b) const
   {
      vec3 d = (b-a).normalize();
      double l = (p-a).dot(d);
      vec3 n = a+l*d;
      return(n);
   }

};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierCurve3D &c)
   {return(out << "curve3D(" << c.p1 << ", " << c.p2 << ", " << c.p3 << ", " << c.p4 << ")");}

//! 2D bezier path consisting of multiple bezier curves
class lgl_BezierPath2D: public std::vector<lgl_BezierCurve2D>
{
public:

   lgl_BezierPath2D()
      : std::vector<lgl_BezierCurve2D>()
   {}

   lgl_BezierPath2D(vec2 a, vec2 b, vec2 c, vec2 d)
      : std::vector<lgl_BezierCurve2D>()
   {
      resize(1, lgl_BezierCurve2D(a,b,c,d));
   }

   lgl_BezierPath2D(vec2 a, vec2 b)
      : std::vector<lgl_BezierCurve2D>()
   {
      resize(1, lgl_BezierCurve2D(a,b));
   }

   lgl_BezierPath2D(const lgl_BezierCurve2D &curve)
      : std::vector<lgl_BezierCurve2D>()
   {
      resize(1, curve);
   }

   //! add a point to the 2D bezier path
   //! * returns the index of the added point
   unsigned int addPoint(vec2 p, bool align = true)
   {
      if (size() == 0)
      {
         push_back(lgl_BezierCurve2D(p, p, align, align));
         return(0);
      }
      else if (size() == 1)
      {
         vec2 p1 = at(0).p1;
         bool lalign = at(0).align_left;
         vec2 p2 = at(0).p4;

         if (p1 == p2)
            clear();
         else
            p1 = p2;

         push_back(lgl_BezierCurve2D(p1, p, lalign, align));
         return(1);
      }
      else
      {
         vec2 p1 = at(size()-1).p4;
         bool ralign = at(size()-1).align_right;
         vec2 p2 = p;

         if (p1 != p2)
            push_back(lgl_BezierCurve2D(p1, p2, ralign, align));
      }

      return(size());
   }

   //! disable alignment for the last added point
   void straighten()
   {
      int n = size();

      if (n > 0)
         at(n-1).align_right = false;
   }

   //! close the 2D bezier path by reduplicating the first point
   void close()
   {
      if (size() > 0)
         addPoint(at(0).p1, at(0).align_left);
   }

   //! is the 2D bezier path a closed path?
   bool closed() const
   {
      int n = size();

      if (n > 0)
         return(at(0).p1 == at(n-1).p4);
      else
         return(false);
   }

   //! start point
   vec2 start()
   {
      int n = size();

      if (n > 0)
         return(at(0).start());
      else
         return(vec2(0));
   }

   //! end point
   vec2 end()
   {
      int n = size();

      if (n > 0)
         return(at(n-1).end());
      else
         return(vec2(0));
   }

   //! evaluate the 2D bezier path
   vec2 evaluate(double w) const
   {
      if (closed())
      {
         if (w < 0) w += 1;
         else if (w > 1) w -= 1;
      }

      int n = size();

      if (n == 0) return(vec2(0));

      double s = w*n;
      int i = (int)floor(s);
      double t = s-i;

      if (i < 0)
      {
         t += i;
         i = 0;
      }

      if (i > n-1)
      {
         t += i-(n-1);
         i = n-1;
      }

      return(at(i).evaluate(t));
   }

   //! evaluate the gradient of the 2D bezier path
   vec2 gradient(double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = evaluate(w-ld);
      vec2 p2 = evaluate(w+rd);

      return((p2-p1)/(ld+rd));
   }

   //! compute the normal of the 2D bezier path
   vec2 normal(double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = evaluate(w-ld);
      vec2 p2 = evaluate(w+rd);

      vec2 df = p2-p1;
      vec2 n = vec2(-df.y, df.x);

      return(n.normalize());
   }

   //! compute length of path
   double length(int steps = 100) const
   {
      double l = 0;
      int n = size();

      for (int i=0; i<n; i++)
         l += at(i).length();

      return(l);
   }

   //! auto-align two consecutive control points of the 2D bezier path
   void align(unsigned int index, vec2 direction, double factor = (sqrt(2.0)-1)*4/3)
   {
      if (index <= size())
      {
         if (index < size())
            at(index).alignLeft(direction, index>0?&at(index-1):(closed()?&at(size()-1):NULL), factor);
         else if (index > 0)
            at(index-1).alignRight(direction, index<size()?&at(index):(closed()?&at(0):NULL), factor);
      }
   }

   //! auto-align the left-most control point of the 2D bezier path
   void alignLeft(vec2 direction, double factor = (sqrt(2.0)-1)*4/3)
   {
      align(0, direction, factor);
   }

   //! auto-align the right-most control point of the 2D bezier path
   void alignRight(vec2 direction, double factor = (sqrt(2.0)-1)*4/3)
   {
      align(size(), direction, factor);
   }

   //! auto-align the curves of the 2D bezier path
   void alignCurves(double factor = (sqrt(2.0)-1)*4/3)
   {
      int n = size();

      if (n > 0)
         for (int i=0; i<n; i++)
         {
            if (i>0 && i<n-1)
               at(i).alignCurves(&at(i-1), &at(i+1), factor);
            else if (i > 0)
               at(i).alignCurves(&at(i-1), closed()?&at(0):NULL, factor);
            else if (i < n-1)
               at(i).alignCurves(closed()?&at(n-1):NULL, &at(i+1), factor);
            else
               at(i).alignCurves(NULL, NULL, factor);
         }
   }

   //! translate path
   void translate(vec2 v)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).translate(v);
   }

   //! rotate path
   void rotate(double angle)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).rotate(angle);
   }

   //! scale path
   void scale(vec2 s)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).scale(s);
   }

   double first(int steps, bool *align = NULL)
   {
      int n = size();

      bool noalign = false;

      if (n>0 && !at(0).align_left) noalign = true;

      if (align)
         *align = !noalign;

      return(0);
   }

   double last(int steps)
   {
      int n = size();

      double u = 1+0.5/steps;

      if (n > 0)
      {
         double v = 1+0.5/n;
         if (v < u) u = v;
      }

      return(u);
   }

   double next(double w, int steps, bool *align = NULL, double minstep = 1E-12)
   {
      int n = size();

      double d = 1.0/steps;
      double v = w+d;

      double s = w*n;
      int a = (int)floor(s);

      double t = v*n;
      int b = (int)floor(t);

      for (int i=a; i<b; i++)
      {
         bool lalign = false;
         bool ralign = false;

         if (i<n && at(i).align_right) ralign = true;
         if (i+1<n && at(i+1).align_left) lalign = true;

         bool noalign = !lalign && !ralign;

         if (align)
            *align = !noalign;

         if (noalign || b>=n)
         {
            double u = (double)(i+1)/n;

            if (u > w + minstep)
               return(u);
         }
      }

      if (align)
         *align = true;

      return(v);
   }

};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierPath2D &p)
{
   out << "path2D(";

   for (unsigned int i=0; i<p.size(); i++)
   {
      if (i > 0) out << ", ";
      out << p[i];
   }

   out << ")";

   return(out);
}

//! 3D bezier path consisting of multiple bezier curves
class lgl_BezierPath3D: public std::vector<lgl_BezierCurve3D>
{
public:

   lgl_BezierPath3D()
      : std::vector<lgl_BezierCurve3D>()
   {}

   lgl_BezierPath3D(vec3 a, vec3 b, vec3 c, vec3 d)
      : std::vector<lgl_BezierCurve3D>()
   {
      resize(1, lgl_BezierCurve3D(a,b,c,d));
   }

   lgl_BezierPath3D(vec3 a, vec3 b)
      : std::vector<lgl_BezierCurve3D>()
   {
      resize(1, lgl_BezierCurve3D(a,b));
   }

   lgl_BezierPath3D(const lgl_BezierCurve3D &curve)
      : std::vector<lgl_BezierCurve3D>()
   {
      resize(1, curve);
   }

   //! add a point to the 3D bezier path
   //! * returns the index of the added point
   unsigned int addPoint(vec3 p, bool align = true)
   {
      if (size() == 0)
      {
         push_back(lgl_BezierCurve3D(p, p, align, align));
         return(0);
      }
      else if (size() == 1)
      {
         vec3 p1 = at(0).p1;
         bool lalign = at(0).align_left;
         vec3 p2 = at(0).p4;

         if (p1 == p2)
            clear();
         else
            p1 = p2;

         push_back(lgl_BezierCurve3D(p1, p, lalign, align));
         return(1);
      }
      else
      {
         vec3 p1 = at(size()-1).p4;
         bool ralign = at(size()-1).align_right;
         vec3 p2 = p;

         if (p1 != p2)
            push_back(lgl_BezierCurve3D(p1, p2, ralign, align));
      }

      return(size());
   }

   //! disable alignment for the last added point
   void straighten()
   {
      int n = size();

      if (n > 0)
         at(n-1).align_right = false;
   }

   //! close the 3D bezier path by reduplicating the first point
   void close()
   {
      if (size() > 0)
         addPoint(at(0).p1, at(0).align_left);
   }

   //! is the 3D bezier path a closed path?
   bool closed() const
   {
      int n = size();

      if (n > 0)
         return(at(0).p1 == at(n-1).p4);
      else
         return(false);
   }

   //! start point
   vec3 start()
   {
      int n = size();

      if (n > 0)
         return(at(0).start());
      else
         return(vec3(0));
   }

   //! end point
   vec3 end()
   {
      int n = size();

      if (n > 0)
         return(at(n-1).end());
      else
         return(vec3(0));
   }

   //! evaluate the 3D bezier path
   vec3 evaluate(double w) const
   {
      if (closed())
      {
         if (w < 0) w += 1;
         else if (w > 1) w -= 1;
      }

      int n = size();

      if (n == 0) return(vec3(0));

      double s = w*n;
      int i = (int)floor(s);
      double t = s-i;

      if (i < 0)
      {
         t += i;
         i = 0;
      }

      if (i > n-1)
      {
         t += i-(n-1);
         i = n-1;
      }

      return(at(i).evaluate(t));
   }

   //! evaluate the gradient of the 3D bezier path
   vec3 gradient(double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec3 p1 = evaluate(w-ld);
      vec3 p2 = evaluate(w+rd);

      return((p2-p1)/(ld+rd));
   }

   //! compute length of path
   double length(int steps = 100) const
   {
      double l = 0;
      int n = size();

      for (int i=0; i<n; i++)
         l += at(i).length();

      return(l);
   }

   //! auto-align two consecutive control points of the 3D bezier path
   void align(unsigned int index, vec2 direction, double factor = (sqrt(2.0)-1)*4/3)
   {
      if (index <= size())
      {
         if (index < size())
            at(index).alignLeft(direction, index>0?&at(index-1):NULL, factor);
         else if (index > 0)
            at(index-1).alignRight(direction, index<size()?&at(index):NULL, factor);
      }
   }

   //! auto-align the left-most control point of the 3D bezier path
   void alignLeft(vec3 direction, double factor = (sqrt(2.0)-1)*4/3)
   {
      if (size() > 0)
         at(0).alignLeft(direction, NULL, factor);
   }

   //! auto-align the right-most control point of the 3D bezier path
   void alignRight(vec3 direction, double factor = (sqrt(2.0)-1)*4/3)
   {
      if (size() > 0)
         at(size()-1).alignRight(direction, NULL, factor);
   }

   //! auto-align the curves of the 3D bezier path
   void alignCurves(double factor = (sqrt(2.0)-1)*4/3)
   {
      int n = size();

      if (n > 0)
         for (int i=0; i<n; i++)
         {
            if (i>0 && i<n-1)
               at(i).alignCurves(&at(i-1), &at(i+1), factor);
            else if (i > 0)
               at(i).alignCurves(&at(i-1), closed()?&at(0):NULL, factor);
            else if (i < n-1)
               at(i).alignCurves(closed()?&at(n-1):NULL, &at(i+1), factor);
            else
               at(i).alignCurves(NULL, NULL, factor);
         }
   }

   //! translate path
   void translate(vec3 v)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).translate(v);
   }

   //! rotate path
   void rotate(quat q)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).rotate(q);
   }

   //! scale path
   void scale(vec3 s)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).scale(s);
   }

   //! rotate path
   void rotate(double angle, vec3 axis)
   {
      rotate(quat::rotate(angle, axis));
   }

   double first(int steps, bool *align = NULL)
   {
      int n = size();

      bool noalign = false;

      if (n>0 && !at(0).align_left) noalign = true;

      if (align)
         *align = !noalign;

      return(0);
   }

   double last(int steps)
   {
      int n = size();

      double u = 1+0.5/steps;

      if (n > 0)
      {
         double v = 1+0.5/n;
         if (v < u) u = v;
      }

      return(u);
   }

   double next(double w, int steps, bool *align = NULL, double minstep = 1E-12)
   {
      int n = size();

      double d = 1.0/steps;
      double v = w+d;

      double s = w*n;
      int a = (int)floor(s);

      double t = v*n;
      int b = (int)floor(t);

      for (int i=a; i<b; i++)
      {
         bool lalign = false;
         bool ralign = false;

         if (i<n && at(i).align_right) ralign = true;
         if (i+1<n && at(i+1).align_left) lalign = true;

         bool noalign = !lalign && !ralign;

         if (align)
            *align = !noalign;

         if (noalign || b>=n)
         {
            double u = (double)(i+1)/n;

            if (u > w + minstep)
               return(u);
         }
      }

      if (align)
         *align = true;

      return(v);
   }

};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierPath3D &p)
{
   out << "path3D(";

   for (unsigned int i=0; i<p.size(); i++)
   {
      if (i > 0) out << ", ";
      out << p[i];
   }

   out << ")";

   return(out);
}

//! container for linear interpolation of 2D bezier paths
class lgl_BezierMultiPath2D: public std::vector<lgl_BezierPath2D>
{
public:

   lgl_BezierMultiPath2D()
      : std::vector<lgl_BezierPath2D>()
   {}

   lgl_BezierMultiPath2D(vec2 a, vec2 b, vec2 c, vec2 d)
      : std::vector<lgl_BezierPath2D>()
   {
      resize(1, lgl_BezierPath2D(a,b,c,d));
   }

   lgl_BezierMultiPath2D(vec2 a, vec2 b)
      : std::vector<lgl_BezierPath2D>()
   {
      resize(1, lgl_BezierPath2D(a,b));
   }

   lgl_BezierMultiPath2D(const lgl_BezierPath2D &path)
      : std::vector<lgl_BezierPath2D>()
   {
      resize(1, path);
   }

   //! get the maximum number of curves in the multi-path
   unsigned int getMaxCurves() const
   {
      unsigned int num = 0;

      int n = size();

      for (int i=0; i<n; i++)
         if (at(i).size() > num)
            num = at(i).size();

      return(num);
   }

   //! evaluate the 2D bezier multi-path
   vec2 evaluate(double v, double w) const
   {
      int n = size();

      if (n == 0) return(vec2(0));

      if (n == 1)
         return(at(0).evaluate(w));
      else
      {
         double p = v*n;
         int p1 = (int)floor(p);
         double t = p-p1;
         p1 = p1 % n;
         if (p1 < 0) p1 += n;
         int p2 = p1+1;
         p2 = p2 % n;
         if (p2 < 0) p2 += n;

         vec2 v1 = at(p1).evaluate(w);
         vec2 v2 = at(p2).evaluate(w);

         t = 0.5*sin((t-0.5)*PI)+0.5;

         return((1-t)*v1+t*v2);
      }
   }

   //! evaluate the gradient of the 2D bezier multi-path
   vec2 gradient(double v, double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = evaluate(v, w-ld);
      vec2 p2 = evaluate(v, w+rd);

      return((p2-p1)/(ld+rd));
   }

   //! compute the normal of the 2D bezier multi-path
   vec2 normal(double v, double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec2 p1 = evaluate(v, w-ld);
      vec2 p2 = evaluate(v, w+rd);

      vec2 df = p2-p1;
      vec2 n = vec2(-df.y, df.x);

      return(n.normalize());
   }

   //! translate multi-path
   void translate(vec2 v)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).translate(v);
   }

   //! rotate multi-path
   void rotate(double angle)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).rotate(angle);
   }

   //! scale multi-path
   void scale(vec2 s)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).scale(s);
   }

};

//! container for linear interpolation of 3D bezier paths
class lgl_BezierMultiPath3D: public std::vector<lgl_BezierPath3D>
{
public:

   lgl_BezierMultiPath3D()
      : std::vector<lgl_BezierPath3D>()
   {}

   lgl_BezierMultiPath3D(vec3 a, vec3 b, vec3 c, vec3 d)
      : std::vector<lgl_BezierPath3D>()
   {
      resize(1, lgl_BezierPath3D(a,b,c,d));
   }

   lgl_BezierMultiPath3D(vec3 a, vec3 b)
      : std::vector<lgl_BezierPath3D>()
   {
      resize(1, lgl_BezierPath3D(a,b));
   }

   lgl_BezierMultiPath3D(const lgl_BezierPath3D &path)
      : std::vector<lgl_BezierPath3D>()
   {
      resize(1, path);
   }

   //! get the maximum number of curves in the multi-path
   unsigned int getMaxCurves() const
   {
      unsigned int num = 0;

      int n = size();

      for (int i=0; i<n; i++)
         if (at(i).size() > num)
            num = at(i).size();

      return(num);
   }

   //! evaluate the 3D bezier multi-path
   vec3 evaluate(double v, double w) const
   {
      int n = size();

      if (n == 0) return(vec3(0));

      if (n == 1)
         return(at(0).evaluate(w));
      else
      {
         double p = v*n;
         int p1 = (int)floor(p);
         double t = p-p1;
         p1 = p1 % n;
         if (p1 < 0) p1 += n;
         int p2 = p1+1;
         p2 = p2 % n;
         if (p2 < 0) p2 += n;

         vec3 v1 = at(p1).evaluate(w);
         vec3 v2 = at(p2).evaluate(w);

         t = 0.5*sin((t-0.5)*PI)+0.5;

         return((1-t)*v1+t*v2);
      }
   }

   //! evaluate the gradient of the 3D bezier multi-path
   vec3 gradient(double v, double w, int mode = 0, double d = 0.001) const
   {
      double ld = d;
      double rd = d;

      if (mode < 0) rd = 0;
      if (mode > 0) ld = 0;

      vec3 p1 = evaluate(v, w-ld);
      vec3 p2 = evaluate(v, w+rd);

      return((p2-p1)/(ld+rd));
   }

   //! translate multi-path
   void translate(vec3 v)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).translate(v);
   }

   //! rotate multi-path
   void rotate(quat q)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).rotate(q);
   }

   //! scale multi-path
   void scale(vec3 s)
   {
      int n = size();

      for (int i=0; i<n; i++)
         at(i).scale(s);
   }

   //! rotate multi-path
   void rotate(double angle, vec3 axis)
   {
      rotate(quat::rotate(angle, axis));
   }

};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierMultiPath3D &m)
{
   out << "multipath3D(";

   for (unsigned int i=0; i<m.size(); i++)
   {
      if (i > 0) out << ", ";
      out << m[i];
   }

   out << ")";

   return(out);
}

//! bezier surface patch
class lgl_BezierPatch
{
public:

   lgl_BezierPatch()
   {
      p11 = p12 = p21 = p22 =
      p13 = p14 = p23 = p24 =
      p31 = p32 = p41 = p42 =
      p33 = p34 = p43 = p44 = vec3(0,0,0);
   }

   lgl_BezierPatch(vec3 a, vec3 b,
                   vec3 c, vec3 d)
   {
      p11 = p12 = p21 = p22 = a;
      p13 = p14 = p23 = p24 = b;
      p31 = p32 = p41 = p42 = c;
      p33 = p34 = p43 = p44 = d;
   }

   lgl_BezierPatch(vec3 a1, vec3 b1, vec3 c1, vec3 d1,
                   vec3 a2, vec3 b2, vec3 c2, vec3 d2,
                   vec3 a3, vec3 b3, vec3 c3, vec3 d3,
                   vec3 a4, vec3 b4, vec3 c4, vec3 d4)
   {
      p11 = a1; p12 = b1; p13 = c1; p14 = d1;
      p21 = a2; p22 = b2; p23 = c2; p24 = d2;
      p31 = a3; p32 = b3; p33 = c3; p34 = d3;
      p41 = a4; p42 = b4; p43 = c4; p44 = d4;
   }

   ~lgl_BezierPatch()
   {}

   //! evaluate the surface patch
   vec3 evaluate(double u, double v) const
   {
      vec3 p1 = lgl_BezierCurve3D(p11, p12, p13, p14).evaluate(u);
      vec3 p2 = lgl_BezierCurve3D(p21, p22, p23, p24).evaluate(u);
      vec3 p3 = lgl_BezierCurve3D(p31, p32, p33, p34).evaluate(u);
      vec3 p4 = lgl_BezierCurve3D(p41, p42, p43, p44).evaluate(u);

      return(lgl_BezierCurve3D(p4, p3, p2, p1).evaluate(v));
   }

   //! evaluate the gradient of the surface patch
   vec3 gradient(double u, double v, double d = 0.001) const
   {
      vec3 p1 = evaluate(u-d, v);
      vec3 p2 = evaluate(u+d, v);
      vec3 p3 = evaluate(u, v-d);
      vec3 p4 = evaluate(u, v+d);

      return(0.5*(p2-p1+p4-p3)/d);
   }

   //! compute the normal of the surface patch
   vec3 normal(double u, double v, double d = 0.001) const
   {
      vec3 p1 = evaluate(u-d, v);
      vec3 p2 = evaluate(u+d, v);
      vec3 p3 = evaluate(u, v-d);
      vec3 p4 = evaluate(u, v+d);

      vec3 p5 = evaluate(u-d, v-d);
      vec3 p6 = evaluate(u+d, v+d);
      vec3 p7 = evaluate(u-d, v+d);
      vec3 p8 = evaluate(u+d, v-d);

      vec3 du = p2-p1+p6-p5+p8-p7;
      vec3 dv = p4-p3+p7-p8+p6-p5;

      vec3 n = dv.cross(du);

      return(n.normalize());
   }

   //! auto-align the control points of the surface patch
   void alignPatches(lgl_BezierPatch *left, lgl_BezierPatch *right,
                     lgl_BezierPatch *bottom, lgl_BezierPatch *top,
                     lgl_BezierPatch *leftbottom, lgl_BezierPatch *rightbottom,
                     lgl_BezierPatch *lefttop, lgl_BezierPatch *righttop,
                     double factor = (sqrt(2.0)-1)*4/3)
   {
      align(leftbottom, bottom, left, this, factor);
      align(bottom, rightbottom, this, right, factor);
      align(left, this, lefttop, top, factor);
      align(this, right, top, righttop, factor);
   }

   //! translate patch
   void translate(vec3 v)
   {
      p11 += v;
      p12 += v;
      p13 += v;
      p14 += v;

      p21 += v;
      p22 += v;
      p23 += v;
      p24 += v;

      p31 += v;
      p32 += v;
      p33 += v;
      p34 += v;

      p41 += v;
      p42 += v;
      p43 += v;
      p44 += v;
   }

   //! rotate patch
   void rotate(quat q)
   {
      p11 = q * p11;
      p12 = q * p12;
      p13 = q * p13;
      p14 = q * p14;

      p21 = q * p21;
      p22 = q * p22;
      p23 = q * p23;
      p24 = q * p24;

      p31 = q * p31;
      p32 = q * p32;
      p33 = q * p33;
      p34 = q * p34;

      p41 = q * p41;
      p42 = q * p42;
      p43 = q * p43;
      p44 = q * p44;
   }

   //! scale patch
   void scale(vec3 s)
   {
      p11 *= s;
      p12 *= s;
      p13 *= s;
      p14 *= s;

      p21 *= s;
      p22 *= s;
      p23 *= s;
      p24 *= s;

      p31 *= s;
      p32 *= s;
      p33 *= s;
      p34 *= s;

      p41 *= s;
      p42 *= s;
      p43 *= s;
      p44 *= s;
   }

   //! rotate patch
   void rotate(double angle, vec3 axis)
   {
      rotate(quat::rotate(angle, axis));
   }

   vec3 p11, p12, p13, p14;
   vec3 p21, p22, p23, p24;
   vec3 p31, p32, p33, p34;
   vec3 p41, p42, p43, p44;

protected:

   vec3 nearestPointOnPlane(vec3 p, vec3 o, vec3 n) const
   {
      double l = (p-o).dot(n);
      vec3 v = p-l*n;
      return(v);
   }

   vec3 projectPointOnPlane(vec3 p1, vec3 p2, vec3 o, vec3 n) const
   {
      vec3 d = (p1-o + p2-o).normalize();
      if (d == vec3(0)) return(nearestPointOnPlane(p1, o, n));
      double l = (p1-o).dot(n) / d.dot(n);
      vec3 v = p1-l*d;
      return(v);
   }

   void align(lgl_BezierPatch *leftbottom, lgl_BezierPatch *rightbottom,
              lgl_BezierPatch *lefttop, lgl_BezierPatch *righttop,
              double factor = (sqrt(2.0)-1)*4/3)
   {
      // average center point:

      vec3 center(0,0,0);
      int count = 0;

      if (leftbottom)
      {
         center += leftbottom->p14;
         count++;
      }

      if (rightbottom)
      {
         center += rightbottom->p11;
         count++;
      }

      if (lefttop)
      {
         center += lefttop->p44;
         count++;
      }

      if (righttop)
      {
         center += righttop->p41;
         count++;
      }

      center = center / count;

      // update center point:

      if (leftbottom)
         leftbottom->p14 = center;

      if (rightbottom)
         rightbottom->p11 = center;

      if (lefttop)
         lefttop->p44 = center;

      if (righttop)
         righttop->p41 = center;

      // average left corner point:

      vec3 left(0,0,0);
      int leftcount = 0;

      if (leftbottom)
      {
         left += leftbottom->p11;
         leftcount++;
      }

      if (lefttop)
      {
         left += lefttop->p41;
         leftcount++;
      }

      left = left / leftcount;

      // average right corner point:

      vec3 right(0,0,0);
      int rightcount = 0;

      if (rightbottom)
      {
         right += rightbottom->p14;
         rightcount++;
      }

      if (righttop)
      {
         right += righttop->p44;
         rightcount++;
      }

      right = right / rightcount;

      // average bottom corner point:

      vec3 bottom(0,0,0);
      int bottomcount = 0;

      if (leftbottom)
      {
         bottom += leftbottom->p44;
         bottomcount++;
      }

      if (rightbottom)
      {
         bottom += rightbottom->p41;
         bottomcount++;
      }

      bottom = bottom / bottomcount;

      // average top corner point:

      vec3 top(0,0,0);
      int topcount = 0;

      if (lefttop)
      {
         top += lefttop->p14;
         topcount++;
      }

      if (righttop)
      {
         top += righttop->p11;
         topcount++;
      }

      top = top / topcount;

      // update left control point:

      if (leftbottom)
         if (leftbottom->p13 == leftbottom->p14)
            leftbottom->p13 = center + factor*(left-center);

      if (lefttop)
         if (lefttop->p43 == lefttop->p44)
            lefttop->p43 = center + factor*(left-center);

      // update right control point:

      if (rightbottom)
         if (rightbottom->p12 == rightbottom->p11)
            rightbottom->p12 = center + factor*(right-center);

      if (righttop)
         if (righttop->p42 == righttop->p41)
            righttop->p42 = center + factor*(right-center);

      // update bottom control point:

      if (leftbottom)
         if (leftbottom->p24 == leftbottom->p14)
            leftbottom->p24 = center + factor*(bottom-center);

      if (rightbottom)
         if (rightbottom->p21 == rightbottom->p11)
            rightbottom->p21 = center + factor*(bottom-center);

      // update top control point:

      if (lefttop)
         if (lefttop->p34 == lefttop->p44)
            lefttop->p34 = center + factor*(top-center);

      if (righttop)
         if (righttop->p31 == righttop->p41)
            righttop->p31 = center + factor*(top-center);

      // average left control point:

      left = vec3(0,0,0);
      leftcount = 0;

      if (leftbottom)
      {
         left += leftbottom->p13;
         leftcount++;
      }

      if (lefttop)
      {
         left += lefttop->p43;
         leftcount++;
      }

      left = left / leftcount;

      // average right control point:

      right = vec3(0,0,0);
      rightcount = 0;

      if (rightbottom)
      {
         right += rightbottom->p12;
         rightcount++;
      }

      if (righttop)
      {
         right += righttop->p42;
         rightcount++;
      }

      right = right / rightcount;

      // average bottom control point:

      bottom = vec3(0,0,0);
      bottomcount = 0;

      if (leftbottom)
      {
         bottom += leftbottom->p24;
         bottomcount++;
      }

      if (rightbottom)
      {
         bottom += rightbottom->p21;
         bottomcount++;
      }

      bottom = bottom / bottomcount;

      // average top control point:

      top = vec3(0,0,0);
      topcount = 0;

      if (lefttop)
      {
         top += lefttop->p34;
         topcount++;
      }

      if (righttop)
      {
         top += righttop->p31;
         topcount++;
      }

      top = top / topcount;

      // determine plane normal from tangent space:

      vec3 du = vec3(0,0,0);
      if (leftcount>0 && rightcount>0) du = right - left;
      else if (leftcount > 0) du = center - left;
      else du = right - center;
      du = du.normalize();

      vec3 dv = vec3(0,0,0);
      if (bottomcount>0 && topcount>0) dv = top - bottom;
      else if (bottomcount > 0) dv = center - bottom;
      else dv = top - center;
      dv = dv.normalize();

      vec3 n = du.cross(dv).normalize();

      // align control points:

      if (leftcount>0 && rightcount>0) left = projectPointOnPlane(left, right, center, n);
      else if (leftcount > 0) left = nearestPointOnPlane(left, center, n);

      if (rightcount>0 && leftcount>0) right = projectPointOnPlane(right, left, center, n);
      else if (rightcount > 0) right = nearestPointOnPlane(right, center, n);

      if (bottomcount>0 && topcount>0) bottom = projectPointOnPlane(bottom, top, center, n);
      else if (bottomcount > 0) bottom = nearestPointOnPlane(bottom, center, n);

      if (topcount>0 && bottomcount>0) top = projectPointOnPlane(top, bottom, center, n);
      else if (topcount > 0) top = nearestPointOnPlane(top, center, n);

      // update control points:

      if (leftbottom)
         leftbottom->p13 = left;

      if (lefttop)
         lefttop->p43 = left;

      if (rightbottom)
         rightbottom->p12 = right;

      if (righttop)
         righttop->p42 = right;

      if (leftbottom)
         leftbottom->p24 = bottom;

      if (rightbottom)
         rightbottom->p21 = bottom;

      if (lefttop)
         lefttop->p34 = top;

      if (righttop)
         righttop->p31 = top;

      // update diagonal control points:

      if (leftbottom)
         if (leftbottom->p23 == leftbottom->p14)
            leftbottom->p23 = center + left-center + bottom-center;

      if (rightbottom)
         if (rightbottom->p22 == rightbottom->p11)
            rightbottom->p22 = center + right-center + bottom-center;

      if (lefttop)
         if (lefttop->p33 == lefttop->p44)
            lefttop->p33 = center + left-center + top-center;

      if (righttop)
         if (righttop->p32 == righttop->p41)
            righttop->p32 = center + right-center + top-center;

      // align diagonal control points:

      if (leftbottom)
         leftbottom->p23 = nearestPointOnPlane(leftbottom->p23, center, n);

      if (rightbottom)
         rightbottom->p22 = nearestPointOnPlane(rightbottom->p22, center, n);

      if (lefttop)
         lefttop->p33 = nearestPointOnPlane(lefttop->p33, center, n);

      if (righttop)
         righttop->p32 = nearestPointOnPlane(righttop->p32, center, n);
   }

};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierPatch &p)
{
   out << "patch(";

   out << p.p11 << ", " << p.p12 << ", " << p.p13 << ", " << p.p14 << ", ";
   out << p.p21 << ", " << p.p22 << ", " << p.p23 << ", " << p.p24 << ", ";
   out << p.p31 << ", " << p.p32 << ", " << p.p33 << ", " << p.p34 << ", ";
   out << p.p41 << ", " << p.p42 << ", " << p.p43 << ", " << p.p44;

   out << ")";

   return(out);
}

//! bezier mesh consisting of multiple bezier surface patches
class lgl_BezierMesh
{
public:

   lgl_BezierMesh(int n, int m)
      : modified(false),
        cols(n), rows(m)
   {
      if (cols>=2 && rows>=2)
      {
         point.resize(cols*rows, vec3(0,0,0));
         patch.resize((cols-1)*(rows-1));
      }
   }

   int getCols()
   {
      return(cols);
   }

   int getRows()
   {
      return(rows);
   }

   void set(int i, int j, vec3 p)
   {
      if (cols>=2 && rows>=2)
      {
         point[i+j*cols] = p;
         modified = true;
      }
   }

   //! auto-align the bezier surface patches
   void align()
   {
      if (cols>=2 && rows>=2)
      {
         for (int i=0; i<cols-1; i++)
            for (int j=0; j<rows-1; j++)
               patch[i+j*(cols-1)] = lgl_BezierPatch(p(i, j+1), p(i+1, j+1), p(i, j), p(i+1, j));

         for (int i=0; i<cols-1; i++)
            for (int j=0; j<rows-1; j++)
               P(i, j)->alignPatches(P(i-1, j), P(i+1, j), P(i, j-1), P(i, j+1),
                                     P(i-1, j-1), P(i+1, j-1), P(i-1, j+1), P(i+1, j+1));
      }
   }

   //! evaluate the bezier mesh
   vec3 evaluate(double u, double v)
   {
      if (cols<2 || rows<2)
         return(vec3(0,0,0));
      else
      {
         if (modified)
         {
            align();
            modified = false;
         }

         u *= cols-1;
         v *= rows-1;

         int i = (int)floor(u);
         int j = (int)floor(v);

         double s = u-i;
         double t = v-j;

         if (i < 0)
         {
            s += i;
            i = 0;
         }

         if (i > cols-2)
         {
            s += i-(cols-2);
            i = cols-2;
         }

         if (j < 0)
         {
            t += j;
            j = 0;
         }

         if (j > rows-2)
         {
            t += j-(rows-2);
            j = rows-2;
         }

         return(P(i, j)->evaluate(s, t));
      }
   }

   //! evaluate the gradient of the bezier mesh
   vec3 gradient(double u, double v, double d = 0.001)
   {
      vec3 p1 = evaluate(u-d, v);
      vec3 p2 = evaluate(u+d, v);
      vec3 p3 = evaluate(u, v-d);
      vec3 p4 = evaluate(u, v+d);

      return(0.5*(p2-p1+p4-p3)/d);
   }

   //! compute the normal of the bezier mesh
   vec3 normal(double u, double v, double d = 0.001)
   {
      vec3 p1 = evaluate(u-d, v);
      vec3 p2 = evaluate(u+d, v);
      vec3 p3 = evaluate(u, v-d);
      vec3 p4 = evaluate(u, v+d);

      vec3 p5 = evaluate(u-d, v-d);
      vec3 p6 = evaluate(u+d, v+d);
      vec3 p7 = evaluate(u-d, v+d);
      vec3 p8 = evaluate(u+d, v-d);

      vec3 du = p2-p1+p6-p5+p8-p7;
      vec3 dv = p4-p3+p7-p8+p6-p5;

      vec3 n = dv.cross(du);

      return(n.normalize());
   }

   //! translate mesh
   void translate(vec3 v)
   {
      int n = point.size();

      for (int i=0; i<n; i++)
         point[i] += v;

      modified = true;
   }

   //! rotate mesh
   void rotate(quat q)
   {
      int n = point.size();

      for (int i=0; i<n; i++)
         point[i] = q * point[i];

      modified = true;
   }

   //! scale mesh
   void scale(vec3 s)
   {
      int n = point.size();

      for (int i=0; i<n; i++)
         point[i] *= s;

      modified = true;
   }

   //! rotate mesh
   void rotate(double angle, vec3 axis)
   {
      rotate(quat::rotate(angle, axis));
   }

   void clear()
   {
      point.clear();
      patch.clear();
      cols = rows = 0;
   }

protected:

   bool modified;
   int cols, rows;
   std::vector<vec3> point;
   std::vector<lgl_BezierPatch> patch;

   vec3 p(int i, int j)
   {
      return(point[i+j*cols]);
   }

   lgl_BezierPatch *P(int i, int j)
   {
      if (i<0 || i>cols-2 || j<0 || j>rows-2)
         return(NULL);
      else
         return(&patch[i+j*(cols-1)]);
   }

   friend inline std::ostream& operator << (std::ostream &out, const lgl_BezierMesh &m);
};

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_BezierMesh &m)
{
   out << "mesh(";

   for (unsigned int i=0; i<m.point.size(); i++)
   {
      if (i > 0) out << ", ";
      out << m.point[i];
   }

   out << ")";

   return(out);
}

#endif
