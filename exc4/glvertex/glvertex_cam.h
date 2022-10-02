// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL camera class
#ifndef GLVERTEX_CAM_H
#define GLVERTEX_CAM_H

#include "glvertex.h"

//! camera convenience class
class lgl_Cam
{
public:

   //! ctor
   lgl_Cam()
      : perspective_(false),
        left_(-1), right_(1), bottom_(-1), top_(1), front_(-1), back_(1),
        fovy_(0), aspect_(0), nearp_(0), farp_(0),
        eye_(vec3(0,0,0)), lookat_(vec3(0,0,-1)), up_(vec3(0,1,0)), alt_up_(vec3(0,0,-1)),
        base_(0), lefteye_(true),
        halted_(false)
   {}

   ~lgl_Cam()
   {}

   //! set camera type to orthographic
   void setOrthographic(double left, double right, double bottom, double top, double front, double back)
   {
      perspective_ = false;

      left_ = left;
      right_ = right;
      bottom_ = bottom;
      top_ = top;
      front_ = front;
      back_ = back;
   }

   //! set camera type to perspective
   void setPerspective(double fovy, double aspect, double nearp, double farp)
   {
      perspective_ = true;

      fovy_ = fovy;
      aspect_ = aspect;
      nearp_ = nearp;
      farp_ = farp;
   }

   //! set camera type to perspective
   void setPerspective(double fovy, double nearp, double farp)
   {
      setPerspective(fovy, 0, nearp, farp);
   }

   //! set camera aspect
   void setAspect(double aspect)
   {
      aspect_ = aspect;
   }

   //! set eye and lookat point and up vector
   void setEye(vec3 eye, vec3 lookat, vec3 up)
   {
      if (halted_) return;

      up_ = up.normalize();
      setEye(eye, lookat);
   }

   //! set eye and lookat point
   void setEye(vec3 eye, vec3 lookat)
   {
      if (halted_) return;

      eye_ = eye;
      lookat_ = lookat;

      vec3 dir = getDirection();
      double lambda = dir.dot(up_);

      if (fabs(lambda) < 1-0.001)
         alt_up_ = (up_-lambda*dir).normalize();
   }

   //! get eye point
   vec3 getEye(bool left = true) const
   {
      vec3 right = getRight();
      if (left) right = -right;
      if (!lefteye_) right = -right;

      return(eye_+0.5*base_*right);
   }

   //! get manipulated eye point
   vec3 getEyeManip(bool left = true) const
   {
      vec3 right = getRight();
      if (left) right = -right;
      if (!lefteye_) right = -right;

      vec4 eye = eye_+0.5*base_*right;

      mat4 M = lglGetManip();
      mat4 V = getViewMatrix();

      eye = (M*V).invert() * V * eye;

      return(eye);
   }

   //! get lookat point
   vec3 getLookAt() const
   {
      return(lookat_);
   }

   //! get viewing direction
   vec3 getDirection() const
   {
      return((lookat_-eye_).normalize());
   }

   //! get right vector
   vec3 getRight() const
   {
      return(getDirection().cross(getUp()).normalize());
   }

   //! get up vector
   vec3 getUp() const
   {
      if (fabs(getDirection().dot(up_)) < 1-0.001)
         return(up_);
      else
         return(alt_up_);
   }

   //! unproject viewport coordinate into view coordinate direction
   vec3 unprojectViewport(double x, double y)
   {
      if (perspective_)
      {
         double mx = x-0.5;
         double my = 0.5-y;

         double wy = tan(fovy_*PI/360);
         double wx = aspect_*wy;

         vec3 dir = vec3(2.0*wx*mx, 2.0*wy*my, -1);

         return(dir.normalize());
      }

      return(vec3(0,0,-1));
   }

   //! unproject viewport coordinate into world coordinate direction
   vec3 unprojectViewportToWorld(double x, double y)
   {
      if (perspective_)
      {
         double mx = x-0.5;
         double my = 0.5-y;

         double wy = tan(fovy_*PI/360);
         double wx = aspect_*wy;

         vec3 dir = getDirection()+
                    getRight()*2.0*wx*mx+
                    getUp()*2.0*wy*my;

         return(dir.normalize());
      }

      return(getDirection());
   }

   //! bounding sphere visibility test
   bool isVisible(const vec3 &center, double radius)
   {
      return(glslmath::vtest_plane_sphere(getEye(), getDirection(), center, radius));
   }

   //! bounding box visibility test
   bool isVisible(const vec3 &center, const vec3 &extent)
   {
      return(glslmath::vtest_plane_bbox(getEye(), getDirection(), center, extent));
   }

   //! set stereo base
   void setStereoBase(double base)
   {
      base_ = base;
   }

   //! get stereo base
   double getStereoBase() const
   {
      return(base_);
   }

   //! is stereo cam?
   bool isStereoCam() const
   {
      return(base_ != 0);
   }

   //! use left eye for stereo rendering
   void useLeftEye(bool left = true)
   {
      lefteye_ = left;
   }

   //! use right eye for stereo rendering
   void useRightEye(bool right = true)
   {
      lefteye_ = !right;
   }

   //! is left eye?
   bool isLeftEye() const
   {
      return(lefteye_);
   }

   //! is right eye?
   bool isRightEye() const
   {
      return(!lefteye_);
   }

   //! get projection matrix
   mat4 getProjectionMatrix() const
   {
      if (perspective_)
         return(mat4::perspective(fovy_, aspect_, nearp_, farp_));
      else
         return(mat4::ortho(left_, right_, bottom_, top_, front_, back_));
   }

   //! get viewing matrix
   mat4 getViewMatrix(bool left = true) const
   {
      return(mat4::lookat(getEye(left), getLookAt(), getUp()));
   }

   //! get inverse viewing matrix
   mat4 getInverseViewMatrix(bool left = true) const
   {
      return(getViewMatrix(left).invert());
   }

   //! get inverse transpose viewing matrix
   mat4 getInverseTransposeViewMatrix(bool left = true) const
   {
      return(getViewMatrix(left).invert().transpose());
   }

   //! get combined viewing and projection matrix
   mat4 getViewProjectionMatrix(bool left = true) const
   {
      mat4 P = getProjectionMatrix();
      mat4 V = getViewMatrix(left);

      return(P * V);
   }

   //! push viewing and projection matrix
   void begin(bool left = true) const
   {
      lglMatrixMode(LGL_PROJECTION);
      lglPushMatrix();
      lglLoadIdentity();
      if (perspective_)
         lglPerspective(fovy_, aspect_, nearp_, farp_);
      else
         lglOrtho(left_, right_, bottom_, top_, front_, back_);
      lglMatrixMode(LGL_MODELVIEW);
      lglPushMatrix();
      lglLookAt(getEye(left), getLookAt(), getUp());
   }

   //! pop viewing and projection matrix
   void end() const
   {
      lglMatrixMode(LGL_PROJECTION);
      lglPopMatrix();
      lglMatrixMode(LGL_MODELVIEW);
      lglPopMatrix();
   }

   //! halt cam
   void halt(bool yes = true)
   {
      halted_ = yes;
   }

   //! resume cam
   void resume(bool yes = true)
   {
      halted_ = !yes;
   }

   //! reset cam manipulator
   void reset()
   {
      if (!halted_)
         lglResetManip();
   }

protected:

   bool perspective_;
   double left_, right_, bottom_, top_, front_, back_;
   double fovy_, aspect_, nearp_, farp_;
   vec3 eye_, lookat_, up_, alt_up_;
   double base_;
   bool lefteye_;
   bool halted_;
};

#endif
