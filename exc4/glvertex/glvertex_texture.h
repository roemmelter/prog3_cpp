// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL texture functions
#ifndef GLVERTEX_TEXTURE_H
#define GLVERTEX_TEXTURE_H

#include "glvertex_core.h"
#include "glvertex_texture_gl.h"

//! texture type
enum lgl_texmap_type {
   LGL_RGB,
   LGL_RGBA,
   LGL_INTENSITY,
   LGL_LUMINANCE,
   LGL_LUMINANCE_ALPHA
};

inline vec4f lglTexmapGet(int x, int y,
                          int width, int height,
                          lgl_texmap_type type, unsigned char *data)
{
   vec4f value(0);

   if (x>=0 && x<width && y>=0 && y<height)
   {
      int components = 0;

      switch (type)
      {
         case LGL_RGB: components = 3; break;
         case LGL_RGBA: components = 4; break;
         case LGL_INTENSITY: case LGL_LUMINANCE: components = 1; break;
         case LGL_LUMINANCE_ALPHA: components = 2; break;
      }

      int offset = (x+y*width)*components;

      if (components > 0) value.r = data[offset]*1.0f/255;
      if (components > 1) value.g = data[offset+1]*1.0f/255;
      if (components > 2) value.b = data[offset+2]*1.0f/255;
      if (components > 3) value.a = data[offset+3]*1.0f/255;
   }

   return(value);
}

inline void lglTexmapSet(int x, int y, vec4f value,
                         int width, int height,
                         lgl_texmap_type type, unsigned char *data)
{
   if (x>=0 && x<width && y>=0 && y<height)
   {
      int components = 0;

      switch (type)
      {
         case LGL_RGB: components = 3; break;
         case LGL_RGBA: components = 4; break;
         case LGL_INTENSITY: case LGL_LUMINANCE: components = 1; break;
         case LGL_LUMINANCE_ALPHA: components = 2; break;
      }

      if (value.r < 0) value.r = 0;
      else if (value.r > 1) value.r = 1;

      if (value.g < 0) value.g = 0;
      else if (value.g > 1) value.g = 1;

      if (value.b < 0) value.b = 0;
      else if (value.b > 1) value.b = 1;

      if (value.a < 0) value.a = 0;
      else if (value.a > 1) value.a = 1;

      int offset = (x+y*width)*components;

      if (components > 0) data[offset] = (unsigned char)(value.r*255+0.5f);
      if (components > 1) data[offset+1] = (unsigned char)(value.g*255+0.5f);
      if (components > 2) data[offset+2] = (unsigned char)(value.b*255+0.5f);
      if (components > 3) data[offset+3] = (unsigned char)(value.a*255+0.5f);
   }
}

inline vec4f lglSampleTexmap(float s, float t,
                             int width, int height,
                             lgl_texmap_type type, unsigned char *data)
{
   int x, y;
   float u, v;

   vec4f value(0);

   if (width<1 || height<1)
   {
      lglError("invalid 2D texture size");
      return(0);
   }

   if (s < 0) s = 0;
   else if (s > 1) s = 1;

   if (t < 0) t = 0;
   else if (t > 1) t = 1;

   s *= width-1;
   t *= height-1;

   x = (int)floor(s);
   y = (int)floor(t);

   u = s - x;
   v = t - y;

   if (width>1 && height>1)
   {
      if (x == width-1)
      {
         x = width-2;
         u = 1.0f;
      }

      if (y == height-1)
      {
         y = height-2;
         v = 1.0f;
      }

      vec4f v1 = lglTexmapGet(x,y, width,height, type,data);
      vec4f v2 = lglTexmapGet(x+1,y, width,height, type,data);
      vec4f v3 = lglTexmapGet(x,y+1, width,height, type,data);
      vec4f v4 = lglTexmapGet(x+1,y+1, width,height, type,data);

      value = (1-v)*((1-u)*v1+u*v2)+v*((1-u)*v3+u*v4);
   }
   else if (width>1)
   {
      if (x == width-1)
      {
         x = width-2;
         u = 1.0f;
      }

      vec4f v1 = lglTexmapGet(x,y, width,height, type,data);
      vec4f v2 = lglTexmapGet(x+1,y, width,height, type,data);

      value = (1-u)*v1+u*v2;
   }
   else
   {
      if (y == height-1)
      {
         y = height-2;
         v = 1.0f;
      }

      vec4f v1 = lglTexmapGet(x,y, width,height, type,data);
      vec4f v2 = lglTexmapGet(x,y+1, width,height, type,data);

      value = (1-v)*v1+v*v2;
   }

   return(value);
}

