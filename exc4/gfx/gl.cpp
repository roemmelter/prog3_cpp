// (c) by Stefan Roettger, licensed under MIT license

#include "gl.h"

void initwglprocs()
   {
#ifdef _WIN32

   static int done=FALSE;

   if (!done)
      {
#ifdef GL_ARB_multitexture
      if ((glActiveTextureARB=(PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB"))==NULL ||
          (glClientActiveTextureARB=(PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB"))==NULL ||
          (glMultiTexCoord3fARB=(PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress("glMultiTexCoord3fARB"))==NULL)
         WARNMSG("unsupported multi-texturing");
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
      if (glext_vp && glext_fp)
         if ((glGenProgramsARB=(PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB"))==NULL ||
             (glBindProgramARB=(PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB"))==NULL ||
             (glProgramStringARB=(PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB"))==NULL ||
             (glProgramEnvParameter4fARB=(PFNGLPROGRAMENVPARAMETER4FARBPROC)wglGetProcAddress("glProgramEnvParameter4fARB"))==NULL ||
             (glDeleteProgramsARB=(PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB"))==NULL ||
             (glGetProgramivARB=(PFNGLGETPROGRAMIVARBPROC)wglGetProcAddress("glGetProgramivARB"))==NULL)
            WARNMSG("unsupported shader programs");
#endif

      done=TRUE;
      }

#endif
   }
