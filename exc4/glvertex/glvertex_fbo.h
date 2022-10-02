// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL frame buffer object (LGL fbo)
#ifndef GLVERTEX_FBO_H
#define GLVERTEX_FBO_H

#include <string.h>

#include "glvertex_gl.h"
#include "glvertex_fbo_gl.h"

#ifdef __APPLE__
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
#include <QApplication>
#endif
#endif

//! frame buffer object base class
class lglFBO_base
{
public:

   //! ctor
   lglFBO_base()
      : init_(false)
   {}

protected:

   bool init_;

#ifdef _WIN32

   #undef WGL_MACRO
   #define WGL_MACRO(proc,proctype) PFN##proctype##PROC proc;
   #include "glvertex_fbo_wgl.h"

   void initWGLprocs()
   {
      if (!init_)
      {
         #undef WGL_MACRO
         #define WGL_MACRO(proc,proctype) if ((proc=(PFN##proctype##PROC)wglGetProcAddress(#proc))==NULL) lglWarning(#proc" not supported");
         #include "glvertex_fbo_wgl.h"
         init_ = true;
      }
   }

#endif
};

//! frame buffer object
class lglFBO: public lglFBO_base
{
public:

   //! ctor
   lglFBO()
      : lglFBO_base()
   {
      hasFBO_ = false;
      fboWidth_ = fboHeight_ = 0;
      fboMipmap_ = false;
      fboFloat_ = false;
      textureId_ = 0;
      rboId_ = fboId_ = 0;
      prev_ = 0;
   }

   ~lglFBO()
   {
      destroyFBO();
   }

   //! create fbo
   bool setupFBO(int width, int height, bool fbomipmap = false, bool fbofloat = false, bool force = false)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

#ifdef __APPLE__
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
      float factor =  qApp->devicePixelRatio();
      width = (int)(width * factor + 0.5f);
      height = (int)(height * factor + 0.5f);
#endif
#endif

#if (LGL_OPENGL_VERSION < 32)
      char *GL_EXTs;

      if ((GL_EXTs=(char *)glGetString(GL_EXTENSIONS)) != NULL)
         if (strstr(GL_EXTs, "EXT_framebuffer_object") != NULL)
#endif
            if (width>0 && height>0)
               if (!hasFBO_ ||
                   width!=fboWidth_ || height!=fboHeight_ ||
                   fbomipmap!=fboMipmap_ || fbofloat!=fboFloat_)
               {
#ifdef GL_EXT_framebuffer_object

                  destroyFBO();

                  hasFBO_ = true;

                  // save actual size and state
                  fboWidth_ = width;
                  fboHeight_ = height;
                  fboMipmap_ = fbomipmap;
                  fboFloat_ = fbofloat;

                  // create a texture object
                  glGenTextures(1, &textureId_);
                  glBindTexture(GL_TEXTURE_2D, textureId_);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if (LGL_OPENGL_VERSION >= 30)
                  if (fbomipmap) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                  else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                  if (fbofloat) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width, height, 0, GL_RGBA, GL_HALF_FLOAT_ARB, 0);
                  else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                  glBindTexture(GL_TEXTURE_2D, 0);

                  // create a renderbuffer object to store depth info
                  glGenRenderbuffers(1, &rboId_);
                  glBindRenderbuffer(GL_RENDERBUFFER, rboId_);
                  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
                  glBindRenderbuffer(GL_RENDERBUFFER, 0);

                  // create a framebuffer object
                  glGenFramebuffers(1, &fboId_);
                  glBindFramebuffer(GL_FRAMEBUFFER, fboId_);

                  // attach texture to color attachment point
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId_, 0);

                  // attach renderbuffer to depth attachment point
                  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId_);

                  // get framebuffer status
                  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

                  // switch back to window-system-provided framebuffer
                  glBindFramebuffer(GL_FRAMEBUFFER, 0);

                  // check framebuffer status
                  if (status != GL_FRAMEBUFFER_COMPLETE) destroyFBO();

#endif
               }

#if (LGL_OPENGL_VERSION < 30)
      if (fbomipmap && force) destroyFBO();
#endif

      return(hasFBO_);
   }

   //! destroy fbo
   void destroyFBO()
   {
#ifdef GL_EXT_framebuffer_object

      if (textureId_ != 0) glDeleteTextures(1, &textureId_);
      if (rboId_ != 0) glDeleteRenderbuffers(1, &rboId_);
      if (fboId_ != 0) glDeleteFramebuffers(1, &fboId_);

      textureId_ = 0;
      rboId_ = 0;
      fboId_ = 0;

      hasFBO_ = false;
      fboWidth_ = fboHeight_ = 0;
      fboMipmap_ = false;
      fboFloat_ = false;

#endif
   }

   //! enable fbo
   void activateFBO()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasFBO_)
      {
         // query actual binding
         prev_ = 0;
         glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_);

         // bind framebuffer
         glBindFramebuffer(GL_FRAMEBUFFER, fboId_);
      }

#endif
   }

   //! disable fbo
   GLuint deactivateFBO()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasFBO_)
      {
         // restore previous binding
         glBindFramebuffer(GL_FRAMEBUFFER, prev_);

#if (LGL_OPENGL_VERSION >= 30)
         if (fboMipmap_)
         {
            glBindTexture(GL_TEXTURE_2D, textureId_);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
         }
#endif
      }

#endif

      return(getFBOTextureId());
   }

   //! get fbo id
   GLuint getFBOId()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasFBO_)
         return(fboId_);