inline unsigned char *lglScaleTexmap(int inwidth, int inheight,
                                     int outwidth, int outheight,
                                     lgl_texmap_type type, unsigned char *data)
{
   if (inwidth<1 || inheight<1 || outwidth<1 || outheight<1)
   {
      lglError("invalid 2D texture size");
      return(NULL);
   }

   int components = 0;

   switch (type)
   {
      case LGL_RGB: components = 3; break;
      case LGL_RGBA: components = 4; break;
      case LGL_INTENSITY: case LGL_LUMINANCE: components = 1; break;
      case LGL_LUMINANCE_ALPHA: components = 2; break;
   }

   unsigned char *data2 = (unsigned char *)malloc(outwidth*outheight*components);

   if (!data2)
   {
      lglError("insufficient memory");
      return(NULL);
   }

   for (int x=0; x<outwidth; x++)
      for (int y=0; y<outheight; y++)
      {
         float s = (float)x/(outwidth-1);
         float t = (float)y/(outheight-1);

         vec4f value = lglSampleTexmap(s,t, inwidth,inheight, type,data);
         lglTexmapSet(x,y, value, outwidth,outheight, type,data2);
      }

   return(data2);
}

inline unsigned char *lglScaleTexmap2POT(int *width, int *height,
                                         lgl_texmap_type type, unsigned char *data)
{
   if (*width<1 || *height<1)
   {
      lglError("invalid 2D texture size");
      return(NULL);
   }

   int width2, height2;

   for (width2=2; width2<*width; width2*=2) ;
   for (height2=2; height2<*height; height2*=2) ;

   if (width2==*width && height2==*height)
      return(data);
   else
   {
      unsigned char *data2 = lglScaleTexmap(*width,*height, width2,height2, type,data);

      *width = width2;
      *height = height2;

      return(data2);
   }
}

inline unsigned char *lglDownsizeTexmap(int *width, int *height,
                                        lgl_texmap_type type, unsigned char *data)
{
   if (*width<2 && *height<2)
   {
      lglError("invalid 2D texture size");
      return(NULL);
   }

   int components = 0;

   switch (type)
   {
      case LGL_RGB: components = 3; break;
      case LGL_RGBA: components = 4; break;
      case LGL_INTENSITY: case LGL_LUMINANCE: components = 1; break;
      case LGL_LUMINANCE_ALPHA: components = 2; break;
   }

   int width2 = *width;
   int height2 = *height;

   if (width2&1) width2++;
   if (height2&1) height2++;

   unsigned char *data2 = data;

   if (width2!=*width || height2!=*height)
      data2 = lglScaleTexmap(*width,*height, width2,height2, type,data);

   *width = width2/2;
   *height = height2/2;

   unsigned char *down = (unsigned char *)malloc((*width)*(*height)*components);

   if (!down)
   {
      lglError("insufficient memory");
      return(NULL);
   }

   for (int x=0; x<*width; x++)
      for (int y=0; y<*height; y++)
      {
         int x2 = 2*x;
         int y2 = 2*y;

         vec4f v1 = lglTexmapGet(x2,y2, width2,height2, type,data2);
         vec4f v2 = lglTexmapGet(x2+1,y2, width2,height2, type,data2);
         vec4f v3 = lglTexmapGet(x2,y2+1, width2,height2, type,data2);
         vec4f v4 = lglTexmapGet(x2+1,y2+1, width2,height2, type,data2);

         vec4f value = 0.25f*(v1+v2+v3+v4);

         lglTexmapSet(x,y, value, *width,*height, type,down);
      }

   if (data2 != data)
      free(data2);

   return(down);
}

