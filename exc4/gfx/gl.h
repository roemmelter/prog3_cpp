// (c) by Stefan Roettger, licensed under MIT license

#ifndef GL_H
#define GL_H

#include "defs.h"

#ifdef _WIN32
#   ifndef APIENTRY
#      ifndef NOMINMAX
#         define NOMINMAX
#      endif
#      include <windows.h>
#   endif
#endif

#ifndef __APPLE__
#   ifndef _WIN32
#      define GL_GLEXT_PROTOTYPES
#   endif
#   include <GL/gl.h>
#   include <GL/glu.h>
#   ifndef __sgi
       extern "C"
          {
#      ifndef _WIN32
#         include <GL/glext.h>
#      else
#         include "GL/glext.h"
#      endif
          }
#   endif
#else
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <OpenGL/glext.h>
#endif

void initwglprocs();

#ifdef _WIN32

#ifdef GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB=NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB=NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB=NULL;
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
PFNGLGENPROGRAMSARBPROC glGenProgramsARB=NULL;
PFNGLBINDPROGRAMARBPROC glBindProgramARB=NULL;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB=NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB=NULL;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB=NULL;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB=NULL;
#endif

#endif

#endif
