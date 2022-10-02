// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_GL_H
#define GLVERTEX_GL_H

#ifdef _WIN32
#   ifndef APIENTRY
#      ifndef NOMINMAX
#         define NOMINMAX
#      endif
#      include <windows.h>
#   endif
#endif

#if defined(LGL_USE_GLES) || defined(LGL_USE_GLES2) || defined(LGL_USE_GLES3)
#   ifndef __APPLE__
#      ifndef LGL_USE_GLES3
#         define GL_GLEXT_PROTOTYPES
#         include <GLES2/gl2.h>
#         include <GLES2/gl2ext.h>
#      else
#         define GL_GLEXT_PROTOTYPES
#         include <GLES3/gl3.h>
#         include <GLES3/gl3ext.h>
#      endif
#   else
#      ifndef LGL_USE_GLES3
#         include <OpenGLES/ES2/gl.h>
#      else
#         include <OpenGLES/ES3/gl.h>
#      endif
#   endif
#else
#   ifndef __APPLE__
#      define GL_GLEXT_PROTOTYPES
#      include <GL/gl.h>
#      ifndef _WIN32
#         ifndef __glew_h__
#            include <GL/glext.h>
#         endif
#      else
#         include "WGL/glext.h"
#      endif
#   else
#      include <OpenGL/gl.h>
#      include <OpenGL/glext.h>
#   endif
#endif

#ifdef GL_ES_VERSION_2_0
#   define LGL_CORE # enable vbo and glsl api
#   define LGL_GLES # enable gles2 api
#   ifdef GL_ES_VERSION_3_0
#      define LGL_GLES3 # enable gles3 api
#      ifdef GL_ES_VERSION_3_1
#         ifdef GL_ES_VERSION_3_2
#            define LGL_OPENGL_VERSION 32
#         else
#            define LGL_OPENGL_VERSION 31
#         endif
#      else
#         define LGL_OPENGL_VERSION 30
#      endif
#   else
#      define LGL_OPENGL_VERSION 20
#   endif
#endif

#ifdef GL_VERSION_2_1
#   define LGL_CORE # enable vbo and glsl api
#   ifdef GL_VERSION_3_0
#      define LGL_GL3 # enable gl3 api
#      ifdef GL_VERSION_3_1
#         ifdef GL_VERSION_3_2
#            ifdef GL_VERSION_3_3
#               ifdef GL_VERSION_4_0
#                  ifdef GL_VERSION_4_1
#                     ifdef GL_VERSION_4_2
#                        ifdef GL_VERSION_4_3
#                           ifdef GL_VERSION_4_4
#                              ifdef GL_VERSION_4_5
#                                 ifdef GL_VERSION_4_6
#                                    define LGL_OPENGL_VERSION 46
#                                 else
#                                    define LGL_OPENGL_VERSION 45
#                                 endif
#                              else
#                                 define LGL_OPENGL_VERSION 44
#                              endif
#                           else
#                              define LGL_OPENGL_VERSION 43
#                           endif
#                        else
#                           define LGL_OPENGL_VERSION 42
#                        endif
#                     else
#                        define LGL_OPENGL_VERSION 41
#                     endif
#                  else
#                     define LGL_OPENGL_VERSION 40
#                  endif
#               else
#                  define LGL_OPENGL_VERSION 33
#               endif
#            else
#               define LGL_OPENGL_VERSION 32
#            endif
#         else
#            define LGL_OPENGL_VERSION 31
#         endif
#      else
#         define LGL_OPENGL_VERSION 30
#      endif
#   else
#      define LGL_OPENGL_VERSION 21
#   endif
#endif

#ifndef LGL_OPENGL_VERSION
#   define LGL_OPENGL_VERSION 12
#endif

#if (LGL_OPENGL_VERSION < 32)
#   ifndef GL_MIN
#      define GL_MIN GL_MIN_EXT
#   endif
#   ifndef GL_MAX
#      define GL_MAX GL_MAX_EXT
#   endif
#   ifndef GL_FUNC_REVERSE_SUBTRACT
#      define GL_FUNC_REVERSE_SUBTRACT GL_FUNC_REVERSE_SUBTRACT_EXT
#   endif
#endif

#if (LGL_OPENGL_VERSION < 46)
#   ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY
#      define GL_MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#   endif
#   ifndef GL_TEXTURE_MAX_ANISOTROPY
#      define GL_TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY_EXT
#   endif
#endif

#ifdef LGL_GLES
#   define GL_INTENSITY GL_LUMINANCE
#endif

#ifdef LGL_DISABLE_CORE
#   undef LGL_CORE
#endif

#endif