//! create a 2D texture map
inline GLuint lglCreateTexmap2D(int *width, int *height,
                                lgl_texmap_type type, unsigned char *data)
{
   if (*width<1 || *height<1)
   {
      lglError("invalid 2D texture size");
      return(0);
   }

   GLuint texid;

   GLenum gl_type = GL_RGB;

   switch (type)
   {
      case LGL_RGB: gl_type = GL_RGB; break;
      case LGL_RGBA: gl_type = GL_RGBA; break;
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_INTENSITY: gl_type = GL_RED; break;
#else
      case LGL_INTENSITY: gl_type = GL_INTENSITY; break;
#endif
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_LUMINANCE: gl_type = GL_RED; break;
#else
      case LGL_LUMINANCE: gl_type = GL_LUMINANCE; break;
#endif
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_LUMINANCE_ALPHA: gl_type = GL_RG; break;
#else
      case LGL_LUMINANCE_ALPHA: gl_type = GL_LUMINANCE_ALPHA; break;
#endif
   }

   unsigned char *data2 = lglScaleTexmap2POT(width,height, type,data);

   if (!data2)
   {
      lglError("insufficient memory");
      return(0);
   }

   glGenTextures(1, &texid);
   glBindTexture(GL_TEXTURE_2D, texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   lglTexImage2D(GL_TEXTURE_2D, 0, gl_type, *width, *height, 0, gl_type==GL_INTENSITY?GL_LUMINANCE:gl_type, GL_UNSIGNED_BYTE, data2);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

#ifdef GLVERTEX_TEXTURE_SWIZZLE
   if (gl_type == GL_RED)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, type==LGL_LUMINANCE?GL_ONE:GL_RED);
   }
   else if (gl_type == GL_RG)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
   }
#endif

   glBindTexture(GL_TEXTURE_2D, 0);

   if (data2 != data)
      free(data2);

   return(texid);
}

//! create a 2D texture map
inline GLuint lglCreateTexmap2D(int width, int height,
                                lgl_texmap_type type, unsigned char *data)
{
   return(lglCreateTexmap2D(&width, &height, type, data));
}

//! create a 2D texture mip-map
inline GLuint lglCreateMipmap2D(int *width, int *height,
                                lgl_texmap_type type, unsigned char *data)
{
   if (*width<1 || *height<1)
   {
      lglError("invalid 2D texture size");
      return(0);
   }

   GLuint texid;

   GLenum gl_type = GL_RGB;

   switch (type)
   {
      case LGL_RGB: gl_type = GL_RGB; break;
      case LGL_RGBA: gl_type = GL_RGBA; break;
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_INTENSITY: gl_type = GL_RED; break;
#else
      case LGL_INTENSITY: gl_type = GL_INTENSITY; break;
#endif
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_LUMINANCE: gl_type = GL_RED; break;
#else
      case LGL_LUMINANCE: gl_type = GL_LUMINANCE; break;
#endif
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_LUMINANCE_ALPHA: gl_type = GL_RG; break;
#else
      case LGL_LUMINANCE_ALPHA: gl_type = GL_LUMINANCE_ALPHA; break;
#endif
   }

   unsigned char *data2 = lglScaleTexmap2POT(width,height, type,data);

   if (!data2)
   {
      lglError("insufficient memory");
      return(0);
   }

   glGenTextures(1, &texid);
   glBindTexture(GL_TEXTURE_2D, texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   lglTexImage2D(GL_TEXTURE_2D, 0, gl_type, *width, *height, 0, gl_type==GL_INTENSITY?GL_LUMINANCE:gl_type, GL_UNSIGNED_BYTE, data2);

   int width2 = *width;
   int height2 = *height;

   unsigned char *m1 = data2;
   unsigned char *m2 = NULL;

   for (int level=1; width2>1 || height2>1; level++)
   {
      m2 = lglDownsizeTexmap(&width2,&height2, type,m1);

      if (!m2)
      {
         lglError("insufficient memory");
         break;
      }

      if (level > 1)
         free(m1);

      lglTexImage2D(GL_TEXTURE_2D, level, gl_type, width2, height2, 0, gl_type==GL_INTENSITY?GL_LUMINANCE:gl_type, GL_UNSIGNED_BYTE, m2);

      m1 = m2;
   }

   if (m2)
      free(m2);

   if (data2 != data)
      free(data2);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

#ifdef GLVERTEX_TEXTURE_SWIZZLE
   if (gl_type == GL_RED)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, type==LGL_LUMINANCE?GL_ONE:GL_RED);
   }
   else if (gl_type == GL_RG)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
   }