#endif

      return(0);
   }

   //! get fbo texture id
   GLuint getFBOTextureId()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasFBO_)
         return(textureId_);

#endif

      return(0);
   }

   //! get fbo texture width
   int width()
   {
      return(fboWidth_);
   }

   //! get fbo texture height
   int height()
   {
      return(fboHeight_);
   }

   //! query fbo texture mip-mapping
   int fbo_mipmap()
   {
      return(fboMipmap_);
   }

   //! query fbo float mode
   int fbo_float()
   {
      return(fboMipmap_);
   }

protected:

   bool hasFBO_;
   int fboWidth_, fboHeight_;
   bool fboMipmap_;
   bool fboFloat_;
   GLuint textureId_;
   GLuint rboId_;
   GLuint fboId_;
   GLint prev_;
};

//! depth buffer object
class lglDBO: public lglFBO_base
{
public:

   //! ctor
   lglDBO()
      : lglFBO_base()
   {
      hasDBO_ = false;
      dboWidth_ = dboHeight_ = 0;
      dboMipmap_ = false;
      textureId_ = 0;
      dboId_ = 0;
      prev_ = 0;
   }

   ~lglDBO()
   {
      destroyDBO();
   }

   //! create dbo
   bool setupDBO(int width, int height, bool dbomipmap = false, bool force = false)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

#if (LGL_OPENGL_VERSION < 32)
      char *GL_EXTs;

      if ((GL_EXTs=(char *)glGetString(GL_EXTENSIONS)) != NULL)
         if (strstr(GL_EXTs, "EXT_framebuffer_object") != NULL)
#endif
            if (width>0 && height>0)
               if (!hasDBO_ ||
                   width!=dboWidth_ || height!=dboHeight_ ||
                   dbomipmap!=dboMipmap_)
               {
#ifdef GL_EXT_framebuffer_object

                  destroyDBO();

                  hasDBO_ = true;

                  // save actual size and state
                  dboWidth_ = width;
                  dboHeight_ = height;
                  dboMipmap_ = dbomipmap;

                  // create a depth texture object
                  glGenTextures(1, &textureId_);
                  glBindTexture(GL_TEXTURE_2D, textureId_);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#if (LGL_OPENGL_VERSION >= 30)
                  if (dbomipmap) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                  else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if (LGL_OPENGL_VERSION < 30)
                  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
#endif
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
                  glBindTexture(GL_TEXTURE_2D, 0);

                  // create a framebuffer object
                  glGenFramebuffers(1, &dboId_);
                  glBindFramebuffer(GL_FRAMEBUFFER, dboId_);

                  // no color buffer is drawn to
                  glDrawBuffer(GL_NONE);
                  glReadBuffer(GL_NONE);

                  // attach texture to depth attachment point
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId_, 0);

                  // get framebuffer status
                  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

                  // switch back to window-system-provided framebuffer
                  glBindFramebuffer(GL_FRAMEBUFFER, 0);

                  // re-enable back buffer
                  glDrawBuffer(GL_BACK);
                  glReadBuffer(GL_BACK);

                  // check framebuffer status
                  if (status != GL_FRAMEBUFFER_COMPLETE) destroyDBO();

#endif
               }

#if (LGL_OPENGL_VERSION < 30)
      if (dbomipmap && force) destroyDBO();
#endif

      return(hasDBO_);
   }

   //! destroy dbo
   void destroyDBO()
   {
#ifdef GL_EXT_framebuffer_object

      if (textureId_ != 0) glDeleteTextures(1, &textureId_);
      if (dboId_ != 0) glDeleteFramebuffers(1, &dboId_);

      textureId_ = 0;
      dboId_ = 0;

      hasDBO_ = false;
      dboWidth_ = dboHeight_ = 0;
      dboMipmap_ = false;

#endif
   }

   //! enable dbo
   void activateDBO()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasDBO_)
      {
         // query actual binding
         prev_ = 0;
         glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_);

         // bind framebuffer
         glBindFramebuffer(GL_FRAMEBUFFER, dboId_);

         // no color buffer is drawn to
         glDrawBuffer(GL_NONE);
         glReadBuffer(GL_NONE);
      }

#endif
   }

   //! disable dbo
   GLuint deactivateDBO()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasDBO_)
      {
         // restore previous binding
         glBindFramebuffer(GL_FRAMEBUFFER, prev_);

         // re-enable back buffer
         glDrawBuffer(GL_BACK);
         glReadBuffer(GL_BACK);

#if (LGL_OPENGL_VERSION >= 30)
         if (dboMipmap_)
         {
            glBindTexture(GL_TEXTURE_2D, textureId_);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
         }
#endif
      }

#endif

      return(getDBOTextureId());
   }

   //! get dbo id
   GLuint getDBOId()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasDBO_)
         return(dboId_);

#endif

      return(0);
   }

   //! get dbo texture id
   GLuint getDBOTextureId()
   {
#ifdef GL_EXT_framebuffer_object

      if (hasDBO_)
         return(textureId_);

#endif

      return(0);
   }

   //! get dbo texture width
   int width()
   {
      return(dboWidth_);
   }

   //! get dbo texture height
   int height()
   {
      return(dboHeight_);
   }

   //! query dbo texture mip-mapping
   int dbo_mipmap()
   {
      return(dboMipmap_);
   }

protected:

   bool hasDBO_;
   int dboWidth_, dboHeight_;
   bool dboMipmap_;
   GLuint textureId_;
   GLuint dboId_;
   GLint prev_;
};

#endif
