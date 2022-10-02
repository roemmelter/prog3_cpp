// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL texture formats
#ifndef GLVERTEX_TEXFORMATS_H
#define GLVERTEX_TEXFORMATS_H

#include "glvertex_texture.h"
#include "glvertex_pnmformat.h"
#include "glvertex_rawformat.h"

//! load image into texture object from file name
inline GLuint lglLoadTextureInto(std::string filename, int *twidth, int *theight, bool mipmapping = true)
{
   GLuint texid = 0;

   unsigned char *img = NULL;
   int tcomps = 0;

   img = lglReadPnmImage(filename, twidth, theight, &tcomps);
   if (!img) img = lglLoadRawImage(filename, twidth, theight, &tcomps);

   if (img)
   {
      if (mipmapping)
      {
         if (tcomps == 1)
            texid = lglCreateMipmap2D(twidth,theight, LGL_LUMINANCE,img);
         else if (tcomps == 3)
            texid = lglCreateMipmap2D(twidth,theight, LGL_RGB,img);
         else if (tcomps == 4)
            texid = lglCreateMipmap2D(twidth,theight, LGL_RGBA,img);
      }
      else
      {
         if (tcomps == 1)
            texid = lglCreateTexmap2D(twidth,theight, LGL_LUMINANCE,img);
         else if (tcomps == 3)
            texid = lglCreateTexmap2D(twidth,theight, LGL_RGB,img);
         else if (tcomps == 4)
            texid = lglCreateTexmap2D(twidth,theight, LGL_RGBA,img);
      }

      free(img);
   }

   return(texid);
}

//! load image into texture object from file name (and from default installation paths)
inline GLuint lglLoadTexture(std::string filename, int *twidth, int *theight, bool mipmapping = true)
{
   GLuint texid = lglLoadTextureInto(filename, twidth, theight, mipmapping);

   if (texid == 0) texid = lglLoadTextureInto((std::string("../") + filename).c_str(), twidth, theight, mipmapping);
   if (texid == 0) texid = lglLoadTextureInto((std::string("data/") + filename).c_str(), twidth, theight, mipmapping);
   if (texid == 0) texid = lglLoadTextureInto((std::string("../data/") + filename).c_str(), twidth, theight, mipmapping);
   if (texid == 0) texid = lglLoadTextureInto((std::string("/usr/local/share/") + filename).c_str(), twidth, theight, mipmapping);

   return(texid);
}

//! load image into texture object from file name (and from default installation paths)
inline GLuint lglLoadTexture(std::string filename, bool mipmapping = true)
{
   int twidth, theight;
   return(lglLoadTexture(filename, &twidth, &theight, mipmapping));
}

#endif