#endif

   glBindTexture(GL_TEXTURE_2D, 0);

   return(texid);
}

//! create a 2D texture mip-map
inline GLuint lglCreateMipmap2D(int width, int height,
                                lgl_texmap_type type, unsigned char *data)
{
   return(lglCreateMipmap2D(&width, &height, type, data));
}

//! create a 3D texture map
inline GLuint lglCreateTexmap3D(int width, int height, int depth,
                                lgl_texmap_type type, unsigned char *data)
{
#if !defined(LGL_GLES) || defined (LGL_GLES3)

   if (width<1 || height<1 || depth<1)
   {
      lglError("invalid 3D texture size");
      return(0);
   }

   if ((width&(width-1))!=0) lglWarning("3D texture map width not a power of two");
   if ((height&(height-1))!=0) lglWarning("3D texture map height not a power of two");
   if ((depth&(depth-1))!=0) lglWarning("3D texture map depth not a power of two");

   GLuint texid;

   GLenum gl_type = GL_RGB;

   switch (type)
   {
      case LGL_RGB: gl_type = GL_RGB; break;
      case LGL_RGBA: gl_type = GL_RGBA; break;
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_INTENSITY: gl_type = GL_RED; break;
#else
      case LGL_INTENSITY: gl_type = GL_INTENSITY; break;
#endif
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_LUMINANCE: gl_type = GL_RED; break;
#else
      case LGL_LUMINANCE: gl_type = GL_LUMINANCE; break;
#endif
#ifdef GLVERTEX_TEXTURE_SWIZZLE
      case LGL_LUMINANCE_ALPHA: gl_type = GL_RG; break;
#else
      case LGL_LUMINANCE_ALPHA: gl_type = GL_LUMINANCE_ALPHA; break;
#endif
   }

   glGenTextures(1, &texid);
   glBindTexture(GL_TEXTURE_3D, texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   lglTexImage3D(GL_TEXTURE_3D, 0, gl_type, width, height, depth, 0, gl_type==GL_INTENSITY?GL_LUMINANCE:gl_type, GL_UNSIGNED_BYTE, data);

   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

#ifdef GLVERTEX_TEXTURE_SWIZZLE
   if (gl_type == GL_RED)
   {
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, type==LGL_LUMINANCE?GL_ONE:GL_RED);
   }
   else if (gl_type == GL_RG)
   {
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
   }
#endif

   glBindTexture(GL_TEXTURE_3D, 0);

   return(texid);

#else

   return(0);

#endif
}

//! create a 3D noise texture
inline GLuint lglCreateNoiseTexmap3D(int width, int height, int depth)
{
#if !defined(LGL_GLES) || defined (LGL_GLES3)

   if (width<1 || height<1 || depth<1)
   {
      lglError("invalid noise texture size");
      return(0);
   }

   GLuint texid;

   float *data = glslnoise::perlinnoise::noise(width, height, depth);

   if (!data)
   {
      lglError("invalid noise texture");
      return(0);
   }

   glGenTextures(1, &texid);
   glBindTexture(GL_TEXTURE_3D, texid);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifdef GLVERTEX_TEXTURE_SWIZZLE
   lglTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width, height, depth, 0, GL_RED, GL_FLOAT, data);
#else
   lglTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, width, height, depth, 0, GL_LUMINANCE, GL_FLOAT, data);
#endif

   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

#ifdef GLVERTEX_TEXTURE_SWIZZLE
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_RED);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_RED);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_A, GL_RED);
#endif

   glBindTexture(GL_TEXTURE_3D, 0);

   free(data);

   return(texid);

#else

   return(0);

#endif
}

//! delete a 2D or 3D texture map
inline void lglDeleteTexture(GLuint texid)
{
   GLuint GLtexid = texid;
   if (texid > 0) glDeleteTextures(1, &GLtexid);
}

//! specify the 2D texture magnification filter type
inline void lglTexMagFilter2D(GLuint texid, bool linear = true)
{
   glBindTexture(GL_TEXTURE_2D, texid);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear?GL_LINEAR:GL_NEAREST);
   glBindTexture(GL_TEXTURE_2D, 0);
}

//! specify the 3D texture magnification filter type
inline void lglTexMagFilter3D(GLuint texid, bool linear = true)
{
#if !defined(LGL_GLES) || defined (LGL_GLES3)

   glBindTexture(GL_TEXTURE_3D, texid);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, linear?GL_LINEAR:GL_NEAREST);
   glBindTexture(GL_TEXTURE_3D, 0);

#endif
}

//! specify the 2D texture wrap type
inline void lglTexWrap2D(GLuint texid, bool repeat = true)
{
   glBindTexture(GL_TEXTURE_2D, texid);

#ifndef GL_CLAMP_TO_EDGE
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat?GL_REPEAT:GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat?GL_REPEAT:GL_CLAMP);
#else
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
#endif

   glBindTexture(GL_TEXTURE_2D, texid);
}

//! specify the 3D texture wrap type
inline void lglTexWrap3D(GLuint texid, bool repeat = true)
{
#if !defined(LGL_GLES) || defined (LGL_GLES3)

   glBindTexture(GL_TEXTURE_3D, texid);

#ifndef GL_CLAMP_TO_EDGE
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, repeat?GL_REPEAT:GL_CLAMP);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, repeat?GL_REPEAT:GL_CLAMP);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, repeat?GL_REPEAT:GL_CLAMP);
#else
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
#endif

   glBindTexture(GL_TEXTURE_3D, texid);

#endif
}

//! specify the 2D texture anisotropic filter type
inline void lglTexFilterAnisotropic2D(GLuint texid, bool ansiotropic = true)
{
#ifdef GL_EXT_texture_filter_anisotropic

   glBindTexture(GL_TEXTURE_2D, texid);

   GLfloat maxaniso=1.0f;
   if (ansiotropic) glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&maxaniso);
   glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,&maxaniso);

   glBindTexture(GL_TEXTURE_2D, texid);

#endif
}

//! specify the 3D texture anisotropic filter type
inline void lglTexFilterAnisotropic3D(GLuint texid, bool ansiotropic = true)
{
#if !defined(LGL_GLES) || defined (LGL_GLES3)

#ifdef GL_EXT_texture_filter_anisotropic

   glBindTexture(GL_TEXTURE_3D, texid);

   GLfloat maxaniso=1.0f;
   if (ansiotropic) glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&maxaniso);
   glTexParameterfv(GL_TEXTURE_3D,GL_TEXTURE_MAX_ANISOTROPY_EXT,&maxaniso);

   glBindTexture(GL_TEXTURE_3D, texid);

#endif

#endif
}

//! get the maximum texture size
inline int lglGetMaxTexSize()
{
   GLint param;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&param);
   return(param);
}

#endif
