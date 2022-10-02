// (c) by Stefan Roettger, licensed under MIT license
// glVertex core header-only library

//! \file
//! LGL core (for immediate mode and vbo rendering)
#ifndef GLVERTEX_CORE_H
#define GLVERTEX_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "glslmath.h"
#include "glvertex_gl.h"
#include "glvertex_io.h"
#include "glvertex_string.h"

//! graphics hardware vendor
enum lgl_vendor_enum
{
   LGL_NVIDIA = 1,
   LGL_ATI,
   LGL_INTEL,
   LGL_UNKNOWN
};

//! graphics primitive type
enum lgl_primitive_enum
{
   LGL_NONE = 0,
   LGL_LINES,
   LGL_LINE_STRIP,
   LGL_TRIANGLES,
   LGL_TRIANGLE_STRIP,
   LGL_TRIANGLE_FAN,
   LGL_QUADS,
   LGL_QUAD_STRIP
};

//! matrix mode enum
enum lgl_matrixmode_enum
{
   LGL_PROJECTION = 1,
   LGL_MODELVIEW,
   LGL_PREMODEL,
   LGL_TEXTURE,
   LGL_PRETEX
};

//! texgen mode enum
enum lgl_texgenmode_enum
{
   LGL_TEXGEN_NONE = 0,
   LGL_TEXGEN_LINEAR,
   LGL_TEXGEN_NORMAL,
   LGL_TEXGEN_HEMISPHERE,
   LGL_TEXGEN_HEDGEHOG,
   LGL_TEXGEN_NORMAL_HEMISPHERE,
   LGL_TEXGEN_NORMAL_HEDGEHOG,
   LGL_TEXGEN_MIXED_HEMISPHERE,
   LGL_TEXGEN_MIXED_HEDGEHOG
};

//! blend mode enum
enum lgl_blendmode_enum
{
   LGL_BLEND_NONE = 0,
   LGL_BLEND_MULT,
   LGL_BLEND_ALPHA,
   LGL_BLEND_ADD,
   LGL_BLEND_SUB,
   LGL_BLEND_MAX,
   LGL_BLEND_MIN
};

//! polygon mode enum
enum lgl_polygonmode_enum
{
   LGL_FILL = 1,
   LGL_LINE,
   LGL_BARYCENTRIC
};

//! interlacing mode enum
enum lgl_interlacing_enum
{
   LGL_INTERLACE_NONE = 0,
   LGL_INTERLACE_HORIZONTAL_LEFT,
   LGL_INTERLACE_HORIZONTAL_RIGHT,
   LGL_INTERLACE_VERTICAL_TOP,
   LGL_INTERLACE_VERTICAL_BOTTOM,
};

//! verbosity level enum
enum lgl_verbosity_enum
{
   LGL_VERBOSITY_NONE = 0,
   LGL_VERBOSITY_ERRORS = 1,
   LGL_VERBOSITY_WARNINGS = 2
};

#define LGL_NUM_BUFFERS 6

#ifndef LGL_NUM_ATTRIBUTES
#   define LGL_NUM_ATTRIBUTES 4
#endif

#define LGL_VERTEX_BUFFER 0
#define LGL_COLOR_BUFFER 1
#define LGL_NORMAL_BUFFER 2
#define LGL_TEXCOORD_BUFFER 3
#define LGL_BARYCENTRIC_BUFFER 4
#define LGL_ATTRIBUTE_BUFFER 5

#define LGL_VERTEX_LOCATION 0
#define LGL_COLOR_LOCATION 1
#define LGL_NORMAL_LOCATION 2
#define LGL_TEXCOORD_LOCATION 3
#define LGL_BARYCENTRIC_LOCATION 4
#define LGL_ATTRIBUTE_LOCATION 5

#define LGL_NUM_SHADERS 12

#ifndef LGL_CORE
#   define LGL_NUM_CLIPPLANES 6
#else
#   define LGL_NUM_CLIPPLANES 8
#endif

#include "glvertex_shaders.h"

//! LGL core
//! * implements an OpenGL vertex buffer object (vbo)
//!   to store a series of vertices
//! * can be either used to simulate immediate mode operations
//!   or as convenient wrapper for vertex buffer objects
//! * the series of vertices is handled in immediate mode fashion
//!  * the series starts with lglBegin()
//!  * vertices are added with lglVertex()
//!  * vertex attributes are specified before lglVertex() with
//!   * lglColor(), lglNormal(), lglTexCoord(), lglAttribute()
//!  * the series is closed with lglEnd()
//!
//! \param name is the name of the vbo
//! \param immediate determine mode of operation
//! * in immediate mode compiled vertices are immediately rendered on lglEnd()
//! * in vbo mode compiled vertices are rendered explicitly on lglRender()
//! \param storagetype determines the storage type of added vertices
template <class VEC4, const GLenum gl_type>
class lgl
{
public:

   //! ctor
   lgl(const std::string &name = "",
       bool immediate = false,
       bool storagetype = true)
      : name_(name),
        immediate_(immediate),
        storagetype_(storagetype),
        vertex_(0), copied_(false), copy_(false),
        color_(1), normal_(0), texcoord_(0),
        hascolor_(false), hasnormal_(false), hastexcoord_(false)
   {
      initialize();

      if (instances_ == 0)
         initializeStatic();

      instances_++;
   }

   virtual ~lgl()
   {
      finalize();

      if (initGL_)
         finalizeOpenGL();

      instances_--;

      if (instances_ == 0)
      {
         finalizeStatic();

         if (initStaticGL_)
         {
            finalizeStaticOpenGL();

            // call exit-gl hook
            exit_gl_hook();
         }
      }
   }

   //! get the vbo name
   std::string getName() const
   {
      return(name_);
   }

   //! set the vbo name
   void setName(std::string name = "")
   {
      name_ = name;
   }

private:

   lgl(const lgl &copy) {} // copy ctor unavailable by design
   lgl& operator = (const lgl &copy) {} // assignment operator unavailable by design

   union lgl_uniform_union
   {
      int int_value;
      float float_value;
      float vec2f_value[2];
      float vec3f_value[3];
      float vec4f_value[4];
      float mat2f_value[4];
      float mat3f_value[9];
      float mat4f_value[16];
   };

   enum lgl_uniform_enum
   {
      lgl_no_type,
      lgl_int_type,
      lgl_float_type,
      lgl_vec2f_type,
      lgl_vec3f_type,
      lgl_vec4f_type,
      lgl_mat2f_type,
      lgl_mat3f_type,
      lgl_mat4f_type
   };

   struct lgl_uniform_struct
   {
      GLint loc;
      std::string uniform;
      lgl_uniform_enum type;
      lgl_uniform_union value;
      bool warn;
   };

   typedef std::map<std::string, unsigned int> lgl_uniform_map_type;
   typedef std::vector<lgl_uniform_struct> lgl_uniform_location_type;

public:

   void lglVertex(const vec4 &v)
   {
      if (!started_)
      {
         lglError("invalid vertex specification");
         return;
      }

      if (!immediate_ && rendered_)
      {
         lglError("invalid vbo operation");
         return;
      }

      if (premodel_matrix_identity_.back())
      {
         if (copy_)
         {
            lglPlainVertex(v);
            if (primitive_ == LGL_QUAD_STRIP) lglPlainVertex(v);
            copy_ = false;
         }

         lglPlainVertex(v, color_, texcoord_, normal_);
      }
      else
      {
         if (!hasnormal_)
         {
            vec4 p = premodel_matrix_.back() * v;

            if (copy_)
            {
               lglPlainVertex(p);
               if (primitive_ == LGL_QUAD_STRIP) lglPlainVertex(p);
               copy_ = false;
            }

            lglPlainVertex(p, color_, texcoord_);
         }
         else
         {
            if (premodel_matrix_recompute_.back())
            {
               premodel_matrix_it_.back() = mat3(premodel_matrix_.back().invert().transpose());
               premodel_matrix_recompute_.back() = false;
            }

            vec4 p = premodel_matrix_.back() * v;
            vec3 n = premodel_matrix_it_.back() * normal_;

            if (copy_)
            {
               lglPlainVertex(p);
               if (primitive_ == LGL_QUAD_STRIP) lglPlainVertex(p);
               copy_ = false;
            }

            lglPlainVertex(p, color_, texcoord_, n);
         }
      }

      if (rearrange_)
         rearrangeQuads();

      copied_ = false;
   }

protected:

   void lglPlainVertex(const vec4 &v,
                       const vec4f &c = vec4f(1),
                       const vec4f &t = vec4f(0),
                       const vec3f &n = vec3f(0))
   {
      vertex_ = v;

      if (!enlarge()) return;

      if (storagetype_) vertices_[size_] = v;
      else verticesf_[size_] = v;

      colors_[size_] = c;
      normals_[size_] = n;

      if (texgen_ != LGL_TEXGEN_NONE)
      {
         vec4 texcoord = texgen(v, n);
         if (!pretex_matrix_identity_.back()) texcoord = pretex_matrix_.back() * texcoord;
         texcoords_[size_] = texcoord;
         hastexcoord_ = true;
      }
      else
      {
         vec4 texcoord = t;
         if (!pretex_matrix_identity_.back()) texcoord = pretex_matrix_.back() * texcoord;
         texcoords_[size_] = texcoord;
      }

      if (hasattributes_)
         for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
            if (hasattribute_[i])
               attributes_[size_*LGL_NUM_ATTRIBUTES+i] = attribute_[i];

      switch (primitive_)
      {
         case LGL_LINES:
         case LGL_LINE_STRIP:
            if (size_%2 == 0) barycentrics_[size_] = vec3f(1,0,1);
            else barycentrics_[size_] = vec3f(0,1,1);
            break;
         case LGL_TRIANGLE_FAN:
            if (size_ == 0) barycentrics_[size_] = vec3f(1,0,0);
            else
               if (size_%2 == 1) barycentrics_[size_] = vec3f(0,1,0);
               else barycentrics_[size_] = vec3f(0,0,1);
            break;
         case LGL_QUADS:
            switch (size_%4)
            {
               case 0: barycentrics_[size_] = vec3f(0,0,0.25); break;
               case 1: barycentrics_[size_] = vec3f(1,0,0.25); break;
               case 2: barycentrics_[size_] = vec3f(1,1,0.75); break;
               case 3: barycentrics_[size_] = vec3f(0,1,0.75); break;
            }
            break;
         case LGL_QUAD_STRIP:
            switch (size_%4)
            {
               case 0: barycentrics_[size_] = vec3f(0,0,0.25); break;
               case 1: barycentrics_[size_] = vec3f(1,0,0.25); break;
               case 2: barycentrics_[size_] = vec3f(0,1,0.75); break;
               case 3: barycentrics_[size_] = vec3f(1,1,0.75); break;
            }
            break;
         case LGL_TRIANGLE_STRIP:
         case LGL_TRIANGLES:
         default:
            switch (size_%3)
            {
               case 0: barycentrics_[size_] = vec3f(1,0,0); break;
               case 1: barycentrics_[size_] = vec3f(0,1,0); break;
               case 2: barycentrics_[size_] = vec3f(0,0,1); break;
            }
            break;
      }

      vec3 v3 = v;

      if (size_ == 0)
      {
         bboxmin_ = bboxmax_ = v3;
      }
      else
      {
         if (v3.x < bboxmin_.x) bboxmin_.x = v3.x;
         else if (v3.x > bboxmax_.x) bboxmax_.x = v3.x;
         if (v3.y < bboxmin_.y) bboxmin_.y = v3.y;
         else if (v3.y > bboxmax_.y) bboxmax_.y = v3.y;
         if (v3.z < bboxmin_.z) bboxmin_.z = v3.z;
         else if (v3.z > bboxmax_.z) bboxmax_.z = v3.z;
      }

      size_++;
      modified_ = true;
   }

   vec4 texgen(const vec4 &v, const vec3f &n)
   {
      vec4 texcoord = v;

      switch (texgen_)
      {
         case LGL_TEXGEN_NORMAL:
            texcoord = normalize(n);
            break;
         case LGL_TEXGEN_HEMISPHERE:
         {
            double x = texcoord.x;
            double y = texcoord.y;
            double z = texcoord.z;
            double alpha = atan2(z, x);
            double beta = atan2(sqrt(x*x+z*z), fabs(y));
            double r = beta / PI;
            r = (2*r + r*r)/3;
            texcoord.x = r * cos(alpha) + 0.5;
            texcoord.y = r * sin(alpha) + 0.5;
            texcoord.z = 0;
            break;
         }
         case LGL_TEXGEN_HEDGEHOG:
         {
            double x = texcoord.x;
            double y = texcoord.y;
            double z = texcoord.z;
            double alpha = atan2(z, x);
            double beta = atan2(sqrt(x*x+z*z), y);
            double r = 0.5 * beta / PI;
            r = (r < 0.5)? 2*(r + r*r)/3 : 2*r - (1 + 2*r*r)/3;
            texcoord.x = r * cos(alpha) + 0.5;
            texcoord.y = r * sin(alpha) + 0.5;
            texcoord.z = 0;
            break;
         }
         case LGL_TEXGEN_NORMAL_HEMISPHERE:
         {
            texcoord = normalize(n);
            double x = texcoord.x;
            double y = texcoord.y;
            double z = texcoord.z;
            double alpha = atan2(z, x);
            double beta = atan2(sqrt(x*x+z*z), fabs(y));
            double r = beta / PI;
            r = (2*r + r*r)/3;
            texcoord.x = r * cos(alpha) + 0.5;
            texcoord.y = r * sin(alpha) + 0.5;
            texcoord.z = 0;
            break;
         }
         case LGL_TEXGEN_NORMAL_HEDGEHOG:
         {
            texcoord = normalize(n);
            double x = texcoord.x;
            double y = texcoord.y;
            double z = texcoord.z;
            double alpha = atan2(z, x);
            double beta = atan2(sqrt(x*x+z*z), y);
            double r = 0.5 * beta / PI;
            r = (r < 0.5)? 2*(r + r*r)/3 : 2*r - (1 + 2*r*r)/3;
            texcoord.x = r * cos(alpha) + 0.5;
            texcoord.y = r * sin(alpha) + 0.5;
            texcoord.z = 0;
            break;
         }
         case LGL_TEXGEN_MIXED_HEMISPHERE:
         {
            double x = texcoord.x;
            double y = texcoord.y;
            double z = texcoord.z;
            double alpha = atan2(z, x);
            double beta = atan2(sqrt(x*x+z*z), fabs(y));
            double r = beta / PI;
            r = (2*r + r*r)/3;
            texcoord.x = r * cos(alpha) + 0.5;
            texcoord.y = r * sin(alpha) + 0.5;
            texcoord.z = 0;
            vec4 t = normalize(n);
            x = t.x;
            y = t.y;
            z = t.z;
            alpha = atan2(z, x);
            beta = atan2(sqrt(x*x+z*z), fabs(y));
            r = beta / PI;
            r = (2*r + r*r)/3;
            r *= texgen_mix_;
            texcoord.x += r * cos(alpha);
            texcoord.y += r * sin(alpha);
            break;
         }
         default:
         case LGL_TEXGEN_MIXED_HEDGEHOG:
         {
            double x = texcoord.x;
            double y = texcoord.y;
            double z = texcoord.z;
            double alpha = atan2(z, x);
            double beta = atan2(sqrt(x*x+z*z), y);
            double r = 0.5 * beta / PI;
            r = (r < 0.5)? 2*(r + r*r)/3 : 2*r - (1 + 2*r*r)/3;
            texcoord.x = r * cos(alpha) + 0.5;
            texcoord.y = r * sin(alpha) + 0.5;
            texcoord.z = 0;
            vec4 t = normalize(n);
            x = t.x;
            y = t.y;
            z = t.z;
            alpha = atan2(z, x);
            beta = atan2(sqrt(x*x+z*z), y);
            r = 0.5 * beta / PI;
            r = (r < 0.5)? 2*(r + r*r)/3 : 2*r - (1 + 2*r*r)/3;
            r *= texgen_mix_;
            texcoord.x += r * cos(alpha);
            texcoord.y += r * sin(alpha);
            break;
         }
      }

      return(texcoord);
   }

   bool enlarge()
   {
      if (maxsize_ == 0)
      {
         maxsize_ = 1024;

         if (storagetype_)
         {
            void *vbuffer = malloc(maxsize_*sizeof(VEC4));
            if ((vertices_=(VEC4 *)vbuffer) == NULL) return(false);
         }
         else
         {
            void *vbuffer = malloc(maxsize_*sizeof(vec4f));
            if ((verticesf_=(vec4f *)vbuffer) == NULL) return(false);
         }

         void *cbuffer = malloc(maxsize_*sizeof(vec4f));
         if ((colors_=(vec4f *)cbuffer) == NULL) return(false);

         void *nbuffer = malloc(maxsize_*sizeof(vec3f));
         if ((normals_=(vec3f *)nbuffer) == NULL) return(false);

         void *tbuffer = malloc(maxsize_*sizeof(vec4f));
         if ((texcoords_=(vec4f *)tbuffer) == NULL) return(false);

         void *bbuffer = malloc(maxsize_*sizeof(vec3f));
         if ((barycentrics_=(vec3f *)bbuffer) == NULL) return(false);

         void *abuffer = malloc(maxsize_*LGL_NUM_ATTRIBUTES*sizeof(vec4f));
         if ((attributes_=(vec4f *)abuffer) == NULL) return(false);
      }
      else if (size_ >= maxsize_)
      {
         if (storagetype_)
         {
            void *vbuffer = realloc((void *)vertices_, 2*maxsize_*sizeof(VEC4));
            if ((vertices_=(VEC4 *)vbuffer) == NULL) return(false);
         }
         else
         {
            void *vbuffer = realloc((void *)verticesf_, 2*maxsize_*sizeof(vec4f));
            if ((verticesf_=(vec4f *)vbuffer) == NULL) return(false);
         }

         void *cbuffer = realloc((void *)colors_, 2*maxsize_*sizeof(vec4f));
         if ((colors_=(vec4f *)cbuffer) == NULL) return(false);

         void *nbuffer = realloc((void *)normals_, 2*maxsize_*sizeof(vec3f));
         if ((normals_=(vec3f *)nbuffer) == NULL) return(false);

         void *tbuffer = realloc((void *)texcoords_, 2*maxsize_*sizeof(vec4f));
         if ((texcoords_=(vec4f *)tbuffer) == NULL) return(false);

         void *bbuffer = realloc((void *)barycentrics_, 2*maxsize_*sizeof(vec3f));
         if ((barycentrics_=(vec3f *)bbuffer) == NULL) return(false);

         void *abuffer = realloc((void *)attributes_, 2*maxsize_*LGL_NUM_ATTRIBUTES*sizeof(vec4f));
         if ((attributes_=(vec4f *)abuffer) == NULL) return(false);

         maxsize_ *= 2;
      }

      return(true);
   }

   void shrink()
   {
      if (maxsize_ != size_)
      {
         maxsize_ = size_;

         if (maxsize_ == 0)
         {
            if (storagetype_)
            {
               free(vertices_);
               vertices_ = NULL;
            }
            else
            {
               free(verticesf_);
               verticesf_ = NULL;
            }
         }
         else
         {
            if (storagetype_)
            {
               void *vbuffer = realloc((void *)vertices_, maxsize_*sizeof(VEC4));
               vertices_ = (VEC4 *)vbuffer;
            }
            else
            {
               void *vbuffer = realloc((void *)verticesf_, maxsize_*sizeof(vec4f));
               verticesf_ = (vec4f *)vbuffer;
            }
         }

         if (maxsize_ == 0 || !hascolor_)
         {
            free(colors_);
            colors_ = NULL;
         }
         else
         {
            void *cbuffer = realloc((void *)colors_, maxsize_*sizeof(vec4f));
            colors_ = (vec4f *)cbuffer;
         }

         if (maxsize_ == 0 || !hasnormal_)
         {
            free(normals_);
            normals_ = NULL;
         }
         else
         {
            void *nbuffer = realloc((void *)normals_, maxsize_*sizeof(vec3f));
            normals_ = (vec3f *)nbuffer;
         }

         if (maxsize_ == 0 || !hastexcoord_)
         {
            free(texcoords_);
            texcoords_ = NULL;
         }
         else
         {
            void *tbuffer = realloc((void *)texcoords_, maxsize_*sizeof(vec4f));
            texcoords_ = (vec4f *)tbuffer;
         }

         if (maxsize_ == 0)
         {
            free(barycentrics_);
            barycentrics_ = NULL;
         }
         else
         {
            void *bbuffer = realloc((void *)barycentrics_, maxsize_*sizeof(vec3f));
            barycentrics_ = (vec3f *)bbuffer;
         }

         if (maxsize_ == 0 || !hasattributes_)
         {
            free(attributes_);
            attributes_ = NULL;
         }
         else
         {
            void *abuffer = realloc((void *)attributes_, maxsize_*LGL_NUM_ATTRIBUTES*sizeof(vec4f));
            attributes_ = (vec4f *)abuffer;
         }
      }
   }

public:

   //! reset all buffers
   void reset()
   {
      size_ = 0;
      hascolor_ = hasnormal_ = hastexcoord_ = false;

      if (hasattributes_)
      {
         for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
            hasattribute_[i] = false;

         hasattributes_ = false;
      }

      copied_ = false;
      copy_ = false;

      bboxmin_ = bboxmax_ = vec3(NAN);

      started_ = false;
      rearrange_ = false;

      modified_ = false;
      rendered_ = false;
      applied_ = false;

      primitives_ = 0;
   }

   //! clear all buffers
   void clear()
   {
      reset();
      shrink();
   }

protected:

   void copyVertex(int index1, int index2)
   {
      if (storagetype_) vertices_[index2] = vertices_[index1];
      else verticesf_[index2] = verticesf_[index1];

      colors_[index2] = colors_[index1];
      normals_[index2] = normals_[index1];
      texcoords_[index2] = texcoords_[index1];
      barycentrics_[index2] = barycentrics_[index1];

      if (hasattributes_)
         for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
            if (hasattribute_[i])
               attributes_[index2*LGL_NUM_ATTRIBUTES+i] = attributes_[index1*LGL_NUM_ATTRIBUTES+i];
   }

   void duplicateVertex(int index)
   {
      if (enlarge())
      {
         copyVertex(index, size_);
         size_++;
      }
   }

   void swapVertex(int index1, int index2)
   {
      if (enlarge())
      {
         copyVertex(index1, size_);
         copyVertex(index2, index1);
         copyVertex(size_, index2);
      }
   }

   void rearrangeQuads()
   {
      if (size_ == 4)
      {
         swapVertex(size_-1, size_-2);

         barycentrics_[size_-4] = vec3f(1,0,0);
         barycentrics_[size_-3] = vec3f(0,1,0);
         barycentrics_[size_-2] = vec3f(0,0,1);
         barycentrics_[size_-1] = vec3f(1,0,0);
      }
      else if (size_ > 4)
      {
         if ((size_-4)%6 == 1)
         {
            duplicateVertex(size_-2);
            swapVertex(size_-2, size_-1);
            size_++;
         }

         if ((size_-4)%6 == 0)
         {
            swapVertex(size_-1, size_-2);

            barycentrics_[size_-4] = vec3f(1,0,0);
            barycentrics_[size_-3] = vec3f(0,1,0);
            barycentrics_[size_-2] = vec3f(0,0,1);
            barycentrics_[size_-1] = vec3f(1,0,0);
         }
      }
   }

public:

   //! specify a vertex in immediate mode fashion (and compile it into vbo)
   void lglVertex(double x, double y, double z, double w = 1)
   {
      lglVertex(vec4(x,y,z,w));
   }

   void lglColor(const vec4f &c)
   {
      if (disable_coloring_)
         return;

      if (immediate_)
         actual_ = color_ = c;
      else
         if (started_)
            color_ = c;
         else
            actual_ = c;

      if (started_)
         hascolor_ = true;
   }

   //! specify the color attribute for the next vertex
   void lglColor(float r, float g, float b, float a = 1)
   {
      lglColor(vec4f(r,g,b,a));
   }

   //! get the actual color attribute
   vec4 lglGetColor() const
   {
      if (started_)
         return(color_);
      else
         return(actual_);
   }

   void lglNormal(const vec3f &n)
   {
      if (disable_lighting_)
         return;

      if (!started_)
         lglError("invalid normal specification");
      else
      {
         normal_ = n;
         hasnormal_ = true;
      }
   }

   //! specify the normal attribute for the next vertex
   void lglNormal(float x, float y, float z)
   {
      lglNormal(vec3f(x,y,z));
   }

   void lglTexCoord(const vec4f &t)
   {
      if (disable_texturing_)
         return;

      if (!started_)
         lglError("invalid texcoord specification");
      else
      {
         texcoord_ = t;
         hastexcoord_ = true;
      }
   }

   //! specify the texture coordinate attribute for the next vertex
   void lglTexCoord(float s, float t = 0, float r = 0, float w = 1)
   {
      lglTexCoord(vec4f(s,t,r,w));
   }

   void lglAttribute(const vec4f &a, unsigned int n = 0)
   {
      if (!started_)
         lglError("invalid attribute specification");
      else
         if (size_ > 0 && !hasattribute_[n])
            lglError("mismatching attribute specification");
         else
            if (n >= LGL_NUM_ATTRIBUTES)
               lglError("invalid attribute number");
            else
            {
               attribute_[n] = a;
               hasattribute_[n] = true;
               hasattributes_ = true;
            }
   }

   //! specify a custom vertex attribute
   void lglAttribute(float x, float y, float z, float w = 1.0f, unsigned int n = 0)
   {
      lglAttribute(vec4f(x,y,z,w), n);
   }

   //! begin a series of vertices for a particular graphics primitive
   void lglBegin(lgl_primitive_enum primitive)
   {
      if (started_)
      {
         lglError("invalid begin operation");
         return;
      }

      if (immediate_)
         reset();

      started_ = true;

#if (LGL_OPENGL_VERSION >= 31) || defined(LGL_GLES)

      // quad strips are not allowed with opengl >= 3.1 or opengl es
      // use identically behaving triangle strips instead
      if (primitive == LGL_QUAD_STRIP)
          primitive = LGL_TRIANGLE_STRIP;

      // quads are not allowed with opengl >= 3.1 or opengl es
      // rearrange incoming vertices as triangle strip instead
      if (primitive == LGL_QUADS)
      {
          primitive = LGL_TRIANGLE_STRIP;

          if (size_ == 0) rearrange_ = true;
          else if (!rearrange_) lglError("unmatched primitives");
      }

#endif

      if (immediate_)
         primitive_ = primitive;
      else
      {
         if (size_ == 0)
            primitive_ = primitive;
         else
         {
            if (primitive != primitive_)
               lglError("unmatched primitives");

            if (primitive_ == LGL_LINE_STRIP)
               lglError("invalid line operation");

            if (primitive_ == LGL_TRIANGLE_FAN)
               lglError("invalid fan operation");

            if (primitive_==LGL_TRIANGLE_STRIP || primitive_==LGL_QUAD_STRIP)
               if (!rearrange_)
               {
                  if (!copied_)
                  {
                     lglPlainVertex(vertex_);
                     if (primitive_ == LGL_QUAD_STRIP) lglPlainVertex(vertex_);
                     copied_ = true;
                  }
                  copy_ = true;
               }
         }

         primitives_++;
      }
   }

   //! end a series of vertices
   void lglEnd()
   {
      if (!started_)
      {
         lglError("invalid end operation");
         return;
      }

      started_ = false;

#ifdef LGL_CORE

      usedprogram_ = 0;

#endif

      if (immediate_)
      {
         lglRender();
         size_ = 0;
      }
   }

   //! specify a series of vertices and attributes as buffers (and copy them into vbo)
   void lglVertexArray(lgl_primitive_enum primitive,
                       unsigned int vertices,
                       const float *vertex_array, // 3 components
                       const float *color_array, // 4 components
                       const float *normal_array = NULL, // 3 components
                       const float *texcoord_array = NULL) // 4 components
   {
      const float *colors = color_array;
      const float *normals = normal_array;
      const float *texcoords = texcoord_array;
      lglBegin(primitive);
      for (unsigned int i=0; i<vertices; i++)
      {
         if (color_array) {lglColor(*colors, *(colors+1), *(colors+2), *(colors+3)); colors+=4;}
         if (normal_array) {lglNormal(*normals, *(normals+1), *(normals+2)); normals+=3;}
         if (texcoord_array) {lglTexCoord(*texcoords, *(texcoords+1), *(texcoords+2), *(texcoords+3)); texcoords+=4;}
         lglVertex(*vertex_array, *(vertex_array+1), *(vertex_array+2)); vertex_array+=3;
      }
      lglEnd();
   }

   //! specify a series of vertices as interleaved buffer (and copy it into vbo)
   void lglInterleavedVertexArray(lgl_primitive_enum primitive,
                                  const float *interleaved_array,
                                  unsigned int vertices,
                                  int colors = 0, // color components (0, 3 or 4)
                                  int normals = 0, // normal components (0 or 3)
                                  int texcoords = 0) // texcoord components (0, 1, 2, 3 or 4)
   {
      const float *array = interleaved_array;
      lglBegin(primitive);
      for (unsigned int i=0; i<vertices; i++)
      {
         vec3 vertex(*array, *(array+1), *(array+2)); array+=3;
         if (colors == 3) {lglColor(*array, *(array+1), *(array+2)); array+=3;}
         else if (colors == 4) {lglColor(*array, *(array+1), *(array+2), *(array+3)); array+=4;}
         if (normals == 3) {lglNormal(*array, *(array+1), *(array+2)); array+=3;}
         if (texcoords == 1) {lglTexCoord(*array); array++;}
         else if (texcoords == 2) {lglTexCoord(*array, *(array+1)); array+=2;}
         else if (texcoords == 3) {lglTexCoord(*array, *(array+1), *(array+2)); array+=3;}
         else if (texcoords == 4) {lglTexCoord(*array, *(array+1), *(array+2), *(array+3)); array+=4;}
         lglVertex(vertex);
      }
      lglEnd();
   }

   //! clear colors
   void lglClearColors()
   {
      hascolor_ = false;
   }

   //! clear normals
   void lglClearNormals()
   {
      hasnormal_ = false;
   }

   //! clear texture coordinates
   void lglClearTexCoords()
   {
      hastexcoord_ = false;
   }

   //! clear attributes
   void lglClearAttributes()
   {
      hasattributes_ = false;

      for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
         hasattribute_[i] = false;
   }

   //! render the series of vertices contained in vbo
   virtual void lglRender(const lgl *vbo = NULL)
   {
      if (started_)
      {
         lglError("invalid render operation");
         return;
      }

      if (!initStaticGL_)
      {
         initializeStaticOpenGL();
         initStaticGL_ = true;
      }

      if (!initGL_)
      {
         initializeOpenGL();
         initGL_ = true;

         // call init-gl hook
         init_gl_hook();
      }

      if (!immediate_)
         shrink();

      if (size_ > 0)
      {
         if (!checkMatrixStacks()) return;

         // view culling
         if (cull_)
            if (size_ >= cull_minsize_)
            {
               mat4 mv = lglGetModelViewMatrix();
               mat4 mvi = mv.invert();
               mat4 mvt = mv.transpose();
               vec3 origin = vec3(0,0,0);
               vec3 direction = vec3(0,0,-1);

               origin = mvi * vec4(origin);
               direction = (mat3(mvt) * direction).normalize();

               if (!glslmath::vtest_plane_bbox(origin, direction, lglGetCenter(), lglGetExtent()))
                  return;
            }

         // copy state and program from vbo
         if (vbo)
         {
            lglCloneState(vbo);

            if (vbo->lglCustomProgram())
               lglCopyProgram(vbo);
            else
               lglCopyUniforms(vbo);
         }

#ifdef LGL_CORE

         GLenum usage = GL_NONE;

         if (immediate_) usage = GL_STREAM_DRAW;
         else usage = GL_STATIC_DRAW;

         bool vao = false;

#if defined(LGL_GL3) || defined(LGL_GLES3)

         // bind vao
         glBindVertexArray(array_);
         vao = true;

#endif

         // update vbo (vertices)
         if (modified_ || !vao)
         {
            glBindBuffer(GL_ARRAY_BUFFER, buffers_[LGL_VERTEX_BUFFER]);
            if (modified_)
            {
               if (storagetype_) glBufferData(GL_ARRAY_BUFFER, size_*sizeof(VEC4), vertices_, usage);
               else glBufferData(GL_ARRAY_BUFFER, size_*sizeof(vec4f), verticesf_, usage);
            }
            glVertexAttribPointer(LGL_VERTEX_LOCATION, 4, storagetype_?gl_type:GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
         }

         // enable vbo (vertices)
         glEnableVertexAttribArray(LGL_VERTEX_LOCATION);

         // update vbo (colors)
         if (hascolor_)
            if (modified_ || !vao)
            {
               glBindBuffer(GL_ARRAY_BUFFER, buffers_[LGL_COLOR_BUFFER]);
               if (modified_) glBufferData(GL_ARRAY_BUFFER, size_*sizeof(vec4f), colors_, usage);
               glVertexAttribPointer(LGL_COLOR_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
               glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

         // enable vbo (colors)
         if (lglActiveColoring())
            glEnableVertexAttribArray(LGL_COLOR_LOCATION);

         // update vbo (normals)
         if (hasnormal_)
            if (modified_ || !vao)
            {
               glBindBuffer(GL_ARRAY_BUFFER, buffers_[LGL_NORMAL_BUFFER]);
               if (modified_) glBufferData(GL_ARRAY_BUFFER, size_*sizeof(vec3f), normals_, usage);
               glVertexAttribPointer(LGL_NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
               glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

         // enable vbo (normals)
         if (lglActiveLighting())
            glEnableVertexAttribArray(LGL_NORMAL_LOCATION);

         // update vbo (texture coordinates)
         if (hastexcoord_)
            if (modified_ || !vao)
            {
               glBindBuffer(GL_ARRAY_BUFFER, buffers_[LGL_TEXCOORD_BUFFER]);
               if (modified_) glBufferData(GL_ARRAY_BUFFER, size_*sizeof(vec4f), texcoords_, usage);
               glVertexAttribPointer(LGL_TEXCOORD_LOCATION, 4, GL_FLOAT, GL_FALSE, 0, 0);
               glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

         // enable vbo (texture coordinates)
         if (lglActiveTexturing())
            glEnableVertexAttribArray(LGL_TEXCOORD_LOCATION);

         // update vbo (barycentric coordinates)
         if (modified_ || !vao)
         {
            glBindBuffer(GL_ARRAY_BUFFER, buffers_[LGL_BARYCENTRIC_BUFFER]);
            if (modified_) glBufferData(GL_ARRAY_BUFFER, size_*sizeof(vec3f), barycentrics_, usage);
            glVertexAttribPointer(LGL_BARYCENTRIC_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
         }

         // enable vbo (barycentric coordinates)
         if (polygonmode_==LGL_LINE || polygonmode_==LGL_BARYCENTRIC)
            glEnableVertexAttribArray(LGL_BARYCENTRIC_LOCATION);

         // enable vbo (generic attributes)
         if (hasattributes_)
         {
            if (modified_ || !vao)
            {
               glBindBuffer(GL_ARRAY_BUFFER, buffers_[LGL_ATTRIBUTE_BUFFER]);
               if (modified_) glBufferData(GL_ARRAY_BUFFER, size_*LGL_NUM_ATTRIBUTES*sizeof(vec4f), attributes_, usage);
               for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
                  if (hasattribute_[i])
                     glVertexAttribPointer(LGL_ATTRIBUTE_LOCATION+i, 4, GL_FLOAT, GL_FALSE, LGL_NUM_ATTRIBUTES*sizeof(vec4f), (void *)(i*sizeof(vec4f)));
               glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
               if (hasattribute_[i])
                  glEnableVertexAttribArray(LGL_ATTRIBUTE_LOCATION+i);
         }

#else

         // enable vertex array
         if (storagetype_) glVertexPointer(4, gl_type, 0, vertices_);
         else glVertexPointer(4, GL_FLOAT, 0, verticesf_);
         glEnableClientState(GL_VERTEX_ARRAY);

         // enable color array
         if (lglActiveColoring())
         {
            glColorPointer(4, GL_FLOAT, 0, colors_);
            glEnableClientState(GL_COLOR_ARRAY);
         }
         else
            glColor4f(actual_.r, actual_.g, actual_.b, actual_.a);

         // enable normal array
         if (lglActiveLighting())
         {
            glNormalPointer(GL_FLOAT, 0, normals_);
            glEnableClientState(GL_NORMAL_ARRAY);
         }

         // enable texture coordinate array
         if (lglActiveTexturing())
         {
            glTexCoordPointer(4, GL_FLOAT, 0, texcoords_);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
         }

#endif

#ifdef LGL_CORE

         // get previous shader program
         GLint prev_program;
         glGetIntegerv(GL_CURRENT_PROGRAM, &prev_program);

         // use shader program
         GLuint program = lglGetActiveProgram();
         glUseProgram(program);

#endif

#ifdef LGL_CORE

         // get actual color
         vec4f color = actual_;

         // check for immediate mode
         if (immediate_)
            if (lglActiveColoring()) color = vec4f(1);

         // get model-view matrix
         mat4 mv = modelview_matrix_.back();

         // get local model matrix
         if (!model_matrix_identity_)
            mv = mv * model_matrix_;

         // get global manip matrix
         if (manip_matrix_apply_ && !manip_matrix_identity_)
            mv = manip_matrix_ * mv;

         // get model-view-projection matrix
         mat4 mvp = projection_matrix_.back() * mv;

         // get inverse transpose model-view matrix
         mat4 mvit = mv.invert().transpose();

         // get texture matrix
         mat4 tm = texture_matrix_.back();

         // get local texture matrix
         if (!tex_matrix_identity_)
            tm = tm * tex_matrix_;

         // get light vector
         vec4f light = light_;

         // update shader locations
         if (program != usedprogram_)
         {
            usedprogram_ = program;

            // get uniform location of actual color
            color_loc_ = glGetUniformLocation(program, "color");

            // get uniform location of model-view-projection matrix
            mvp_loc_ = glGetUniformLocation(program, "mvp");

            // check for presence of model-view-projection matrix
            if (mvp_loc_ < 0)
               lglWarning("uniform location not found: model-view-projection matrix (mvp)");

            // get uniform locations of model-view matrices
            mv_loc_ = glGetUniformLocation(program, "mv");
            mvit_loc_ = glGetUniformLocation(program, "mvit");

            // get uniform location of texture matrix
            tm_loc_ = glGetUniformLocation(program, "tm");

            // get uniform locations of standard program parameters
            if (!lglCustomProgram())
            {
               if (lglActiveLighting())
               {
                  light_loc_ = glGetUniformLocation(program, "light");

                  if (light_loc_ < 0)
                     lglWarning("uniform location not found: light");

                  kaIa_loc_ = glGetUniformLocation(program, "kaIa");

                  if (kaIa_loc_ < 0)
                     lglWarning("uniform location not found: lighting term (kaIa)");

                  kdId_loc_ = glGetUniformLocation(program, "kdId");

                  if (kdId_loc_ < 0)
                     lglWarning("uniform location not found: lighting term (kdId)");

                  ksIs_loc_ = glGetUniformLocation(program, "ksIs");

                  if (ksIs_loc_ < 0)
                     lglWarning("uniform location not found: lighting term (ksIs)");

                  exponent_loc_ = glGetUniformLocation(program, "exponent");

                  if (exponent_loc_ < 0)
                     lglWarning("uniform location not found: lighting term (exponent)");

                  falloff_loc_ = glGetUniformLocation(program, "falloff");

                  if (falloff_loc_ < 0)
                     lglWarning("uniform location not found: lighting term (falloff)");
               }

               if (lglActiveTexturing())
               {
                  sampler_loc_ = glGetUniformLocation(program, "sampler");

                  if (sampler_loc_ < 0)
                     lglWarning("uniform location not found: sampler");
               }

               alphatest_loc_ = glGetUniformLocation(program, "alphatest");

               if (alphatest_loc_ < 0)
                  lglWarning("uniform location not found: alphatest");

               clipping_loc_ = glGetUniformLocation(program, "clipping");

               if (clipping_loc_ < 0)
                  lglWarning("uniform location not found: clipping");

               clipplane_loc_ = glGetUniformLocation(program, "clipplane");

               if (clipplane_loc_ < 0)
                  lglWarning("uniform location not found: clipplane");

               fogdensity_loc_ = glGetUniformLocation(program, "fogdensity");
               fogcolor_loc_ = glGetUniformLocation(program, "fogcolor");

               if (fogdensity_loc_ < 0)
                  lglWarning("uniform location not found: fogdensity");

               if (fogcolor_loc_ < 0)
                  lglWarning("uniform location not found: fogcolor");

               wireframe_loc_ = glGetUniformLocation(program, "wireframe");

               if (wireframe_loc_ < 0)
                  lglWarning("uniform location not found: wireframe");

               interlacing_loc_ = glGetUniformLocation(program, "interlacing");

               if (interlacing_loc_ < 0)
                  lglWarning("uniform location not found: interlacing");
            }
         }

         // set actual color
         if (color_loc_ >= 0) glUniform4fv(color_loc_, 1, (const float *)color);

         // set model-view-projection matrix
         if (mvp_loc_ >= 0) glUniformMatrix4fv(mvp_loc_, 1, GL_FALSE, (const float *)mat4f(mvp));

         // set model-view matrices
         if (mv_loc_ >= 0) glUniformMatrix4fv(mv_loc_, 1, GL_FALSE, (const float *)mat4f(mv));
         if (mvit_loc_ >= 0) glUniformMatrix4fv(mvit_loc_, 1, GL_FALSE, (const float *)mat4f(mvit));

         // set texture matrix
         if (tm_loc_ >= 0) glUniformMatrix4fv(tm_loc_, 1, GL_FALSE, (const float *)mat4f(tm));

         // set program parameters
         if (!lglCustomProgram())
         {
            // set standard lighting parameters
            if (lglActiveLighting())
            {
               glUniform4fv(light_loc_, 1, (const float *)light);

               glUniform3fv(kaIa_loc_, 1, (const float *)vec3f(ka_*Ia_));
               glUniform3fv(kdId_loc_, 1, (const float *)vec3f(kd_*Id_));
               glUniform3fv(ksIs_loc_, 1, (const float *)vec3f(ks_*Is_));

               glUniform1f(exponent_loc_, exponent_);
               glUniform3fv(falloff_loc_, 1, (const float *)vec3f(falloff_));
            }

            // set standard texturing parameters
            if (lglActiveTexturing())
            {
               glUniform1i(sampler_loc_, 0);

               if (texid2D_ != 0)
               {
                  glBindTexture(GL_TEXTURE_2D, texid2D_);
               }
#if !defined(LGL_GLES) || defined (LGL_GLES3)
               else if (texid3D_ != 0)
               {
                  glBindTexture(GL_TEXTURE_3D, texid3D_);
               }
#endif
            }

            // set standard alpha testing parameters
            glUniform3fv(alphatest_loc_, 1, (const float *)vec3f(alphatest_));

            // set standard clipping parameters
            glUniform1f(clipping_loc_, (clipping_>0)?1.0f:0.0f);
            if (clipping_)
               for (unsigned int i=0; i<LGL_NUM_CLIPPLANES; i++)
                  glUniform4fv(clipplane_loc_+i, 1, (const float *)vec4f(clipplane_[i]));

            // set standard fogging parameters
            glUniform1f(fogdensity_loc_, fogdensity_);
            glUniform4fv(fogcolor_loc_, 1, (const float *)vec4f(fogcolor_));

            // set standard polygon mode parameter
            glUniform1f(wireframe_loc_, (polygonmode_==LGL_LINE)?1.0f:0.0f);

            // set standard interlacing parameters
            glUniform4fv(interlacing_loc_, 1, (const float *)setupStereoMode(interlacing_));
         }

         // set custom program parameters
         for (typename lgl_uniform_location_type::const_iterator i = locations_.begin(); i != locations_.end(); i++)
         {
            GLint loc = i->loc;

            if (loc < 0)
            {
               std::string uniform = i->uniform;
               loc = glGetUniformLocation(program, uniform.c_str());
            }

            if (loc < 0)
            {
               if (i->warn)
                  lglWarning("uniform location not found: " + i->uniform);
            }
            else
            {
               switch (i->type)
               {
                  case lgl_int_type: glUniform1i(loc, i->value.int_value); break;
                  case lgl_float_type: glUniform1f(loc, i->value.float_value); break;
                  case lgl_vec2f_type: glUniform2fv(loc, 1, i->value.vec2f_value); break;
                  case lgl_vec3f_type: glUniform3fv(loc, 1, i->value.vec3f_value); break;
                  case lgl_vec4f_type: glUniform4fv(loc, 1, i->value.vec4f_value); break;
                  case lgl_mat2f_type: glUniformMatrix2fv(loc, 1, GL_FALSE, i->value.mat2f_value); break;
                  case lgl_mat3f_type: glUniformMatrix3fv(loc, 1, GL_FALSE, i->value.mat3f_value); break;
                  case lgl_mat4f_type: glUniformMatrix4fv(loc, 1, GL_FALSE, i->value.mat4f_value); break;
                  case lgl_no_type: if (i->warn) lglWarning("using undefined uniform: " + i->uniform); break;
               }
            }
         }

#else

         // get light vector
         vec4f light = light_;

         // set projection matrix
         glMatrixMode(GL_PROJECTION);
         glLoadMatrixf((const float *)mat4f(projection_matrix_.back()));

         // set model-view matrix
         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();
         glLightfv(GL_LIGHT0, GL_POSITION, (const float *)light);
         if (clipping_ > 0)
            for (unsigned int i=0; i<LGL_NUM_CLIPPLANES; i++)
            {
               GLdouble clipplane[4] = {clipplane_[i].x, clipplane_[i].y, clipplane_[i].z, clipplane_[i].w};
               glClipPlane(GL_CLIP_PLANE0+i, clipplane);
            }
         if (model_matrix_identity_)
            if (!manip_matrix_apply_ || manip_matrix_identity_)
               glLoadMatrixf((const float *)mat4f(modelview_matrix_.back()));
            else
               glLoadMatrixf((const float *)mat4f(manip_matrix_ * modelview_matrix_.back()));
         else
            if (!manip_matrix_apply_ || manip_matrix_identity_)
               glLoadMatrixf((const float *)mat4f(modelview_matrix_.back() * model_matrix_));
            else
               glLoadMatrixf((const float *)mat4f(manip_matrix_ * modelview_matrix_.back() * model_matrix_));

         // set texture matrix
         glMatrixMode(GL_TEXTURE);
         if (tex_matrix_identity_)
            glLoadMatrixf((const float *)mat4f(texture_matrix_.back()));
         else
            glLoadMatrixf((const float *)mat4f(texture_matrix_.back() * tex_matrix_));
         glMatrixMode(GL_MODELVIEW);

         // enable lighting
         if (lglActiveLighting())
         {
            glLightfv(GL_LIGHT0, GL_AMBIENT, (const float *)vec4f(ka_*Ia_));
            glLightfv(GL_LIGHT0, GL_DIFFUSE, (const float *)vec4f(kd_*Id_));
            glLightfv(GL_LIGHT0, GL_SPECULAR, (const float *)vec4f(ks_*Is_));
            glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, falloff_.x);
            glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, falloff_.y);
            glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, falloff_.z);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const float *)vec4f(1));
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, exponent_);
            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

            glEnable(GL_COLOR_MATERIAL);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHTING);

            glEnable(GL_NORMALIZE);
         }

         // enable texturing
         if (lglActiveTexturing())
         {
            if (texid2D_ != 0)
            {
               glBindTexture(GL_TEXTURE_2D, texid2D_);
               glEnable(GL_TEXTURE_2D);
            }
            else if (texid3D_ != 0)
            {
               glBindTexture(GL_TEXTURE_3D, texid3D_);
               glEnable(GL_TEXTURE_3D);
            }

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
         }

         // enable clipping
         if (clipping_ > 0)
            for (unsigned int i=0; i<LGL_NUM_CLIPPLANES; i++)
               if (clipplane_[i] != vec4(0,0,0,0))
                   glEnable(GL_CLIP_PLANE0+i);

         // enable fogging
         if (fogdensity_ > 0.0f)
         {
            GLfloat color[4];

            color[0] = fogcolor_.r;
            color[1] = fogcolor_.g;
            color[2] = fogcolor_.b;
            color[3] = fogcolor_.a;

            glFogfv(GL_FOG_COLOR, color);
            glFogi(GL_FOG_MODE, GL_EXP2);
            glFogf(GL_FOG_DENSITY, fogdensity_);

            glEnable(GL_FOG);
         }

         // set polygon mode
         if (polygonmode_ == LGL_LINE)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#endif

         // call pre-render hook
         pre_render_hook();

         // get rendering primitive type
         GLenum p = GL_NONE;
         switch (primitive_)
         {
            case LGL_LINES: p = GL_LINES; break;
            case LGL_LINE_STRIP: p = GL_LINE_STRIP; break;
            case LGL_TRIANGLES: p = GL_TRIANGLES; break;
            case LGL_TRIANGLE_STRIP: p = GL_TRIANGLE_STRIP; break;
            case LGL_TRIANGLE_FAN: p = GL_TRIANGLE_FAN; break;
#if (LGL_OPENGL_VERSION < 31) && !defined(LGL_GLES)
            case LGL_QUADS: p = GL_QUADS; break;
            case LGL_QUAD_STRIP: p = GL_QUAD_STRIP; break;
#endif
            default: break;
         }

         // render primitives
         if (p != GL_NONE)
            glDrawArrays(p, 0, size_);

         // call post-render hook
         post_render_hook();

#ifdef LGL_CORE

         // restore previous program
         glUseProgram(prev_program);

         // reset program state
         if (!lglCustomProgram())
            if (lglActiveTexturing())
            {
               if (texid2D_ != 0)
               {
                  glBindTexture(GL_TEXTURE_2D, 0);
                  glDisable(GL_TEXTURE_2D);
               }
#if !defined(LGL_GLES) || defined(LGL_GLES3)
               else if (texid3D_ != 0)
               {
                  glBindTexture(GL_TEXTURE_3D, 0);
                  glDisable(GL_TEXTURE_3D);
               }
#endif
            }

#else

         // reset lighting
         if (lglActiveLighting())
         {
            glDisable(GL_COLOR_MATERIAL);
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
            glDisable(GL_NORMALIZE);
         }

         // reset texturing
         if (lglActiveTexturing())
         {
            if (texid2D_ != 0)
            {
               glBindTexture(GL_TEXTURE_2D, 0);
               glDisable(GL_TEXTURE_2D);
            }
            else if (texid3D_ != 0)
            {
               glBindTexture(GL_TEXTURE_3D, 0);
               glDisable(GL_TEXTURE_3D);
            }
         }

         // reset clipping
         if (clipping_ > 0)
            for (unsigned int i=0; i<LGL_NUM_CLIPPLANES; i++)
               if (clipplane_[i] != vec4(0,0,0,0))
                  glDisable(GL_CLIP_PLANE0+i);

         // reset fogging
         if (fogdensity_ > 0.0f)
            glDisable(GL_FOG);

         // reset polygon mode
         if (polygonmode_ == LGL_LINE)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#endif

#ifdef LGL_CORE

         // reset vbo (vertices)
         glDisableVertexAttribArray(LGL_VERTEX_LOCATION);

         // reset vbo (colors)
         if (lglActiveColoring())
            glDisableVertexAttribArray(LGL_COLOR_LOCATION);

         // reset vbo (normals)
         if (lglActiveLighting())
            glDisableVertexAttribArray(LGL_NORMAL_LOCATION);

         // reset vbo (texture coordinates)
         if (lglActiveTexturing())
            glDisableVertexAttribArray(LGL_TEXCOORD_LOCATION);

         // reset vbo (barycentric coordinates)
         if (polygonmode_==LGL_LINE || polygonmode_==LGL_BARYCENTRIC)
            glDisableVertexAttribArray(LGL_BARYCENTRIC_LOCATION);

         // reset vbo (generic attributes)
         if (hasattributes_)
            for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
               if (hasattribute_[i])
                  glDisableVertexAttribArray(LGL_ATTRIBUTE_LOCATION+i);

#if defined(LGL_GL3) || defined(LGL_GLES3)

         // reset vao
         glBindVertexArray(0);

#endif

#else

         // reset vertex array
         glDisableClientState(GL_VERTEX_ARRAY);

         // reset color array
         if (lglActiveColoring())
            glDisableClientState(GL_COLOR_ARRAY);

         // reset normal array
         if (lglActiveLighting())
            glDisableClientState(GL_NORMAL_ARRAY);

         // reset texture coordinate array
         if (lglActiveTexturing())
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#endif
      }

      // ray casting
      lglRayCast(this);

      // scene export
      lglExport(this);

      modified_ = false;
      rendered_ = true;
   }

   //! get the primitive mode of vertices contained in vbo
   lgl_primitive_enum lglGetVertexMode() const
   {
      return(primitive_);
   }

   //! get the number of vertices contained in vbo
   int lglGetVertexCount() const
   {
      return(size_);
   }

   //! get the number of primitives contained in vbo
   int lglGetPrimitiveCount() const
   {
      return(primitives_);
   }

   //! get the bounding box of the contained vertices in vbo
   //! * the bounding box is invalid if a maximum component is less than the minimum component
   void lglGetBoundingBox(vec3 &bboxmin, vec3 &bboxmax) const
   {
      bboxmin = bboxmin_;
      bboxmax = bboxmax_;
   }

   //! get the bounding sphere of the contained vertices in vbo
   //! * returns the radius of the bounding sphere
   //! * the bounding sphere is invalid if the radius is zero
   double lglGetBoundingSphere(vec3 &center) const
   {
      center = lglGetCenter();
      return(lglGetRadius());
   }

   //! get the bounding box barycenter of the contained vertices in vbo
   vec3 lglGetCenter() const
   {
      return(0.5*(bboxmin_+bboxmax_));
   }

   //! get the bounding box extent of the contained vertices in vbo
   vec3 lglGetExtent() const
   {
      return(bboxmax_-bboxmin_);
   }

   //! get the bounding sphere radius of the contained vertices in vbo
   //! * zero radius means that the vbo does not contain valid geometry
   double lglGetRadius() const
   {
      if (bboxmin_.x <= bboxmax_.x &&
          bboxmin_.y <= bboxmax_.y &&
          bboxmin_.z <= bboxmax_.z)
         return((0.5*(bboxmax_-bboxmin_)).length());

      return(0);
   }

   //! get the bounding sphere norm of the contained vertices in vbo
   //! * zero norm means that the vbo does not contain valid geometry
   double lglGetNorm() const
   {
      if (bboxmin_.x <= bboxmax_.x &&
          bboxmin_.y <= bboxmax_.y &&
          bboxmin_.z <= bboxmax_.z)
         return((0.5*(bboxmax_-bboxmin_)).norm());

      return(0);
   }

   //! get the maximum extent of the contained vertices in vbo
   //! * zero means that the vbo does not contain valid geometry
   double lglGetMaxExtent() const
   {
      if (bboxmin_.x <= bboxmax_.x &&
          bboxmin_.y <= bboxmax_.y &&
          bboxmin_.z <= bboxmax_.z)
      {
         vec3 ext = bboxmax_-bboxmin_;
         double maxext = ext.x;
         if (ext.y > maxext) maxext = ext.y;
         if (ext.z > maxext) maxext = ext.z;
         return(maxext);
      }

      return(0);
   }

   //! get the maximum absolute value of the coordinates in vbo
   //! * zero means that the vbo does not contain valid geometry
   double lglGetMaxAbsCoord() const
   {
      if (bboxmin_.x <= bboxmax_.x &&
          bboxmin_.y <= bboxmax_.y &&
          bboxmin_.z <= bboxmax_.z)
      {
         double maxabs = bboxmax_.x;
         if (bboxmax_.y > maxabs) maxabs = bboxmax_.y;
         if (bboxmax_.z > maxabs) maxabs = bboxmax_.z;
         if (-bboxmin_.x > maxabs) maxabs = -bboxmin_.x;
         if (-bboxmin_.y > maxabs) maxabs = -bboxmin_.y;
         if (-bboxmin_.z > maxabs) maxabs = -bboxmin_.z;
         return(maxabs);
      }

      return(0);
   }

   //! return a copy of the vertex coordinates in vbo
   std::vector<vec4> lglGetVertexCoordinates() const
   {
      std::vector<vec4> vertices;
      for (int i=0; i<size_; i++) vertices.push_back(vertices_[i]);
      return(vertices);
   }

   //! return a copy of the color attributes in vbo
   std::vector<vec4f> lglGetColorAttributes() const
   {
      std::vector<vec4f> colors;
      if (hascolor_)
         for (int i=0; i<size_; i++) colors.push_back(colors_[i]);
      return(colors);
   }

   //! return a copy of the normal attributes in vbo
   std::vector<vec3f> lglGetNormalAttributes() const
   {
      std::vector<vec3f> normals;
      if (hasnormal_)
         for (int i=0; i<size_; i++) normals.push_back(normals_[i]);
      return(normals);
   }

   //! return a copy of the texture coordinate attributes in vbo
   std::vector<vec4f> lglGetTexCoordAttributes() const
   {
      std::vector<vec4f> texcoords;
      if (hastexcoord_)
         for (int i=0; i<size_; i++) texcoords.push_back(texcoords_[i]);
      return(texcoords);
   }

   //! append a copy of the vertices and attributes in vbo to another vbo
   void lglAppendVerticesTo(lgl *vbo) const
   {
      if (size_ == 0) return;

      if (hascolor_) vbo->hascolor_ = true;

      if (vbo->size_ == 0)
      {
         vbo->primitive_ = primitive_;
         if (hasnormal_) vbo->hasnormal_ = true;
         if (hastexcoord_) vbo->hastexcoord_ = true;
         vbo->primitives_ = primitives_;
      }
      else
      {
         if (primitive_ == LGL_LINE_STRIP ||
             primitive_ == LGL_TRIANGLE_FAN)
         {
            lglError("invalid vbo operation");
            return;
         }

         if (primitive_ != vbo->primitive_ ||
             hasnormal_ != vbo->hasnormal_ ||
             hastexcoord_ != vbo->hastexcoord_ ||
             hasattributes_ || vbo->hasattributes_)
         {
            lglError("unmatched primitives");
            return;
         }

         if (primitive_==LGL_TRIANGLE_STRIP || primitive_==LGL_QUAD_STRIP)
         {
            vbo->lglPlainVertex(vbo->vertices_[vbo->size_-1]);
            if (primitive_ == LGL_QUAD_STRIP)
            {
               vbo->lglPlainVertex(vbo->vertices_[vbo->size_-1]);
               vbo->lglPlainVertex(vertices_[0]);
            }
            vbo->lglPlainVertex(vertices_[0]);
         }

         vbo->primitives_ += primitives_;
      }

      for (int i=0; i<size_; i++)
         vbo->lglPlainVertex(vertices_[i],
                             hascolor_?colors_[i]:vec4f(1),
                             hastexcoord_?texcoords_[i]:vec4f(0),
                             hasnormal_?normals_[i]:vec3f(0));
   }

   //! determine the actual matrix mode
   void lglMatrixMode(lgl_matrixmode_enum mode = LGL_MODELVIEW)
   {
      matrixmode_ = mode;
   }

   //! get the actual matrix mode
   lgl_matrixmode_enum lglGetMatrixMode() const
   {
      return(matrixmode_);
   }

   //! load the identity matrix in immediate mode fashion
   void lglLoadIdentity()
   {
      if (!checkVertexScope()) return;
      if (!checkMatrixStacks()) return;

      if (matrixmode_ == LGL_MODELVIEW)
         modelview_matrix_.back() = mat4(1);
      else if (matrixmode_ == LGL_PREMODEL)
      {
         premodel_matrix_.back() = mat4(1);
         premodel_matrix_identity_.back() = true;
         premodel_matrix_it_.back() = mat3(1);
         premodel_matrix_recompute_.back() = false;
      }
      else if (matrixmode_ == LGL_PROJECTION)
         projection_matrix_.back() = mat4(1);
      else if (matrixmode_ == LGL_TEXTURE)
         texture_matrix_.back() = mat4(1);
      else
      {
         pretex_matrix_.back() = mat4(1);
         pretex_matrix_identity_.back() = true;
      }
   }

   //! load a specific matrix in immediate mode fashion
   void lglLoadMatrix(const mat4 &matrix)
   {
      if (!checkVertexScope()) return;
      if (!checkMatrixStacks()) return;

      if (matrixmode_ == LGL_MODELVIEW)
         modelview_matrix_.back() = matrix;
      else if (matrixmode_ == LGL_PREMODEL)
      {
         premodel_matrix_.back() = matrix;
         premodel_matrix_identity_.back() = false;
         premodel_matrix_recompute_.back() = true;
      }
      else if (matrixmode_ == LGL_PROJECTION)
         projection_matrix_.back() = matrix;
      else if (matrixmode_ == LGL_TEXTURE)
         texture_matrix_.back() = matrix;
      else
      {
         pretex_matrix_.back() = matrix;
         pretex_matrix_identity_.back() = false;
      }
   }

   //! multiply a specific matrix in immediate mode fashion
   void lglMultMatrix(const mat4 &matrix)
   {
      if (!checkVertexScope()) return;
      if (!checkMatrixStacks()) return;

      if (matrixmode_ == LGL_MODELVIEW)
         modelview_matrix_.back() = modelview_matrix_.back() * matrix;
      else if (matrixmode_ == LGL_PREMODEL)
      {
         premodel_matrix_.back() = premodel_matrix_.back() * matrix;
         premodel_matrix_identity_.back() = false;
         premodel_matrix_recompute_.back() = true;
      }
      else if (matrixmode_ == LGL_PROJECTION)
         projection_matrix_.back() = projection_matrix_.back() * matrix;
      else  if (matrixmode_ == LGL_TEXTURE)
         texture_matrix_.back() = texture_matrix_.back() * matrix;
      else
      {
         pretex_matrix_.back() = pretex_matrix_.back() * matrix;
         pretex_matrix_identity_.back() = false;
      }
   }

   //! push the top entry of the matrix stack
   void lglPushMatrix()
   {
      if (!checkVertexScope()) return;
      if (!checkMatrixStacks()) return;

      if (matrixmode_ == LGL_MODELVIEW)
         modelview_matrix_.push_back(modelview_matrix_.back());
      else if (matrixmode_ == LGL_PREMODEL)
      {
         premodel_matrix_.push_back(premodel_matrix_.back());
         premodel_matrix_identity_.push_back(premodel_matrix_identity_.back());
         premodel_matrix_it_.push_back(premodel_matrix_it_.back());
         premodel_matrix_recompute_.push_back(premodel_matrix_recompute_.back());
      }
      else if (matrixmode_ == LGL_PROJECTION)
         projection_matrix_.push_back(projection_matrix_.back());
      else  if (matrixmode_ == LGL_TEXTURE)
         texture_matrix_.push_back(texture_matrix_.back());
      else
      {
         pretex_matrix_.push_back(pretex_matrix_.back());
         pretex_matrix_identity_.push_back(pretex_matrix_identity_.back());
      }
   }

   //! pop the top entry of the matrix stack
   void lglPopMatrix()
   {
      if (!checkVertexScope()) return;
      if (!checkMatrixStacks()) return;

      if (matrixmode_ == LGL_MODELVIEW)
         modelview_matrix_.pop_back();
      else if (matrixmode_ == LGL_PREMODEL)
      {
         premodel_matrix_.pop_back();
         premodel_matrix_identity_.pop_back();
         premodel_matrix_it_.pop_back();
         premodel_matrix_recompute_.pop_back();
      }
      else if (matrixmode_ == LGL_PROJECTION)
         projection_matrix_.pop_back();
      else if (matrixmode_ == LGL_TEXTURE)
         texture_matrix_.pop_back();
      else
      {
         pretex_matrix_.pop_back();
         pretex_matrix_identity_.pop_back();
      }

      checkMatrixStacks();
   }

   //! get the top entry of the matrix stack
   mat4 lglGetMatrix() const
   {
      if (!checkMatrixStacks()) return(mat4(1));

      if (matrixmode_ == LGL_MODELVIEW)
         return(modelview_matrix_.back());
      else if (matrixmode_ == LGL_PREMODEL)
         return(premodel_matrix_.back());
      else if (matrixmode_ == LGL_PROJECTION)
         return(projection_matrix_.back());
      else if (matrixmode_ == LGL_TEXTURE)
         return(texture_matrix_.back());
      else
         return(pretex_matrix_.back());
   }

   //! get the actual projection matrix
   mat4 lglGetProjectionMatrix() const
   {
      if (!checkMatrixStacks()) return(mat4(1));

      // get projection matrix
      mat4 p = projection_matrix_.back();

      return(p);
   }

   //! get the actual modelview matrix
   mat4 lglGetModelViewMatrix() const
   {
      if (!checkMatrixStacks()) return(mat4(1));

      // get model-view matrix
      mat4 mv = modelview_matrix_.back();

      // get local model matrix
      if (!model_matrix_identity_)
         mv = mv * model_matrix_;

      // get global manip matrix
      if (manip_matrix_apply_ && !manip_matrix_identity_)
         mv = manip_matrix_ * mv;

      return(mv);
   }

   //! get the inverse of the actual modelview matrix
   mat4 lglGetInverseModelViewMatrix() const
   {
      // invert model-view matrix
      return(lglGetModelViewMatrix().invert());
   }

   //! get the inverse transpose of the actual modelview matrix
   mat4 lglGetInverseTransposeModelViewMatrix() const
   {
      // invert and transpose model-view matrix
      return(lglGetModelViewMatrix().invert().transpose());
   }

   //! get the combined modelview and projection matrix
   mat4 lglGetModelViewProjectionMatrix() const
   {
      // multiply model-view and projection matrix
      return(lglGetProjectionMatrix() * lglGetModelViewMatrix());
   }

   //! get the actual texture matrix
   mat4 lglGetTextureMatrix() const
   {
      if (!checkMatrixStacks()) return(mat4(1));

      // get texture matrix
      mat4 tm = texture_matrix_.back();

      return(tm);
   }

   //! scale in immediate mode fashion
   void lglScale(const vec4 &c)
   {
      if (c != vec4(1,1,1,1))
         lglMultMatrix(mat4::scale(c));
   }

   //! scale in immediate mode fashion
   void lglScale(double s, double t, double r, double w = 1)
   {
      lglScale(vec4(s,t,r,w));
   }

   //! scale in immediate mode fashion
   void lglScale(double s, double w = 1)
   {
      lglScale(vec4(s,s,s,w));
   }

   //! translate in immediate mode fashion
   void lglTranslate(const vec4 &v)
   {
      if (v != vec4(0,0,0,0))
         lglMultMatrix(mat4::translate(v));
   }

   //! translate in immediate mode fashion
   void lglTranslate(double x, double y, double z, double w = 1)
   {
      lglTranslate(vec4(x,y,z,w));
   }

   //! rotate in immediate mode fashion
   void lglRotate(double angle, const vec3 &v)
   {
      if (angle != 0)
         lglMultMatrix(mat4::rotate(angle, v));
   }

   //! rotate in immediate mode fashion
   void lglRotate(double angle, double vx, double vy, double vz)
   {
      lglRotate(angle, vec3(vx,vy,vz));
   }

   //! rotate about x-axis in immediate mode fashion
   void lglRotateX(double angle)
   {
      lglRotate(angle, vec3(1,0,0));
   }

   //! rotate about y-axis in immediate mode fashion
   void lglRotateY(double angle)
   {
      lglRotate(angle, vec3(0,1,0));
   }

   //! rotate about z-axis in immediate mode fashion
   void lglRotateZ(double angle)
   {
      lglRotate(angle, vec3(0,0,1));
   }

   //! orthographic projection
   void lglOrtho(double left, double right, double bottom, double top, double nearp = -1, double farp = 1)
   {
      lglMultMatrix(mat4::ortho(left, right, bottom, top, nearp, farp));
   }

   //! frustum projection
   void lglFrustum(double left, double right, double bottom, double top, double nearp, double farp)
   {
      lglMultMatrix(mat4::frustum(left, right, bottom, top, nearp, farp));
   }

   //! parallel projection
   void lglParallel(const vec3 &p,const vec3 &n,const vec3 &d)
   {
      lglMultMatrix(mat4::parallel(p, n, d));
   }

   //! perspective projection
   void lglPerspective(double fovy, double aspect, double nearp, double farp)
   {
      fovy_ = fovy;
      aspect_ = aspect;
      nearp_ = nearp;
      farp_ = farp;

      lglMultMatrix(mat4::perspective(fovy, aspect, nearp, farp));
   }

   //! get fovy parameter for last projection transformation
   double getFovy()
   {
      return(fovy_);
   }

   //! get aspect parameter for last projection transformation
   double getAspect()
   {
      return(aspect_);
   }

   //! get near parameter for last projection transformation
   double getNear()
   {
      return(nearp_);
   }

   //! get far parameter for last projection transformation
   double getFar()
   {
      return(farp_);
   }

   //! lookat transformation
   void lglLookAt(const vec3 &eye, const vec3 &at, const vec3 &up = vec3(0,1,0))
   {
      eye_ = eye;
      at_ = at;
      up_ = up;

      lglMultMatrix(mat4::lookat(eye, at, up));
   }

   //! lookat transformation
   void lglLookAt(double eye_x, double eye_y, double eye_z,
                  double at_x, double at_y, double at_z,
                  double up_x = 0, double up_y = 1, double up_z = 0)
   {
      lglLookAt(vec3(eye_x,eye_y,eye_z), vec3(at_x,at_y,at_z), vec3(up_x,up_y,up_z));
   }

   //! get eye point for last lookat transformation
   vec3 getEye() const
   {
      return(eye_);
   }

   //! get lookat point for last lookat transformation
   vec3 getLookAt() const
   {
      return(at_);
   }

   //! get up vector for last lookat transformation
   vec3 getUp() const
   {
      return(up_);
   }

   //! reset the actual projection matrix
   void lglProjection()
   {
      lgl_matrixmode_enum mode = matrixmode_;
      matrixmode_ = LGL_PROJECTION;
      lglLoadIdentity();
      matrixmode_ = mode;
   }

   //! set the actual projection matrix
   void lglProjection(const mat4 &projection)
   {
      lgl_matrixmode_enum mode = matrixmode_;
      matrixmode_ = LGL_PROJECTION;
      lglLoadMatrix(projection);
      matrixmode_ = mode;
   }

   //! set the actual projection matrix
   void lglProjection(double left, double right, double bottom, double top, double nearp, double farp)
   {
      lglProjection(mat4::ortho(left, right, bottom, top, nearp, farp));
   }

   //! set the actual projection matrix
   void lglProjection(double fovy, double aspect, double nearp, double farp)
   {
      fovy_ = fovy;
      aspect_ = aspect;
      nearp_ = nearp;
      farp_ = farp;

      lglProjection(mat4::perspective(fovy, aspect, nearp, farp));
   }

   //! reset the actual modelview matrix
   void lglModelView()
   {
      lgl_matrixmode_enum mode = matrixmode_;
      matrixmode_ = LGL_MODELVIEW;
      lglLoadIdentity();
      matrixmode_ = mode;
   }

   //! set the actual modelview matrix
   void lglModelView(const mat4 &modelview)
   {
      lgl_matrixmode_enum mode = matrixmode_;
      matrixmode_ = LGL_MODELVIEW;
      lglLoadMatrix(modelview);
      matrixmode_ = mode;
   }

   //! set the actual modelview matrix
   void lglView(const vec3 &eye, const vec3 &at, const vec3 &up = vec3(0,1,0))
   {
      eye_ = eye;
      at_ = at;
      up_ = up;

      lglModelView(mat4::lookat(eye, at, up));
   }

   //! set the actual modelview matrix
   void lglView(double eye_x, double eye_y, double eye_z,
                double at_x, double at_y, double at_z,
                double up_x = 0, double up_y = 1, double up_z = 0)
   {
      lglView(vec3(eye_x,eye_y,eye_z), vec3(at_x,at_y,at_z), vec3(up_x,up_y,up_z));
   }

   //! reset the actual texture matrix
   void lglTexture()
   {
      lgl_matrixmode_enum mode = matrixmode_;
      matrixmode_ = LGL_TEXTURE;
      lglLoadIdentity();
      matrixmode_ = mode;
   }

   //! set the actual texture matrix
   void lglTexture(const mat4 &texture)
   {
      lgl_matrixmode_enum mode = matrixmode_;
      matrixmode_ = LGL_TEXTURE;
      lglLoadMatrix(texture);
      matrixmode_ = mode;
   }

   //! enable or disable the manipulator matrix
   static void lglManip(bool on = true)
   {
      manip_matrix_apply_ = on;
   }

   //! update the manipulator matrix by multiplying it with a custom matrix
   static void lglManip(const mat4 &manip)
   {
      manip_matrix_ = manip * manip_matrix_;
      manip_matrix_identity_ = false;
      manip_matrix_apply_ = true;
   }

   //! update the manipulator matrix by applying a rotation about a focus point and a zoom factor
   static void lglManip(double focus, double angle, double tilt, double zoom)
   {
      mat4 rotate = mat4::rotate(-tilt, vec3(1,0,0)) * mat4::rotate(-angle, vec3(0,1,0));
      mat4 scale = mat4::scale(zoom);
      lglManip(mat4::translate(vec3(0,0,-focus)) * scale * rotate * mat4::translate(vec3(0,0,focus)));
   }

   //! update the manipulator matrix by applying a screen-space delta
   static void lglManip(double dx, double dy, double zoom = 1)
   {
      double fovy = fovy_;
      double aspect = aspect_;

      if (fovy == 0) fovy_ = 360;
      if (aspect == 0) aspect_ = 1;

      mat4 scale = mat4::scale(zoom);

      if (eye_==at_ || up_==vec3(0))
      {
         mat4 rotate = mat4::rotate(-dy*fovy, vec3(1,0,0)) * mat4::rotate(-dx*fovy*aspect, vec3(0,1,0));
         lglManip(scale * rotate);
         manip_matrix_apply_ = false;
      }
      else
      {
         double focus = (at_-eye_).length();
         mat4 v = mat4::lookat(eye_, at_, up_) * mat4::translate(at_);
         mat4 rotatex = mat4::rotate(-dx*fovy*aspect, up_);
         mat4 rotatey = mat4::rotate(-dy*fovy, vec3(1,0,0));
         lglManip(mat4::translate(vec3(0,0,-focus)) * scale * rotatey * mat4::translate(vec3(0,0,focus)));
         lglManip(v * rotatex * v.invert());
      }
   }

   //! update the manipulator matrix by applying a screen-space delta
   static void lglManip(vec2 delta, double zoom = 1)
   {
      lglManip(delta.x, delta.y, zoom);
   }

   //! reset the manipulator matrix
   static void lglResetManip()
   {
      manip_matrix_ = mat4(1);
      manip_matrix_identity_ = true;
      manip_matrix_apply_ = false;
   }

   //! get the manipulator matrix
   static mat4 lglGetManip()
   {
      return(manip_matrix_);
   }

   //! is the manipulator matrix applied?
   static bool lglIsManipApplied()
   {
      return(manip_matrix_apply_);
   }

   //! get the inverse transpose of the manipulator matrix
   static mat4 lglGetInverseTransposeManip()
   {
      return(manip_matrix_.invert().transpose());
   }

   //! reset the vbo modeling matrix
   void lglModel()
   {
      model_matrix_ = mat4(1);
      model_matrix_identity_ = true;
   }

   //! specify the vbo modeling matrix
   void lglModel(const mat4 &model)
   {
      model_matrix_ = model;
      model_matrix_identity_ = false;
   }

   //! get the vbo modeling matrix
   mat4 lglGetModelMatrix() const
   {
      return(model_matrix_);
   }

   //! get the inverse transpose of the vbo modeling matrix
   mat4 lglGetInverseTransposeModelMatrix() const
   {
      return(model_matrix_.invert().transpose());
   }

   //! apply the vbo modeling matrix to the vbo and reset the matrix
   void lglApplyModelMatrix()
   {
      if (started_)
      {
         lglError("invalid vbo operation");
         return;
      }

      if (size_ > 0)
      {
         mat4 m = lglGetModelMatrix();
         mat3 mit = lglGetInverseTransposeModelMatrix();

         for (int i=0; i<size_; i++)
         {
            if (hasnormal_) normals_[i] = mit * normals_[i];
            vertices_[i] = m * vertices_[i];
         }

         modified_ = true;

         lglModel();
      }
   }

   //! reset the vbo texturing matrix
   void lglTex()
   {
      tex_matrix_ = mat4(1);
      tex_matrix_identity_ = true;
   }

   //! specify the vbo texturing matrix
   void lglTex(const mat4 &tex)
   {
      tex_matrix_ = tex;
      tex_matrix_identity_ = false;
   }

   //! specify the vbo texturing matrix
   void lglTex(const vec4 &scale, const vec4 &offset)
   {
      lglTex(mat4::translate(offset) * mat4::scale(scale));
   }

   //! get the vbo texturing matrix
   mat4 lglGetTexMatrix() const
   {
      return(tex_matrix_);
   }

   //! apply the vbo texturing matrix to the vbo and reset the matrix
   void lglApplyTexMatrix()
   {
      if (started_)
      {
         lglError("invalid vbo operation");
         return;
      }

      if (size_ > 0)
      {
         mat4 m = lglGetTexMatrix();

         for (int i=0; i<size_; i++)
            if (hastexcoord_) texcoords_[i] = m * texcoords_[i];

         modified_ = true;

         lglTex();
      }
   }

   //! add a color channel to the vbo
   //! * if a color channel is already present the color is modulated
   void lglApplyColor(const vec4f &c)
   {
      if (started_)
      {
         lglError("invalid vbo operation");
         return;
      }

      if (size_ > 0)
      {
         hascolor_ = true;

         for (int i=0; i<size_; i++)
            colors_[i] *= c;

         modified_ = true;
      }

      applied_ = true;
   }

   //! check for GLSL support
   static bool lglSupportsGLSL()
   {
#ifdef LGL_CORE
      return(true);
#else
      return(false);
#endif
   }

   //! get GLSL version string
   static std::string lglGetGLSLVersionString()
   {
#ifdef LGL_CORE

      const GLubyte *version = glGetString(GL_SHADING_LANGUAGE_VERSION);
      return((const char *)version);

#else

      return("none");

#endif
   }

   //! get the LGL plain GLSL vertex shader
   static std::string lglPlainGLSLVertexShader()
   {
      return(std::string(lgl_plain_vertex_shader_header)+
             lgl_plain_vertex_shader);
   }

   //! get the LGL plain GLSL fragment shader
   static std::string lglPlainGLSLFragmentShader()
   {
      return(std::string(lgl_plain_fragment_shader_header)+
             lgl_plain_fragment_shader);
   }

   //! get the combined LGL plain GLSL program
   static std::string lglPlainGLSLProgram()
   {
      return(lglCombineGLSLProgram(lglPlainGLSLVertexShader(), lglPlainGLSLFragmentShader()));
   }

   //! get the LGL default vertex shader
   static std::string lglBasicGLSLVertexShader(bool colors = false, bool normals = false, bool texcoords = false)
   {
      if (!texcoords)
         if (!normals)
            if (!colors)
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader1);
            else
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader2);
         else
            if (!colors)
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_lighting+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader3);
            else
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_lighting+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader4);
      else
         if (!normals)
            if (!colors)
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader5);
            else
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader6);
         else
            if (!colors)
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_lighting+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader7);
            else
               return(std::string(lgl_default_vertex_shader_header)+
                      lgl_default_vertex_shader_lighting+
                      lgl_default_vertex_shader_clipping+
                      lgl_default_vertex_shader8);
   }

   //! get the LGL default fragment shader
   static std::string lglBasicGLSLFragmentShader(bool colors = false, bool normals = false, bool texcoords = false, bool tex3D = false)
   {
      if (!texcoords)
         if (!normals)
            if (!colors)
               return(std::string(lgl_default_fragment_shader_header)+
                      lgl_default_fragment_shader_alphatesting+
                      lgl_default_fragment_shader_clipping+
                      lgl_default_fragment_shader_interlacing+
                      lgl_default_fragment_shader1);
            else
               return(std::string(lgl_default_fragment_shader_header)+
                      lgl_default_fragment_shader_alphatesting+
                      lgl_default_fragment_shader_clipping+
                      lgl_default_fragment_shader_interlacing+
                      lgl_default_fragment_shader2);
         else
            if (!colors)
               return(std::string(lgl_default_fragment_shader_header)+
                      lgl_default_fragment_shader_lighting+
                      lgl_default_fragment_shader_alphatesting+
                      lgl_default_fragment_shader_clipping+
                      lgl_default_fragment_shader_interlacing+
                      lgl_default_fragment_shader3);
            else
               return(std::string(lgl_default_fragment_shader_header)+
                      lgl_default_fragment_shader_lighting+
                      lgl_default_fragment_shader_alphatesting+
                      lgl_default_fragment_shader_clipping+
                      lgl_default_fragment_shader_interlacing+
                      lgl_default_fragment_shader4);
      else
         if (!tex3D)
            if (!normals)
               if (!colors)
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader5_2D);
               else
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader6_2D);
            else
               if (!colors)
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_lighting+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader7_2D);
               else
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_lighting+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader8_2D);
         else
            if (!normals)
               if (!colors)
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader5_3D);
               else
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader6_3D);
            else
               if (!colors)
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_lighting+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader7_3D);
               else
                  return(std::string(lgl_default_fragment_shader_header)+
                         lgl_default_fragment_shader_lighting+
                         lgl_default_fragment_shader_alphatesting+
                         lgl_default_fragment_shader_clipping+
                         lgl_default_fragment_shader_interlacing+
                         lgl_default_fragment_shader8_3D);
   }

protected:

   static void tweakShader(std::string &shader)
   {
#ifdef LGL_GLES

      lgl_string::stringReplaceAll(shader, "#version 120", "#version 100");
      lgl_string::stringReplaceAll(shader, ".0f", ".0");

#endif
   }

public:

   //! compile GLSL vertex shader
   static GLuint lglCompileGLSLVertexShader(std::string shader)
   {
#ifdef LGL_CORE

#ifdef _WIN32
      initWGLprocs();
#endif

      tweakShader(shader);

      GLint length = shader.size();
      GLuint shader_id = glCreateShader(GL_VERTEX_SHADER);

      const char *source = shader.c_str();
      glShaderSource(shader_id, 1, &source, &length);
      glCompileShader(shader_id);

      GLint status;
      glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

      if (status == GL_FALSE)
      {
         GLint infolength;
         glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infolength);
         char *info = new char[infolength+1];
         glGetShaderInfoLog(shader_id, infolength, NULL, info);
         lglError("GLSL vertex shader:");
         lglError(source);
         lglError("GLSL vertex shader errors: " + std::string(info));
         delete[] info;

         glDeleteShader(shader_id);
         shader_id = 0;
      }

      return(shader_id);

#else

      lglError("vertex programs unsupported");
      return(0);

#endif
   }

   //! compile GLSL fragment shader
   static GLuint lglCompileGLSLFragmentShader(std::string shader)
   {
#ifdef LGL_CORE

#ifdef _WIN32
      initWGLprocs();
#endif

      tweakShader(shader);

      GLint length = shader.size();
      GLuint shader_id = glCreateShader(GL_FRAGMENT_SHADER);

      const char *source = shader.c_str();
      glShaderSource(shader_id, 1, &source, &length);
      glCompileShader(shader_id);

      GLint status;
      glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

      if (status == GL_FALSE)
      {
         GLint infolength;
         glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infolength);
         char *info = new char[infolength+1];
         glGetShaderInfoLog(shader_id, infolength, NULL, info);
         lglError("GLSL fragment shader:");
         lglError(source);
         lglError("GLSL fragment shader errors: " + std::string(info));
         delete[] info;

         glDeleteShader(shader_id);
         shader_id = 0;
      }

      return(shader_id);

#else

      lglError("fragment programs unsupported");
      return(0);

#endif
   }

   //! link GLSL program
   static GLuint lglLinkGLSLProgram(GLuint vertex_shader_id, GLuint fragment_shader_id, GLuint custom_shader_program = 0)
   {
#ifdef LGL_CORE

#ifdef _WIN32
      initWGLprocs();
#endif

      GLuint shader_program = custom_shader_program;

      if (vertex_shader_id!=0 && fragment_shader_id!=0)
      {
         if (shader_program == 0)
         {
            shader_program = glCreateProgram();

            glBindAttribLocation(shader_program, LGL_VERTEX_LOCATION, "vertex_position");
            glBindAttribLocation(shader_program, LGL_COLOR_LOCATION, "vertex_color");
            glBindAttribLocation(shader_program, LGL_NORMAL_LOCATION, "vertex_normal");
            glBindAttribLocation(shader_program, LGL_TEXCOORD_LOCATION, "vertex_texcoord");
            glBindAttribLocation(shader_program, LGL_BARYCENTRIC_LOCATION, "vertex_barycentric");
         }

         for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
         {
            std::ostringstream vertex_attribute;
            vertex_attribute << "vertex_attribute" << i;

            glBindAttribLocation(shader_program, LGL_ATTRIBUTE_LOCATION+i, vertex_attribute.str().c_str());
         }

         glAttachShader(shader_program, vertex_shader_id);
         glAttachShader(shader_program, fragment_shader_id);

         glLinkProgram(shader_program);

         GLint status;
         glGetProgramiv(shader_program, GL_LINK_STATUS, &status);

         if (status == GL_FALSE)
         {
            GLint infolength;
            glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &infolength);
            char *info = new char[infolength+1];
            glGetProgramInfoLog(shader_program, infolength, NULL, info);
            lglError("GLSL linker errors: " + std::string(info));
            delete[] info;

            glDeleteProgram(shader_program);
            shader_program = 0;
         }
         else
         {
            glDetachShader(shader_program, vertex_shader_id);
            glDetachShader(shader_program, fragment_shader_id);
         }
      }

      return(shader_program);

#else

      return(0);

#endif
   }

   //! compile a GLSL program from a vertex and a fragment shader source
   static GLuint lglCompileGLSLProgram(std::string vertex_shader, std::string fragment_shader)
   {
#ifdef LGL_CORE

      GLuint vertex_shader_id = lglCompileGLSLVertexShader(vertex_shader);

      if (vertex_shader_id != 0)
      {
         GLuint fragment_shader_id = lglCompileGLSLFragmentShader(fragment_shader);

         if (fragment_shader_id != 0)
         {
            GLuint shader_program = lglLinkGLSLProgram(vertex_shader_id, fragment_shader_id);

            lglDeleteGLSLShader(vertex_shader_id);
            lglDeleteGLSLShader(fragment_shader_id);

            return(shader_program);
         }
         else
            lglDeleteGLSLShader(vertex_shader_id);
      }

      return(0);

#else

      return(0);

#endif
   }

   //! compile a GLSL program from a vertex and a fragment shader source file
   static GLuint lglLoadGLSLProgram(const char *vertex_shader_file, const char *fragment_shader_file)
   {
#ifdef LGL_CORE

      GLuint program = 0;

      std::string vertex_shader = lglReadTextFile(vertex_shader_file);
      std::string fragment_shader = lglReadTextFile(fragment_shader_file);

      if (!vertex_shader.empty() && !fragment_shader.empty())
         program = lgl::lglCompileGLSLProgram(vertex_shader, fragment_shader);
      else
         lglError("unable to load file");

      return(program);

#else

      return(0);

#endif
   }

   //! combine GLSL program
   static std::string lglCombineGLSLProgram(const std::string &vertex_shader, const std::string &fragment_shader)
   {
      return(vertex_shader+"---\n"+fragment_shader);
   }

   //! split GLSL program
   static bool lglSplitGLSLProgram(std::string shader,
                                   std::string &vertex_shader, std::string &fragment_shader)
   {
      size_t pos = shader.find("---");

      if (pos < shader.size())
      {
         size_t pos1 = pos;

         while (shader[pos1]=='-' && pos1>0) pos1--;

         while ((shader[pos1]==' ' || shader[pos1]=='\t' ||
                 shader[pos1]=='\n' || shader[pos1]=='\r') && pos1>0) pos1--;

         vertex_shader = shader.substr(0, pos1+1);

         if (vertex_shader.size() == 0)
            return(false);

         vertex_shader += "\n";

         size_t pos2 = pos;

         while (shader[pos2]=='-' && pos2+1<shader.size()) pos2++;

         while ((shader[pos2]==' ' || shader[pos2]=='\t' ||
                 shader[pos2]=='\n' || shader[pos2]=='\r') && pos2+1<shader.size()) pos2++;

         fragment_shader = shader.substr(pos2);

         if (fragment_shader.size() == 0)
            return(false);

         fragment_shader += "\n";

         return(true);
      }

      return(false);
   }

   //! compile a GLSL program from a combined vertex and fragment shader source
   static GLuint lglCompileGLSLProgram(std::string shader)
   {
      std::string vertex_shader;
      std::string fragment_shader;

      if (lglSplitGLSLProgram(shader, vertex_shader, fragment_shader))
         return(lglCompileGLSLProgram(vertex_shader, fragment_shader));

      return(0);
   }

   //! compile a GLSL program from a combined vertex and fragment shader source file
   static GLuint lglLoadGLSLProgram(const char *shader_file)
   {
#ifdef LGL_CORE

      GLuint program = 0;

      std::string shader = lglReadTextFile(shader_file);

      if (!shader.empty())
         program = lgl::lglCompileGLSLProgram(shader);
      else
         lglError("unable to load file");

      return(program);

#else

      return(0);

#endif
   }

   //! delete a compiled GLSL shader
   static void lglDeleteGLSLShader(GLuint shader_id)
   {
#ifdef LGL_CORE

#ifdef _WIN32
      initWGLprocs();
#endif

      if (shader_id != 0)
         glDeleteShader(shader_id);

#endif
   }

   //! delete a compiled GLSL program
   static void lglDeleteGLSLProgram(GLuint program)
   {
#ifdef LGL_CORE

#ifdef _WIN32
      initWGLprocs();
#endif

      if (program != 0)
         glDeleteProgram(program);

#endif
   }

   //! use a custom GLSL program
   void lglUseProgram(GLuint program, bool clear = true)
   {
#ifdef LGL_CORE

      if (program == 0)
         lglError("invalid GLSL program");

      if (program != program_)
      {
         program_ = program;

         if (clear)
         {
            uniforms_.clear();
            locations_.clear();
         }
      }

#endif
   }

   //! use the default GLSL program which simulates the fixed function OpenGL pipeline
   void lglUseDefaultProgram(bool clear = true)
   {
#ifdef LGL_CORE

      if (program_ != 0)
      {
         program_ = 0;
         usedprogram_ = 0;

         if (clear)
         {
            uniforms_.clear();
            locations_.clear();
         }
      }

#endif
   }

   //! replace default GLSL program
   void lglReplaceDefaultProgram(GLuint program, bool colors = false, bool normals = true, bool texcoords = false, bool tex3D = false)
   {
#ifdef LGL_CORE

      int num = 0;

      if (colors) num |= 1;
      if (normals) num |= 2;
      if (texcoords) num |= 4;

      if (texcoords && tex3D) num += 4;

      custom_programs_[num] = program;

      usedprogram_ = 0;

#endif
   }

   //! get the actual custom GLSL program in use
   GLuint lglGetProgram() const
   {
#ifdef LGL_CORE

      return(program_);

#else

      return(0);

#endif
   }

   //! reuse a compiled GLSL program /wo clearing and updating uniforms
   void lglReuseProgram(GLuint program)
   {
#ifdef LGL_CORE

      if (program != 0)
         lglUseProgram(program, false);
      else
         lglUseDefaultProgram(false);

#endif
   }

   //! check whether or not a custom GLSL program is in use
   bool lglCustomProgram() const
   {
#ifdef LGL_CORE

      if (program_>0 && polygonmode_!=LGL_LINE)
         return(true);

#endif

      return(false);
   }

   //! get the actual GLSL program in use
   GLuint lglGetActiveProgram() const
   {
#ifdef LGL_CORE

      GLuint program;

      if (lglCustomProgram())
         program = program_;
      else
      {
         int num = 0;

         if (lglActiveColoring()) num |= 1;
         if (lglActiveLighting()) num |= 2;
         if (lglActiveTexturing()) num |= 4;

         if (lglActiveTexturing() && texid3D_!=0) num += 4;

         if (custom_programs_[num] != 0)
            program = custom_programs_[num];
         else
            program = programs_[num];
      }

      return(program);

#else

      return(0);

#endif
   }

   //! check for active vertex color attributes
   bool lglActiveColoring() const
   {
      return(hascolor_ && coloring_ && !disable_coloring_);
   }

   //! check for applied vertex color attributes
   bool lglAppliedColoring() const
   {
      return(hascolor_ && coloring_ && !disable_coloring_ && applied_);
   }

   //! check for active vertex normal attributes
   bool lglActiveLighting() const
   {
      return(hasnormal_ && lighting_ && !disable_lighting_);
   }

   //! check for active vertex texture coordinate attributes
   bool lglActiveTexturing() const
   {
      if (!hastexcoord_ || !texturing_ || disable_texturing_)
         return(false);

#ifdef LGL_CORE

      if (lglCustomProgram())
         return(true);

#endif

      if (texid2D_!=0 || texid3D_!=0)
         return(true);

      return(false);
   }

   //! enable or disable the vertex color attributes contained in vbo
   void lglColoring(bool on)
   {
      if (on != coloring_)
      {
         coloring_ = on;

#ifdef LGL_CORE

         usedprogram_ = 0;

#endif
      }
   }

   //! check for coloring
   bool lglGetColoring() const
   {
      return(coloring_);
   }

   //! enable or disable the vertex normal attributes contained in vbo
   void lglLighting(bool on)
   {
      if (on != lighting_)
      {
         lighting_ = on;

#ifdef LGL_CORE

         usedprogram_ = 0;

#endif
      }
   }

   //! check for lighting
   bool lglGetLighting() const
   {
      return(lighting_);
   }

   //! enable or disable the vertex texture coordinate attributes contained in vbo
   void lglTexturing(bool on)
   {
      if (on != texturing_)
      {
         texturing_ = on;

#ifdef LGL_CORE

         usedprogram_ = 0;

#endif
      }
   }

   //! check for texturing
   bool lglGetTexturing() const
   {
      return(texturing_);
   }

   //! entirely disable vertex colors
   static void lglDisableColoring(bool off)
   {
      disable_coloring_ = off;
   }

   //! entirely disable vertex normals
   static void lglDisableLighting(bool off)
   {
      disable_lighting_ = off;
   }

   //! entirely disable texture coordinates
   static void lglDisableTexturing(bool off)
   {
      disable_texturing_ = off;
   }

   //! specify a uniform for the actual compiled GLSL program (undefined)
   unsigned int lglUniform(std::string uniform, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_no_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (integer)
   unsigned int lglUniformi(std::string uniform, int value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_int_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformi(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (integer, indexed)
   void lglUniformi(unsigned int index, int value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_int_type;
            locations_[index].value.int_value = value;
         }
         else if (locations_[index].type == lgl_int_type)
            locations_[index].value.int_value = value;
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (float)
   unsigned int lglUniformf(std::string uniform, double value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_float_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformf(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (float, indexed)
   void lglUniformf(unsigned int index, double value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_float_type;
            locations_[index].value.float_value = float(value);
         }
         else if (locations_[index].type == lgl_float_type)
            locations_[index].value.float_value = float(value);
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (vec2f)
   unsigned int lglUniformfv(std::string uniform, const vec2f &value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_vec2f_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformfv(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (float[2])
   unsigned int lglUniform2fv(std::string uniform, const float value[2], bool warn = true)
   {
      return(lglUniformfv(uniform, vec2f(value[0], value[1]), warn));
   }

   //! specify a uniform for the actual compiled GLSL program (vec2f, indexed)
   void lglUniformfv(unsigned int index, const vec2f &value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_vec2f_type;
            for (int n=0; n<2; n++)
               locations_[index].value.vec2f_value[n] = ((const float *)value)[n];
         }
         else if (locations_[index].type == lgl_vec2f_type)
            for (int n=0; n<2; n++)
               locations_[index].value.vec2f_value[n] = ((const float *)value)[n];
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   unsigned int lglUniformfv(std::string uniform, const vec2 &value, bool warn = true)
      {return(lglUniformfv(uniform, vec2f(value), warn));}

   void lglUniformfv(unsigned int index, const vec2 &value, bool warn = true)
      {lglUniformfv(index, vec2f(value), warn);}

   //! specify a uniform for the actual compiled GLSL program (vec3f)
   unsigned int lglUniformfv(std::string uniform, const vec3f &value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_vec3f_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformfv(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (float[3])
   unsigned int lglUniform3fv(std::string uniform, const float value[3], bool warn = true)
   {
      return(lglUniformfv(uniform, vec3f(value[0], value[1], value[2]), warn));
   }

   //! specify a uniform for the actual compiled GLSL program (vec3f, indexed)
   void lglUniformfv(unsigned int index, const vec3f &value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_vec3f_type;
            for (int n=0; n<3; n++)
               locations_[index].value.vec3f_value[n] = ((const float *)value)[n];
         }
         else if (locations_[index].type == lgl_vec3f_type)
            for (int n=0; n<3; n++)
               locations_[index].value.vec3f_value[n] = ((const float *)value)[n];
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   unsigned int lglUniformfv(std::string uniform, const vec3 &value, bool warn = true)
      {return(lglUniformfv(uniform, vec3f(value), warn));}

   void lglUniformfv(unsigned int index, const vec3 &value, bool warn = true)
      {lglUniformfv(index, vec3f(value), warn);}

   //! specify a uniform for the actual compiled GLSL program (vec4f)
   unsigned int lglUniformfv(std::string uniform, const vec4f &value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_vec4f_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformfv(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform for the actual compiled GLSL program (float[4])
   unsigned int lglUniform4fv(std::string uniform, const float value[4], bool warn = true)
   {
      return(lglUniformfv(uniform, vec4f(value[0], value[1], value[2], value[3]), warn));
   }

   //! specify a uniform for the actual compiled GLSL program (vec4f, indexed)
   void lglUniformfv(unsigned int index, const vec4f &value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_vec4f_type;
            for (int n=0; n<4; n++)
               locations_[index].value.vec4f_value[n] = ((const float *)value)[n];
         }
         else if (locations_[index].type == lgl_vec4f_type)
            for (int n=0; n<4; n++)
               locations_[index].value.vec4f_value[n] = ((const float *)value)[n];
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   unsigned int lglUniformfv(std::string uniform, const vec4 &value, bool warn = true)
      {return(lglUniformfv(uniform, vec4f(value), warn));}

   void lglUniformfv(unsigned int index, const vec4 &value, bool warn = true)
      {lglUniformfv(index, vec4f(value), warn);}

   unsigned int lglUniformf(std::string uniform, double x, double y, double z, double w = 1, bool warn = true)
      {return(lglUniformfv(uniform, vec4(x,y,z,w), warn));}

   void lglUniformf(unsigned int index, double x, double y, double z, double w = 1, bool warn = true)
      {lglUniformfv(index, vec4(x,y,z,w), warn);}

   //! specify a uniform for the actual compiled GLSL program (mat2f)
   unsigned int lglUniformfv(std::string uniform, const mat2f &value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_mat2f_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformfv(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform matrix for the actual compiled GLSL program (float[4])
   unsigned int lglUniformMatrix2fv(std::string uniform, const float value[4], bool warn = true)
   {
      mat2f matrix;
      matrix.fromOpenGL(value);
      return(lglUniformfv(uniform, matrix, warn));
   }

   //! specify a uniform matrix for the actual compiled GLSL program (mat2f, indexed)
   void lglUniformfv(unsigned int index, const mat2f &value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_mat2f_type;
            for (int n=0; n<4; n++)
               locations_[index].value.mat2f_value[n] = ((const float *)value)[n];
         }
         else if (locations_[index].type == lgl_mat2f_type)
            for (int n=0; n<4; n++)
               locations_[index].value.mat2f_value[n] = ((const float *)value)[n];
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   unsigned int lglUniformfv(std::string uniform, const mat2 &value, bool warn = true)
      {return(lglUniformfv(uniform, mat2f(value), warn));}

   void lglUniformfv(unsigned int index, const mat2 &value, bool warn = true)
      {lglUniformfv(index, mat2f(value), warn);}

   //! specify a uniform for the actual compiled GLSL program (mat3f)
   unsigned int lglUniformfv(std::string uniform, const mat3f &value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_mat3f_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformfv(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform matrix for the actual compiled GLSL program (float[9])
   unsigned int lglUniformMatrix3fv(std::string uniform, const float value[9], bool warn = true)
   {
      mat3f matrix;
      matrix.fromOpenGL(value);
      return(lglUniformfv(uniform, matrix, warn));
   }

   //! specify a uniform matrix for the actual compiled GLSL program (mat3f, indexed)
   void lglUniformfv(unsigned int index, const mat3f &value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_mat3f_type;
            for (int n=0; n<9; n++)
               locations_[index].value.mat3f_value[n] = ((const float *)value)[n];
         }
         else if (locations_[index].type == lgl_mat3f_type)
            for (int n=0; n<9; n++)
               locations_[index].value.mat3f_value[n] = ((const float *)value)[n];
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   unsigned int lglUniformfv(std::string uniform, const mat3 &value, bool warn = true)
      {return(lglUniformfv(uniform, mat3f(value), warn));}

   void lglUniformfv(unsigned int index, const mat3 &value, bool warn = true)
      {lglUniformfv(index, mat3f(value), warn);}

   //! specify a uniform for the actual compiled GLSL program (mat4f)
   unsigned int lglUniformfv(std::string uniform, const mat4f &value, bool warn = true)
   {
#ifdef LGL_CORE

      lgl_uniform_map_type::iterator i = uniforms_.find(uniform);

      if (i == uniforms_.end())
      {
         i = uniforms_.insert(std::pair<std::string, unsigned int>(uniform, locations_.size())).first;

         lgl_uniform_struct u = {-1, uniform, lgl_mat4f_type, {0}, warn};
         locations_.resize(locations_.size()+1, u);
      }

      lglUniformfv(i->second, value, warn);

      return(i->second);

#else

      return(0);

#endif
   }

   //! specify a uniform matrix for the actual compiled GLSL program (float[16])
   unsigned int lglUniformMatrix4fv(std::string uniform, const float value[16], bool warn = true)
   {
      mat4f matrix;
      matrix.fromOpenGL(value);
      return(lglUniformfv(uniform, matrix, warn));
   }

   //! specify a uniform matrix for the actual compiled GLSL program (mat4f, indexed)
   void lglUniformfv(unsigned int index, const mat4f &value, bool warn = true)
   {
#ifdef LGL_CORE

      if (index < locations_.size())
      {
         if (locations_[index].type == lgl_no_type)
         {
            locations_[index].type = lgl_mat4f_type;
            for (int n=0; n<16; n++)
               locations_[index].value.mat4f_value[n] = ((const float *)value)[n];
         }
         else if (locations_[index].type == lgl_mat4f_type)
            for (int n=0; n<16; n++)
               locations_[index].value.mat4f_value[n] = ((const float *)value)[n];
         else
            lglError("uniform type mismatch");
      }
      else
         lglError("invalid uniform index");

#endif
   }

   unsigned int lglUniformfv(std::string uniform, const mat4 &value, bool warn = true)
      {return(lglUniformfv(uniform, mat4f(value), warn));}

   void lglUniformfv(unsigned int index, const mat4 &value, bool warn = true)
      {lglUniformfv(index, mat4f(value), warn);}

   //! specify a uniform sampler for the actual compiled GLSL program
   unsigned int lglSampler(std::string sampler, int value = 0, bool warn = true)
   {
      return(lglUniformi(sampler, value, warn));
   }

   //! specify a uniform sampler for the actual compiled GLSL program (indexed)
   void lglSampler(unsigned int index, int value = 0, bool warn = true)
   {
      lglUniformi(index, value, warn);
   }

   //! bind a 2D sampler for the actual compiled GLSL program
   unsigned int lglSampler2D(std::string sampler, GLuint texid2D, int value = 0, bool warn = true)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

      unsigned int index = lglUniformi(sampler, value, warn);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB+value);
#endif
      glBindTexture(GL_TEXTURE_2D, texid2D);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB);
#endif
      return(index);
   }

   //! bind a 2D sampler for the actual compiled GLSL program (indexed)
   void lglSampler2D(unsigned int index, GLuint texid2D, int value = 0, bool warn = true)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

      lglUniformi(index, value, warn);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB+value);
#endif
      glBindTexture(GL_TEXTURE_2D, texid2D);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB);
#endif
   }

   //! bind a 3D sampler for the actual compiled GLSL program
   unsigned int lglSampler3D(std::string sampler, GLuint texid3D, int value = 0, bool warn = true)
   {
#if !defined(LGL_GLES) || defined(LGL_GLES3)

#ifdef _WIN32
      initWGLprocs();
#endif

      unsigned int index = lglUniformi(sampler, value, warn);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB+value);
#endif
      glBindTexture(GL_TEXTURE_3D, texid3D);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB);
#endif
      return(index);

#else

      return(0);

#endif
   }

   //! bind a 3D sampler for the actual compiled GLSL program (indexed)
   void lglSampler3D(unsigned int index, GLuint texid3D, int value = 0, bool warn = true)
   {
#if !defined(LGL_GLES) || defined(LGL_GLES3)

#ifdef _WIN32
      initWGLprocs();
#endif

      lglUniformi(index, value, warn);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB+value);
#endif
      glBindTexture(GL_TEXTURE_3D, texid3D);
#ifdef GL_ARB_multitexture
      if (value != 0) glActiveTexture(GL_TEXTURE0_ARB);
#endif

#endif
   }

   //! clone uniforms from vbo
   void lglCloneUniforms(const lgl *vbo)
   {
#ifdef LGL_CORE

      uniforms_ = vbo->uniforms_;
      locations_ = vbo->locations_;

#endif
   }

   //! copy uniforms from vbo
   void lglCopyUniforms(const lgl *vbo, bool warn = false)
   {
#ifdef LGL_CORE

      for (typename lgl_uniform_map_type::const_iterator i = vbo->uniforms_.begin(); i != vbo->uniforms_.end(); i++)
      {
         std::string uniform = i->first;
         unsigned int index = i->second;

         switch (vbo->locations_[index].type)
         {
            case lgl_int_type: lglUniformi(uniform, vbo->locations_[index].value.int_value, warn); break;
            case lgl_float_type: lglUniformf(uniform, vbo->locations_[index].value.float_value, warn); break;
            case lgl_vec2f_type: lglUniform2fv(uniform, vbo->locations_[index].value.vec2f_value, warn); break;
            case lgl_vec3f_type: lglUniform3fv(uniform, vbo->locations_[index].value.vec3f_value, warn); break;
            case lgl_vec4f_type: lglUniform4fv(uniform, vbo->locations_[index].value.vec4f_value, warn); break;
            case lgl_mat2f_type: lglUniformMatrix2fv(uniform, vbo->locations_[index].value.mat2f_value, warn); break;
            case lgl_mat3f_type: lglUniformMatrix3fv(uniform, vbo->locations_[index].value.mat3f_value, warn); break;
            case lgl_mat4f_type: lglUniformMatrix4fv(uniform, vbo->locations_[index].value.mat4f_value, warn); break;
            case lgl_no_type: if (vbo->locations_[index].warn) lglWarning("copying undefined uniform: " + uniform); break;
         }
   }

#endif
   }

   //! print GLSL uniforms
   void lglPrintUniforms()
   {
#ifdef LGL_CORE

      for (typename lgl_uniform_location_type::const_iterator i = locations_.begin(); i != locations_.end(); i++)
      {
         std::cout << "uniform \"" << i->uniform << "\": ";

         switch (i->type)
         {
            case lgl_int_type: std::cout << i->value.int_value; break;
            case lgl_float_type: std::cout << i->value.float_value; break;
            case lgl_vec2f_type: std::cout << vec2f(i->value.vec2f_value[0], i->value.vec2f_value[1]); break;
            case lgl_vec3f_type: std::cout << vec3f(i->value.vec3f_value[0], i->value.vec3f_value[1], i->value.vec3f_value[2]); break;
            case lgl_vec4f_type: std::cout << vec4f(i->value.vec4f_value[0], i->value.vec4f_value[1], i->value.vec4f_value[2], i->value.vec4f_value[3]); break;
            case lgl_mat2f_type: {mat2f m2f; m2f.fromOpenGL(i->value.mat2f_value); std::cout << m2f;} break;
            case lgl_mat3f_type: {mat3f m3f; m3f.fromOpenGL(i->value.mat3f_value); std::cout << m3f;} break;
            case lgl_mat4f_type: {mat4f m4f; m4f.fromOpenGL(i->value.mat4f_value); std::cout << m4f;} break;
            case lgl_no_type: std::cout << "undefined"; break;
         }

         std::cout << std::endl;
      }

#endif
   }

   //! clone GLSL program and uniforms from vbo
   void lglCloneProgram(const lgl *vbo)
   {
#ifdef LGL_CORE

      program_ = vbo->program_;
      uniforms_ = vbo->uniforms_;
      locations_ = vbo->locations_;

#endif
   }

   //! copy GLSL program and uniforms from vbo
   void lglCopyProgram(const lgl *vbo)
   {
#ifdef LGL_CORE

      program_ = vbo->program_;
      lglCopyUniforms(vbo);

#endif
   }

   //! specify the lighting parameters as supported by the default GLSL program
   void lglLight(vec4f light = vec4f(0,0,1,0), bool camera_light = true,
                 vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                 vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                 float exponent = 30, vec3f falloff = vec3f(1,0,0))
   {
      lglLightVector(light, camera_light);
      lglLightParameters(ka, kd, ks, Ia, Id, Is, exponent, falloff);
   }

   //! set the light direction
   void lglLightDirection(vec3f light = vec3f(0,0,1), bool camera_light = true)
   {
      lglLightVector(vec4f(light, 0), camera_light);
   }

   //! set the light position
   void lglLightPosition(vec3f light = vec3f(0,0,0), bool camera_light = true)
   {
      lglLightVector(vec4f(light, 1), camera_light);
   }

   //! set the light vector
   void lglLightVector(vec4f light = vec4f(0,0,1,0), bool camera_light = true)
   {
      if (camera_light)
         light_ = light;
      else
      {
         if (light.w == 0)
         {
            // directional light vector
            mat4 mvit = lglGetInverseTransposeModelViewMatrix();
            light_ = vec4f((mat3(mvit) * light.xyz()).normalize(), 0);
         }
         else
         {
            // positional light vector
            mat4 mv = lglGetModelViewMatrix();
            light_ = mv * light;
         }
      }

      if (light_.w == 0)
         falloff_ = vec3f(1,0,0);
   }

   //! get the light vector (in camera coordinates)
   vec4 lglGetLightVector() const
   {
      return(light_);
   }

   //! set the light parameters
   void lglLightParameters(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                           vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                           float exponent = 30, vec3f falloff = vec3f(1,0,0))
   {
      ka_ = ka;
      kd_ = kd;
      ks_ = ks;

      Ia_ = Ia;
      Id_ = Id;
      Is_ = Is;

      exponent_ = exponent;
      falloff_ = falloff;

      if (light_.w == 0)
         falloff_ = vec3f(1,0,0);
   }

   //! get the light parameters
   void lglGetLightParameters(vec3f &ka, vec3f &kd, vec3f &ks,
                              vec3f &Ia, vec3f &Id, vec3f &Is,
                              float &exponent, vec3f &falloff) const
   {
      ka = ka_;
      kd = kd_;
      ks = ks_;

      Ia = Ia_;
      Id = Id_;
      Is = Is_;

      exponent = exponent_;
      falloff = falloff_;
   }

   //! set the light source parameters
   void lglLightSourceParameters(vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                                 vec3f falloff = vec3f(1,0,0))
   {
      Ia_ = Ia;
      Id_ = Id;
      Is_ = Is;

      falloff_ = falloff;

      if (light_.w == 0)
         falloff_ = vec3f(1,0,0);
   }

   //! get the light source parameters
   void lglGetLightSourceParameters(vec3f &Ia, vec3f &Id, vec3f &Is,
                                    vec3f &falloff) const
   {
      Ia = Ia_;
      Id = Id_;
      Is = Is_;

      falloff = falloff_;
   }

   //! set the material parameters
   void lglMaterialParameters(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                              float exponent = 30)
   {
      ka_ = ka;
      kd_ = kd;
      ks_ = ks;

      exponent_ = exponent;
   }

   //! get the material parameters
   void lglGetMaterialParameters(vec3f &ka, vec3f &kd, vec3f &ks,
                                 float &exponent) const
   {
      ka = ka_;
      kd = kd_;
      ks = ks_;

      exponent = exponent_;
   }

   //! bind a 2D texture for the default GLSL program
   void lglTexture2D(GLuint texid)
   {
      texid2D_ = texid;
      if (texid != 0) texid3D_ = 0;
   }

   //! get the bound 2D texture
   GLuint lglGetTexture2D() const
   {
      return(texid2D_);
   }

   //! bind a 3D texture for the default GLSL program
   void lglTexture3D(GLuint texid)
   {
      texid3D_ = texid;
      if (texid != 0) texid2D_ = 0;
   }

   //! get the bound 3D texture
   GLuint lglGetTexture3D() const
   {
      return(texid3D_);
   }

   //! enable texture coordinate generation
   void lglTexCoordGen(lgl_texgenmode_enum mode = LGL_TEXGEN_LINEAR,
                       double mix = 0.5)
   {
      texgen_ = mode;
      texgen_mix_ = mix;
   }

   //! check whether or not texture coordinate generation is enabled
   lgl_texgenmode_enum lglGetTexCoordGen()
   {
      return(texgen_);
   }

   //! clone state from vbo (coloring, lighting and texturing)
   void lglCloneState(const lgl *vbo)
   {
      vec3f ka, kd, ks;
      float exponent;

      lglColoring(vbo->lglGetColoring());

      lglLighting(vbo->lglGetLighting());
      vbo->lglGetMaterialParameters(ka, kd, ks, exponent);
      lglMaterialParameters(ka, kd, ks, exponent);

      lglTexturing(vbo->lglGetTexturing());
      lglTexture2D(vbo->lglGetTexture2D());
      lglTexture3D(vbo->lglGetTexture3D());
   }

   //! initialize the OpenGL state (clear color, depth test, back-face culling)
   static void lglInitializeOpenGL(float r=0, float g=0, float b=0, float a=1,
                                   bool ztest = true, bool culling = false)
   {
      lglClearColor(r,g,b,a);
      lglDepthTest(ztest);
      lglBackFaceCulling(culling);
   }

   //! set the viewport
   static void lglViewport(int ax, int ay, int bx, int by)
   {
      glViewport(ax, ay, bx, by);
   }

   static void lglClearColor(float w, float a=1)
   {
      glClearColor(w,w,w,a);
   }

   //! set the clear color
   static void lglClearColor(float r, float g, float b, float a=1)
   {
      glClearColor(r,g,b,a);
   }

   //! clear the color and depth buffer
   static void lglClear(GLuint bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
   {
      glClear(bits);
   }

   //! specify the RGBA write mask
   static void lglRGBAWrite(bool r = true, bool g = true, bool b = true, bool a = true)
   {
      // specify color mask
      glColorMask(r?GL_TRUE:GL_FALSE,
                  g?GL_TRUE:GL_FALSE,
                  b?GL_TRUE:GL_FALSE,
                  a?GL_TRUE:GL_FALSE);
   }

   //! get the RGBA write mask
   static bool lglGetRGBAWrite()
   {
      GLboolean color_write[4];
      glGetBooleanv(GL_DEPTH_WRITEMASK, color_write);
      return(color_write[0]!=0 && color_write[1]!=0 && color_write[2]!=0 && color_write[3]!=0);
   }

   //! specify the depth write mask
   static void lglZWrite(bool on = true)
   {
      if (on)
      {
         // enable Z writing
         glDepthMask(GL_TRUE);
      }
      else
      {
         // disable Z writing
         glDepthMask(GL_FALSE);
      }
   }

   //! get the depth write mask
   static bool lglGetZWrite()
   {
      GLboolean depth_write;
      glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_write);
      return(depth_write!=0);
   }

   //! enable or disable depth testing
   static void lglDepthTest(bool on = true)
   {
      if (on)
      {
         // enable Z test
         glEnable(GL_DEPTH_TEST);
      }
      else
      {
         // disable Z test
         glDisable(GL_DEPTH_TEST);
      }
   }

   //! check whether or not depth testing is enabled
   static bool lglGetDepthTest()
   {
      GLboolean depth_test;
      glGetBooleanv(GL_DEPTH_TEST, &depth_test);
      return(depth_test!=0);
   }

   //! enable or disable back-face culling
   static void lglBackFaceCulling(bool on = true)
   {
      if (on)
      {
         // enable back-face culling
         glFrontFace(GL_CCW); // front faces are defined counter-clockwise
         glCullFace(GL_BACK); // cull back faces
         glEnable(GL_CULL_FACE); // enable culling
      }
      else
      {
         // disable culling
         glDisable(GL_CULL_FACE);
      }
   }

   //! check whether or not back-face culling is enabled
   static bool lglGetBackFaceCulling()
   {
      GLboolean cull_face;
      glGetBooleanv(GL_CULL_FACE, &cull_face);
      return(cull_face!=0);
   }

   //! enable or disable blending
   static void lglBlendMode(lgl_blendmode_enum mode = LGL_BLEND_NONE)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

      blendmode_ = mode;

      if (mode == LGL_BLEND_NONE)
      {
         // disable blending
         glDisable(GL_BLEND);
      }
      else if (mode == LGL_BLEND_MULT)
      {
         // enable multiplicative blending
         glBlendFunc(GL_DST_COLOR, GL_ZERO);
#ifdef _WIN32
         if (glBlendEquation) glBlendEquation(GL_FUNC_ADD);
#else
         glBlendEquation(GL_FUNC_ADD);
#endif
         glEnable(GL_BLEND);
      }
      else if (mode == LGL_BLEND_ALPHA)
      {
         // enable alpha blending
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef _WIN32
         if (glBlendEquation) glBlendEquation(GL_FUNC_ADD);
#else
         glBlendEquation(GL_FUNC_ADD);
#endif
         glEnable(GL_BLEND);
      }
      else if (mode == LGL_BLEND_ADD)
      {
         // enable additive blending
         glBlendFunc(GL_ONE, GL_ONE);
#ifdef _WIN32
         if (glBlendEquation) glBlendEquation(GL_FUNC_ADD);
#else
         glBlendEquation(GL_FUNC_ADD);
#endif
         glEnable(GL_BLEND);
      }
      else if (mode == LGL_BLEND_SUB)
      {
         // enable subtractive blending
         glBlendFunc(GL_ONE, GL_ONE);
#ifdef _WIN32
         if (glBlendEquation) glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
#else
         glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
#endif
         glEnable(GL_BLEND);
      }
      else if (mode == LGL_BLEND_MAX)
      {
         // maximum intensity mode
         glBlendFunc(GL_ONE,GL_ONE);
#ifdef _WIN32
         if (glBlendEquation) glBlendEquation(GL_MAX);
#else
         glBlendEquation(GL_MAX);
#endif
         glEnable(GL_BLEND);
      }
      else if (mode == LGL_BLEND_MIN)
      {
         // minimum intensity mode
         glBlendFunc(GL_ONE,GL_ONE);
#ifdef _WIN32
         if (glBlendEquation) glBlendEquation(GL_MIN);
#else
         glBlendEquation(GL_MIN);
#endif
         glEnable(GL_BLEND);
      }
   }

   //! check whether or not blending is enabled
   static bool lglGetBlending()
   {
      GLboolean blend;
      glGetBooleanv(GL_BLEND, &blend);
      return(blend!=0);
   }

   //! get the actual blending mode
   static lgl_blendmode_enum lglGetBlendMode()
   {
      return(blendmode_);
   }

   //! enable or disable alpha testing
   static void lglAlphaTest(bool on = false, float value = 0.0f, bool greater = true, bool equal = false)
   {
      alphavalue_ = value;

#ifndef LGL_CORE

      if (on)
      {
         // enable alpha test
         if (greater) glAlphaFunc(equal?GL_GEQUAL:GL_GREATER, value);
         else glAlphaFunc(equal?GL_LEQUAL:GL_LESS, value);
         glEnable(GL_ALPHA_TEST);
      }
      else
      {
         // disable alpha test
         glDisable(GL_ALPHA_TEST);
      }

#endif

      // setup shader test
      if (on)
         if (greater)
            if (equal)
               alphatest_ = vec3f(value, -1, 1); // discard alpha < value
            else
               alphatest_ = vec3f(value, value, 1); // discard alpha <= value
         else
            if (equal)
               alphatest_ = vec3f(-value, 1, -1); // discard -alpha < -value
            else
               alphatest_ = vec3f(-value, -value, -1); // discard -alpha <= -value
      else
         alphatest_ = vec3f(-1,-1,0); // discard none
   }

   //! check whether or not alpha testing is enabled
   static bool lglGetAlphaTest()
   {
#ifndef LGL_CORE

      GLboolean alpha_test;
      glGetBooleanv(GL_ALPHA_TEST, &alpha_test);
      return(alpha_test != 0);

#else

      return(alphatest_.z != 0);

#endif
   }

   //! get the actual alpha test comparison value
   static float lglGetAlphaTestValue()
   {
      return(alphavalue_);
   }

   //! get the actual alpha test comparison mode
   static bool lglGetAlphaTestGreater()
   {
      return(alphatest_.z > 0);
   }

   //! get the actual alpha test equality mode
   static bool lglGetAlphaTestEqual()
   {
      return(alphatest_.x != alphatest_.y);
   }

   //! specify a clip plane as supported by the default GLSL program
   void lglClipPlane(vec4 equation = vec4(0,0,0,0), unsigned int n = 0, bool camera_plane = false)
   {
      if (n < LGL_NUM_CLIPPLANES)
      {
         if (equation != vec4(0,0,0,0))
         {
            if (clipplane_[n] == vec4(0,0,0,0)) clipping_++;
            if (!camera_plane)
            {
               mat4 mvit = lglGetInverseTransposeModelViewMatrix();
               equation = mvit * equation;
            }
            clipplane_[n] = equation;
         }
         else
         {
            if (clipplane_[n] != vec4(0,0,0,0)) clipping_--;
            clipplane_[n] = vec4(0,0,0,0);
         }
      }
   }

   //! specify a clip plane as supported by the default GLSL program
   void lglClipPlane(double a, double b, double c, double d, unsigned int n = 0, bool camera_plane = false)
   {
      lglClipPlane(vec4(a,b,c,d), n, camera_plane);
   }

   //! specify a clip plane by a point on the plane and a plane normal
   void lglClipPlane(vec3 point, vec3 normal, unsigned int n = 0, bool camera_plane = false)
   {
      lglClipPlane(lglGetClipPlaneEquation(point, normal), n, camera_plane);
   }

   //! check whether or not a clip plane is enabled
   static bool lglGetClipPlane(unsigned int n = 0)
   {
      if (n < LGL_NUM_CLIPPLANES)
         return(clipplane_[n]!=vec4(0,0,0,0));
      else
         return(false);
   }

   //! get the actual clip plane equation
   static vec4 lglGetClipPlaneEquation(unsigned int n = 0)
   {
      if (n < LGL_NUM_CLIPPLANES)
         return(clipplane_[n]);
      else
         return(vec4(0,0,0,0));
   }

   //! compute a clip plane equation from a point and a normal
   static vec4 lglGetClipPlaneEquation(vec3 point, vec3 normal)
   {
      // normalize clip plane normal
      normal = normal.normalize();

      // define clip plane equation via point on plane and normal
      return(vec4(normal.x, normal.y, normal.z, -normal.dot(point)));
   }

   //! specify the fog parameters as supported by the default GLSL program
   static void lglFog(float density = 0.0f, vec4f color = vec4f(1))
   {
      fogdensity_ = density;
      fogcolor_ = color;
   }

   //! check whether or not fogging is enabled
   static bool lglGetFog()
   {
      return(fogdensity_ > 0.0f);
   }

   //! get the actual fog density
   static float lglGetFogDensity()
   {
      return(fogdensity_);
   }

   //! get the actual fog color
   static vec4f lglGetFogColor()
   {
      return(fogcolor_);
   }

   //! specify the line rasterization width
   static void lglLineWidth(float width = 1.0f)
   {
      glLineWidth(width);
   }

   //! get the line rasterization width
   static float lglGetLineWidth()
   {
      GLfloat line_width;
      glGetFloatv(GL_LINE_WIDTH, &line_width);
      return(line_width);
   }

   //! specify the polygon mode
   static void lglPolygonMode(lgl_polygonmode_enum mode)
   {
      polygonmode_ = mode;
   }

   static void lglTogglePolygonMode()
   {
      if (polygonmode_ == LGL_FILL)
         polygonmode_ = LGL_LINE;
      else if (polygonmode_ == LGL_LINE)
         polygonmode_ = LGL_FILL;
   }

   //! get the actual polygon mode
   static lgl_polygonmode_enum lglGetPolygonMode()
   {
      return(polygonmode_);
   }

   //! specify the interlacing mode
   static void lglInterlacingMode(lgl_interlacing_enum mode)
   {
      interlacing_ = mode;
   }

   //! specify the complementary interlacing mode
   static void lglComplementaryInterlacingMode(lgl_interlacing_enum mode)
   {
      switch (mode)
      {
         case LGL_INTERLACE_NONE: break;
         case LGL_INTERLACE_HORIZONTAL_LEFT: mode = LGL_INTERLACE_HORIZONTAL_RIGHT; break;
         case LGL_INTERLACE_HORIZONTAL_RIGHT: mode = LGL_INTERLACE_HORIZONTAL_LEFT; break;
         case LGL_INTERLACE_VERTICAL_TOP: mode = LGL_INTERLACE_VERTICAL_BOTTOM; break;
         case LGL_INTERLACE_VERTICAL_BOTTOM: mode = LGL_INTERLACE_VERTICAL_TOP; break;
      }

      lglInterlacingMode(mode);
   }

   //! get the actual interlacing mode
   static lgl_interlacing_enum lglGetInterlacingMode()
   {
      return(interlacing_);
   }

   //! read frame buffer
   static unsigned char *lglReadRGBPixels(int x, int y, int width, int height)
   {
#ifndef LGL_GLES

      unsigned char *pixels;

      glFinish();

      if ((pixels=(unsigned char *)malloc(3*width*height)) == NULL)
      {
         lglError("insufficient memory");
         return(NULL);
      }

      glReadBuffer(GL_BACK);
      glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

      for (int y=1; y<height/2; y++)
         for (int x=1; x<width; x++)
            for (int z=0; z<3; z++)
            {
               unsigned char tmp = pixels[3*(x+y*width)+z];
               pixels[3*(x+y*width)+z] = pixels[3*(x+(height-y)*width)+z];
               pixels[3*(x+(height-y)*width)+z] = tmp;
            }

      return(pixels);

#endif

      return(NULL);
   }

   //! cast a ray and intersect it with the transformed triangles contained in the vbo
   double lglIntersect(vec3 origin, vec3 direction, mat4 m, double mindist = 0) const
   {
      double dist = DBL_MAX;

      mat4 mi = m.invert();
      vec3 o = mi * vec4(origin);
      vec3 d = (mat3(mi) * direction).normalize();

      // check bbox for intersection
      if (!glslmath::itest_ray_bbox(o, d, lglGetCenter(), 0.5*lglGetExtent()))
         return(dist);

      // check vertex array for intersections
      switch (primitive_)
      {
         case LGL_NONE:
         case LGL_LINES:
         case LGL_LINE_STRIP:
            break;
         case LGL_TRIANGLE_FAN:
            if (size_ > 2)
            {
               vec3 v1 = vertices_[0];
               vec3 v2 = vertices_[1];
               for (int i=2; i<size_; i++)
               {
                  vec3 v3 = vertices_[i];
                  double td = glslmath::ray_triangle_dist(o, d, v1, v2, v3);
                  v2 = v3;

                  if (td < dist) dist = td;
               }
            }
            break;
         case LGL_QUADS:
            if (size_ > 3)
            {
               for (int i=0; i<size_/4; i++)
               {
                  vec3 v1 = vertices_[4*i];
                  vec3 v2 = vertices_[4*i+1];
                  vec3 v3 = vertices_[4*i+2];
                  vec3 v4 = vertices_[4*i+3];
                  double td1 = glslmath::ray_triangle_dist(o, d, v1, v2, v3);
                  double td2 = glslmath::ray_triangle_dist(o, d, v1, v3, v4);

                  if (td1 < dist) dist = td1;
                  if (td2 < dist) dist = td2;
               }
            }
            break;
         case LGL_QUAD_STRIP:
            if (size_ > 3)
            {
               vec3 v1 = vertices_[0];
               vec3 v2 = vertices_[1];
               for (int i=2; i+1<size_; i+=2)
               {
                  vec3 v3 = vertices_[i];
                  vec3 v4 = vertices_[i+1];
                  double td1 = glslmath::ray_triangle_dist(o, d, v1, v2, v4);
                  double td2 = glslmath::ray_triangle_dist(o, d, v1, v4, v3);
                  v1 = v3;
                  v2 = v4;

                  if (td1 < dist) dist = td1;
                  if (td2 < dist) dist = td2;
               }
            }
            break;
         case LGL_TRIANGLE_STRIP:
            if (size_ > 2)
            {
               vec3 v1 = vertices_[0];
               vec3 v2 = vertices_[1];
               for (int i=2; i<size_; i++)
               {
                  vec3 v3 = vertices_[i];
                  double td = glslmath::ray_triangle_dist(o, d, v1, v2, v3);
                  v1 = v2;
                  v2 = v3;

                  if (td < dist) dist = td;
               }
            }
            break;
         case LGL_TRIANGLES:
            if (size_ > 2)
            {
               for (int i=0; i<size_/3; i++)
               {
                  vec3 v1 = vertices_[3*i];
                  vec3 v2 = vertices_[3*i+1];
                  vec3 v3 = vertices_[3*i+2];
                  double td = glslmath::ray_triangle_dist(o, d, v1, v2, v3);

                  if (td < dist) dist = td;
               }
            }
            break;
      }

      // transform hit distance
      if (dist != DBL_MAX)
      {
         vec3 p = m*vec4(o + dist*d);
         dist = (p-origin).length();
      }

      if (dist < mindist)
         dist = DBL_MAX;

      return(dist);
   }

   //! cast a ray and intersect it with the transformed triangles contained in the vbo
   double lglIntersect(vec3 origin, vec3 direction, double mindist = 0) const
   {
      return(lglIntersect(origin, direction, lglGetModelViewMatrix(), mindist));
   }

   //! begin ray casting in view coordinates
   static void lglBeginRayCast(vec3 origin, vec3 direction, double mindist = 0)
   {
      raycast_ = true;
      raycast_origin_ = origin;
      raycast_direction_ = direction;
      raycast_mindist_ = mindist;
      raycast_dist_ = DBL_MAX;
      raycast_vbo_ = NULL;
   }

   //! cast a ray and intersect it with the transformed triangles contained in the vbo
   static void lglRayCast(lgl *vbo)
   {
      if (raycast_)
      {
         double d = vbo->lglIntersect(raycast_origin_, raycast_direction_, raycast_mindist_);

         if (d < raycast_dist_)
         {
            raycast_dist_ = d;
            raycast_vbo_ = vbo;
         }
      }
   }

   //! end ray casting
   static lgl *lglEndRayCast()
   {
      raycast_ = false;
      return(raycast_vbo_);
   }

   //! get distance to front-most hit point determined by ray casting
   static double lglGetRayCastDistance()
   {
      return(raycast_dist_);
   }

   //! enable view culling
   static void lglEnableViewCulling(int minsize = 0)
   {
      cull_ = true;
      cull_minsize_ = minsize;
   }

   //! disable view culling
   static void lglDisableViewCulling()
   {
      cull_ = false;
   }

   //! begin scene export
   //! * if lglLoadMatrix is used instead of lglLookAt or lglView
   //!   then the inverse of the actual view matrix needs to be provided as additional parameter
   //! * otherwise the scene will be exported in view coordinates and not in world coordinates
   static void lglBeginExport(void (*callback)(lgl *vbo, void *data), void *data,
                              mat4 matrix = mat4(1))
   {
      export_ = callback;
      export_data_ = data;
      export_matrix_ = matrix;
   }

   //! export vbo
   static void lglExport(lgl *vbo)
   {
      if (export_)
      {
         lgl copy;
         vbo->lglAppendVerticesTo(&copy);
         vec4 c = vbo->lglGetColor();
         if (c != vec4(1)) copy.lglApplyColor(c);
         mat4 m = vbo->lglGetModelViewMatrix();
         if (export_matrix_ != mat4(1)) m = export_matrix_ * m;
         else
         {
            if (lglIsManipApplied()) m = lglGetManip().invert() * m;
            if (eye_!=at_ && up_!=vec3(0)) m = mat4::lookat(eye_, at_, up_).invert() * m;
         }
         copy.lglModel(m);
         copy.lglApplyModelMatrix();
         export_(&copy, export_data_);
      }
   }

   //! end scene export
   static void lglEndExport()
   {
      export_ = NULL;
   }

   //! get the OpenGL version
   static int lglGetGLVersion()
   {
      return(LGL_OPENGL_VERSION);
   }

   //! map gl version to glsl version
   static int lglMapGL2GLSLVersion(int glversion)
   {
      if (glversion < 20) return(0);

      switch (glversion)
      {
         case 20 : return(11);
         case 21 : return(12);
         case 30 : return(13);
         case 31 : return(14);
         case 32 : return(15);
         case 33 : return(33);
         case 40 : return(40);
         case 41 : return(41);
         case 42 : return(42);
         case 43 : return(43);
         case 44 : return(44);
         case 45 : return(45);
         case 46 : return(46);
         default : return(glversion);
      }
   }

   //! get the GLSL version
   static int lglGetGLSLVersion()
   {
      return(lglMapGL2GLSLVersion(lglGetGLVersion()));
   }

   //! get the OpenGL renderer
   static std::string lglGetRenderer()
   {
      initializeRenderer();
      return(renderer_);
   }

   //! get the OpenGL vendor
   static lgl_vendor_enum lglGetVendor()
   {
      initializeRenderer();
      return(vendor_);
   }

   //! get the vendor name of the graphics hardware
   static std::string lglGetVendorName()
   {
      initializeRenderer();
      switch (vendor_)
      {
         case LGL_NVIDIA: return("NVIDIA");
         case LGL_ATI: return("ATI");
         case LGL_INTEL: return("Intel");
         default: return("unknown");
      }
   }

   //! get a description of the last occured error
   static std::string lglGetError()
   {
      if (error_.empty())
      {
         GLenum error = glGetError();

         if (error != GL_NO_ERROR)
         {
            if (error == GL_INVALID_ENUM) error_ = "invalid GL enum";
            else if (error == GL_INVALID_VALUE) error_ = "invalid GL value";
            else if (error == GL_INVALID_OPERATION) error_ = "invalid GL operation";
            else error_ = "unknown GL error";

            std::cerr << error_ << std::endl;
         }
      }

      std::string e = error_;
      error_.clear();

      return(e);
   }

   //! get a description of the last occured warning
   static std::string lglGetWarning()
   {
      std::string w = warning_;
      warning_.clear();

      return(w);
   }

protected:

   std::string name_;
   bool immediate_;
   bool storagetype_;

   vec4 vertex_;
   bool copied_;
   bool copy_;

   vec4f color_;
   vec3f normal_;
   vec4f texcoord_;
   vec4f attribute_[LGL_NUM_ATTRIBUTES];

   bool hascolor_;
   bool hasnormal_;
   bool hastexcoord_;
   bool hasattribute_[LGL_NUM_ATTRIBUTES];
   bool hasattributes_;

   int size_, maxsize_;

   VEC4 *vertices_;
   vec4f *verticesf_;
   vec4f *colors_;
   vec3f *normals_;
   vec4f *texcoords_;
   vec3f *barycentrics_;
   vec4f *attributes_;

   vec3 bboxmin_, bboxmax_;

   lgl_primitive_enum primitive_;
   int primitives_;

   bool started_;
   bool rearrange_;

   bool modified_;
   bool rendered_;
   bool applied_;

   bool coloring_;
   bool lighting_;
   bool texturing_;

   static bool disable_coloring_;
   static bool disable_lighting_;
   static bool disable_texturing_;

   static std::string renderer_;
   static lgl_vendor_enum vendor_;

   static vec4f actual_;
   static lgl_matrixmode_enum matrixmode_;

   static std::vector<mat4> projection_matrix_;
   static std::vector<mat4> modelview_matrix_;
   static std::vector<mat4> texture_matrix_;

   static std::vector<mat4> premodel_matrix_;
   static std::vector<bool> premodel_matrix_identity_;
   static std::vector<mat3> premodel_matrix_it_;
   static std::vector<bool> premodel_matrix_recompute_;

   static std::vector<mat4> pretex_matrix_;
   static std::vector<bool> pretex_matrix_identity_;

   static mat4 manip_matrix_;
   static bool manip_matrix_identity_;
   static bool manip_matrix_apply_;

   mat4 model_matrix_;
   bool model_matrix_identity_;

   mat4 tex_matrix_;
   bool tex_matrix_identity_;

   static double fovy_, aspect_, nearp_, farp_;
   static vec3 eye_, at_, up_;

   static vec4f light_;
   vec3f ka_, kd_, ks_;
   static vec3f Ia_, Id_, Is_;
   float exponent_;
   static vec3f falloff_;

   GLuint texid2D_;
   GLuint texid3D_;

   lgl_texgenmode_enum texgen_;
   double texgen_mix_;

   static lgl_blendmode_enum blendmode_;

   static float alphavalue_;
   static vec3f alphatest_;

   static int clipping_;
   static vec4 clipplane_[LGL_NUM_CLIPPLANES];

   static float fogdensity_;
   static vec4f fogcolor_;

   static lgl_polygonmode_enum polygonmode_;
   static lgl_interlacing_enum interlacing_;

   static bool raycast_;
   static vec3 raycast_origin_;
   static vec3 raycast_direction_;
   static double raycast_mindist_;
   static double raycast_dist_;
   static lgl *raycast_vbo_;

   static bool cull_;
   static double cull_minsize_;

   static void (*export_)(lgl *vbo, void *data);
   static void *export_data_;
   static mat4 export_matrix_;

   static std::string error_;
   static std::string warning_;
   static lgl_verbosity_enum verbosity_;

   virtual void init_gl_hook() {}
   virtual void exit_gl_hook() {}

   virtual void pre_render_hook() {}
   virtual void post_render_hook() {}

#ifdef LGL_CORE

   GLuint buffers_[LGL_NUM_BUFFERS];
   GLuint array_;

   GLuint program_;
   GLuint usedprogram_;

   GLint color_loc_;
   GLint mv_loc_;
   GLint mvp_loc_;
   GLint mvit_loc_;
   GLint tm_loc_;
   GLint light_loc_;
   GLint kaIa_loc_, kdId_loc_, ksIs_loc_;
   GLint exponent_loc_;
   GLint falloff_loc_;
   GLint sampler_loc_;
   GLint alphatest_loc_;
   GLint clipping_loc_;
   GLint clipplane_loc_;
   GLint fogdensity_loc_;
   GLint fogcolor_loc_;
   GLint wireframe_loc_;
   GLint interlacing_loc_;

   static GLuint programs_[LGL_NUM_SHADERS];
   static GLuint custom_programs_[LGL_NUM_SHADERS];

   lgl_uniform_map_type uniforms_;
   lgl_uniform_location_type locations_;

#endif

   bool initGL_;

   static int instances_;
   static bool initStaticGL_;

#ifdef _WIN32

   #undef WGL_MACRO
   #define WGL_MACRO(proc,proctype) static PFN##proctype##PROC proc;
   #include "glvertex_wgl.h"

#endif

   void initialize()
   {
      size_ = maxsize_ = 0;
      primitives_ = 0;

      vertices_ = NULL;
      verticesf_ = NULL;

      colors_ = NULL;
      normals_ = NULL;
      texcoords_ = NULL;
      barycentrics_ = NULL;
      attributes_ = NULL;

      for (unsigned int i=0; i<LGL_NUM_ATTRIBUTES; i++)
      {
         attribute_[i] = vec4f(0);
         hasattribute_[i] = false;
      }

      hasattributes_ = false;

      bboxmin_ = bboxmax_ = vec3(NAN);

      primitive_ = LGL_NONE;

      started_ = false;
      rearrange_ = false;

      modified_ = false;
      rendered_ = false;
      applied_ = false;

      coloring_ = true;
      lighting_ = true;
      texturing_ = true;

      model_matrix_ = mat4(1);
      model_matrix_identity_ = true;

      tex_matrix_ = mat4(1);
      tex_matrix_identity_ = true;

      ka_ = vec3f(0.1f);
      kd_ = vec3f(0.7f);
      ks_ = vec3f(0.2f);

      exponent_ = 30;

      texid2D_ = 0;
      texid3D_ = 0;

      texgen_ = LGL_TEXGEN_NONE;
      texgen_mix_ = 0;

#ifdef LGL_CORE

      for (int i=0; i<LGL_NUM_BUFFERS; i++)
         buffers_[i] = 0;

      program_ = 0;
      usedprogram_ = 0;

      color_loc_ = -1;
      mv_loc_ = -1;
      mvp_loc_ = -1;
      mvit_loc_ = -1;
      tm_loc_ = -1;
      light_loc_ = -1;
      kaIa_loc_ = kdId_loc_ = ksIs_loc_ = -1;
      exponent_loc_ = -1;
      falloff_loc_ = -1;
      sampler_loc_ = -1;
      alphatest_loc_ = -1;
      clipping_loc_ = -1;
      clipplane_loc_ = -1;
      fogdensity_loc_ = -1;
      fogcolor_loc_ = -1;
      wireframe_loc_ = -1;
      interlacing_loc_ = -1;

#endif

      initGL_ = false;
   }

   static void initializeStatic()
   {
#ifdef LGL_CORE

      for (int i=0; i<LGL_NUM_SHADERS; i++)
      {
         programs_[0] = 0;
         custom_programs_[0] = 0;
      }

#endif

      for (int i=0; i<LGL_NUM_CLIPPLANES; i++)
         clipplane_[i] = vec4(0,0,0,0);
   }

   static void initializeRenderer()
   {
      if (renderer_ == "")
      {
         char *renderer = (char *)glGetString(GL_RENDERER);

         if (renderer != NULL)
            renderer_ = std::string(renderer);

         char *vendor = (char *)glGetString(GL_VENDOR);

         if (vendor != NULL)
         {
            if (strstr(vendor,"NVIDIA")!=NULL) vendor_ = LGL_NVIDIA;
            else if (strstr(vendor,"ATI")!=NULL) vendor_ = LGL_ATI;
            else if (strstr(vendor,"Intel")!=NULL) vendor_ = LGL_INTEL;
         }
      }
   }

   void initializeOpenGL()
   {
#ifdef LGL_CORE

      glGenBuffers(LGL_NUM_BUFFERS, buffers_);

#if defined(LGL_GL3) || defined(LGL_GLES3)

      glGenVertexArrays(1, &array_);

#endif

#endif
   }

   static void initializeStaticOpenGL()
   {
      initializeRenderer();

#ifdef _WIN32
      initWGLprocs();
#endif

#ifdef LGL_CORE

      unsigned int num = LGL_NUM_SHADERS;

      programs_[0] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(false,false,false),lglBasicGLSLFragmentShader(false,false,false,false));
      programs_[1] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(true,false,false),lglBasicGLSLFragmentShader(true,false,false,false));
      programs_[2] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(false,true,false),lglBasicGLSLFragmentShader(false,true,false,false));
      programs_[3] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(true,true,false),lglBasicGLSLFragmentShader(true,true,false,false));
      programs_[4] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(false,false,true),lglBasicGLSLFragmentShader(false,false,true,false));
      programs_[5] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(true,false,true),lglBasicGLSLFragmentShader(true,false,true,false));
      programs_[6] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(false,true,true),lglBasicGLSLFragmentShader(false,true,true,false));
      programs_[7] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(true,true,true),lglBasicGLSLFragmentShader(true,true,true,false));
#if !defined(LGL_GLES) || defined(LGL_GLES3)
      programs_[8] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(false,false,true),lglBasicGLSLFragmentShader(false,false,true,true));
      programs_[9] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(true,false,true),lglBasicGLSLFragmentShader(true,false,true,true));
      programs_[10] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(false,true,true),lglBasicGLSLFragmentShader(false,true,true,true));
      programs_[11] = lglCompileGLSLProgram(lglBasicGLSLVertexShader(true,true,true),lglBasicGLSLFragmentShader(true,true,true,true));
#else
      num -= 4;
#endif

      for (unsigned int i=0; i<num; i++)
         if (programs_[i] == 0)
            lglFatal("could not compile basic GLSL program");

#endif
   }

   void finalizeOpenGL()
   {
#ifdef LGL_CORE

      glDeleteBuffers(LGL_NUM_BUFFERS, buffers_);

#if defined(LGL_GL3) || defined(LGL_GLES3)

      glDeleteVertexArrays(1, &array_);

#endif

#endif
   }

   static void finalizeStaticOpenGL()
   {
#ifdef LGL_CORE

      for (int i=0; i<8; i++)
         lglDeleteGLSLProgram(programs_[i]);

#endif
   }

   void finalize()
   {
      if (vertices_) free(vertices_);
      if (verticesf_) free(verticesf_);

      if (colors_) free(colors_);
      if (normals_) free(normals_);
      if (texcoords_) free(texcoords_);
      if (barycentrics_) free(barycentrics_);
      if (attributes_) free(attributes_);
   }

   static void finalizeStatic()
   {
      checkMatrixStacks();

      if (projection_matrix_.size() > 1)
         lglError("unbalanced projection matrix stack");

      if (modelview_matrix_.size() > 1)
         lglError("unbalanced model-view matrix stack");

      if (premodel_matrix_.size() > 1)
         lglError("unbalanced pre-model matrix stack");

      if (texture_matrix_.size() > 1)
         lglError("unbalanced texture matrix stack");

      if (pretex_matrix_.size() > 1)
         lglError("unbalanced pre-texture matrix stack");
   }

   bool checkVertexScope()
   {
      if (started_)
         if (matrixmode_!=LGL_PREMODEL && matrixmode_!=LGL_PRETEX)
         {
            lglError("operation not allowed between lglBegin and lglEnd");
            return(false);
         }

      return(true);
   }

   static bool checkMatrixStacks()
   {
      if (projection_matrix_.size() == 0)
      {
         lglError("projection matrix stack underrun");
         return(false);
      }

      if (modelview_matrix_.size() == 0)
      {
         lglError("model-view matrix stack underrun");
         return(false);
      }

      if (premodel_matrix_.size() == 0)
      {
         lglError("pre-model matrix stack underrun");
         return(false);
      }

      if (texture_matrix_.size() == 0)
      {
         lglError("texture matrix stack underrun");
         return(false);
      }

      if (pretex_matrix_.size() == 0)
      {
         lglError("pre-texture matrix stack underrun");
         return(false);
      }

      return(true);
   }

   //! interlaced stereo modes:
   //! * columns are numbered from left to right starting with 1
   //! * lines are numbered from top to bottom starting with 1
   //! * mode LGL_INTERLACE_HORIZONTAL_LEFT: render odd vertical lines
   //! * mode LGL_INTERLACE_HORIZONTAL_RIGHT: render even vertical lines
   //! * mode LGL_INTERLACE_VERTICAL_TOP: render odd horizontal lines
   //! * mode LGL_INTERLACE_VERTICAL_BOTTOM: render even horizontal lines
   vec4f setupStereoMode(lgl_interlacing_enum mode)
   {
      float a=0.0f,b=0.0f,c=0.5f,d=0.5f;

      if (mode==LGL_INTERLACE_HORIZONTAL_LEFT) {a=0.5f; c=0.0f;}
      else if (mode==LGL_INTERLACE_HORIZONTAL_RIGHT) {a=0.5f; c=0.5f;}
      else if (mode==LGL_INTERLACE_VERTICAL_TOP) {b=0.5f; d=0.0f;}
      else if (mode==LGL_INTERLACE_VERTICAL_BOTTOM) {b=0.5f; d=0.5f;}

      return(vec4f(a,b,c,d));
   }

#ifdef _WIN32

   static void initWGLprocs()
   {
      static bool init = false;
      if (!init)
      {
         #undef WGL_MACRO
         #define WGL_MACRO(proc,proctype) if ((proc=(PFN##proctype##PROC)wglGetProcAddress(#proc))==NULL) lglWarning(#proc" not supported");
         #include "glvertex_wgl.h"
         init = true;
      }
   }

#endif

public:

   // wrapped extension function
   void lglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

      glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
   }

   // wrapped extension function
   void lglTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
   {
#ifdef _WIN32
      initWGLprocs();
#endif

#if !defined(LGL_GLES) || defined (LGL_GLES3)
      glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
#endif
   }

   //! generate an error message
   static void lglError(std::string e)
   {
      error_ = e;

      if (verbosity_ >= LGL_VERBOSITY_ERRORS)
         std::cerr << e << std::endl;
   }

   //! generate a warning message
   static void lglWarning(std::string w)
   {
      warning_ = w;

      if (verbosity_ >= LGL_VERBOSITY_WARNINGS)
         std::cerr << w << std::endl;
   }

   //! generate a non-recoverable error
   static void lglFatal(std::string e)
   {
      lglError(e);
      exit(1);
   }

   //! change verbosity level
   static void lglVerbosity(lgl_verbosity_enum v)
   {
      verbosity_ = v;
   }

   template<class v, const GLenum t>
   friend std::ostream& operator << (std::ostream &out, const lgl<v, t> &vbo);
};

template<class VEC4, const GLenum gl_type> std::string lgl<VEC4, gl_type>::renderer_("");
template<class VEC4, const GLenum gl_type> lgl_vendor_enum lgl<VEC4, gl_type>::vendor_(LGL_UNKNOWN);

template<class VEC4, const GLenum gl_type> vec4f lgl<VEC4, gl_type>::actual_(1);
template<class VEC4, const GLenum gl_type> lgl_matrixmode_enum lgl<VEC4, gl_type>::matrixmode_(LGL_MODELVIEW);

template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::disable_coloring_(false);
template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::disable_lighting_(false);
template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::disable_texturing_(false);

template<class VEC4, const GLenum gl_type> std::vector<mat4> lgl<VEC4, gl_type>::projection_matrix_(1, mat4(1));
template<class VEC4, const GLenum gl_type> std::vector<mat4> lgl<VEC4, gl_type>::modelview_matrix_(1, mat4(1));
template<class VEC4, const GLenum gl_type> std::vector<mat4> lgl<VEC4, gl_type>::texture_matrix_(1, mat4(1));

template<class VEC4, const GLenum gl_type> std::vector<mat4> lgl<VEC4, gl_type>::premodel_matrix_(1, mat4(1));
template<class VEC4, const GLenum gl_type> std::vector<bool> lgl<VEC4, gl_type>::premodel_matrix_identity_(1, true);
template<class VEC4, const GLenum gl_type> std::vector<mat3> lgl<VEC4, gl_type>::premodel_matrix_it_(1, mat3(1));
template<class VEC4, const GLenum gl_type> std::vector<bool> lgl<VEC4, gl_type>::premodel_matrix_recompute_(1, false);

template<class VEC4, const GLenum gl_type> std::vector<mat4> lgl<VEC4, gl_type>::pretex_matrix_(1, mat4(1));
template<class VEC4, const GLenum gl_type> std::vector<bool> lgl<VEC4, gl_type>::pretex_matrix_identity_(1, true);

template<class VEC4, const GLenum gl_type> mat4 lgl<VEC4, gl_type>::manip_matrix_(mat4(1));
template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::manip_matrix_identity_(true);
template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::manip_matrix_apply_(false);

template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::fovy_(0);
template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::aspect_(0);
template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::nearp_(0);
template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::farp_(0);

template<class VEC4, const GLenum gl_type> vec3 lgl<VEC4, gl_type>::eye_(0);
template<class VEC4, const GLenum gl_type> vec3 lgl<VEC4, gl_type>::at_(0);
template<class VEC4, const GLenum gl_type> vec3 lgl<VEC4, gl_type>::up_(0);

template<class VEC4, const GLenum gl_type> vec4f lgl<VEC4, gl_type>::light_(0,0,1,0);
template<class VEC4, const GLenum gl_type> vec3f lgl<VEC4, gl_type>::Ia_(1);
template<class VEC4, const GLenum gl_type> vec3f lgl<VEC4, gl_type>::Id_(1);
template<class VEC4, const GLenum gl_type> vec3f lgl<VEC4, gl_type>::Is_(1);
template<class VEC4, const GLenum gl_type> vec3f lgl<VEC4, gl_type>::falloff_(1,0,0);

template<class VEC4, const GLenum gl_type> lgl_blendmode_enum lgl<VEC4, gl_type>::blendmode_(LGL_BLEND_NONE);

template<class VEC4, const GLenum gl_type> float lgl<VEC4, gl_type>::alphavalue_(0.0f);
template<class VEC4, const GLenum gl_type> vec3f lgl<VEC4, gl_type>::alphatest_(-1,-1,0);

template<class VEC4, const GLenum gl_type> int lgl<VEC4, gl_type>::clipping_(0);
template<class VEC4, const GLenum gl_type> vec4 lgl<VEC4, gl_type>::clipplane_[LGL_NUM_CLIPPLANES];

template<class VEC4, const GLenum gl_type> float lgl<VEC4, gl_type>::fogdensity_(0.0f);
template<class VEC4, const GLenum gl_type> vec4f lgl<VEC4, gl_type>::fogcolor_(1);

template<class VEC4, const GLenum gl_type> lgl_polygonmode_enum lgl<VEC4, gl_type>::polygonmode_(LGL_FILL);
template<class VEC4, const GLenum gl_type> lgl_interlacing_enum lgl<VEC4, gl_type>::interlacing_(LGL_INTERLACE_NONE);

template<class VEC4, const GLenum gl_type> std::string lgl<VEC4, gl_type>::error_;
template<class VEC4, const GLenum gl_type> std::string lgl<VEC4, gl_type>::warning_;
template<class VEC4, const GLenum gl_type> lgl_verbosity_enum lgl<VEC4, gl_type>::verbosity_(LGL_VERBOSITY_ERRORS);

#ifdef LGL_CORE

template<class VEC4, const GLenum gl_type> GLuint lgl<VEC4, gl_type>::programs_[LGL_NUM_SHADERS];
template<class VEC4, const GLenum gl_type> GLuint lgl<VEC4, gl_type>::custom_programs_[LGL_NUM_SHADERS];

#endif

template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::raycast_(false);
template<class VEC4, const GLenum gl_type> vec3 lgl<VEC4, gl_type>::raycast_origin_(0);
template<class VEC4, const GLenum gl_type> vec3 lgl<VEC4, gl_type>::raycast_direction_(0);
template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::raycast_mindist_(DBL_MAX);
template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::raycast_dist_(DBL_MAX);
template<class VEC4, const GLenum gl_type> lgl<VEC4, gl_type> *lgl<VEC4, gl_type>::raycast_vbo_(NULL);

template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::cull_(false);
template<class VEC4, const GLenum gl_type> double lgl<VEC4, gl_type>::cull_minsize_(0);

template<class VEC4, const GLenum gl_type> void (*lgl<VEC4, gl_type>::export_)(lgl *vbo, void *data)(NULL);
template<class VEC4, const GLenum gl_type> void *lgl<VEC4, gl_type>::export_data_(NULL);
template<class VEC4, const GLenum gl_type> mat4 lgl<VEC4, gl_type>::export_matrix_(0);

template<class VEC4, const GLenum gl_type> int lgl<VEC4, gl_type>::instances_(0);
template<class VEC4, const GLenum gl_type> bool lgl<VEC4, gl_type>::initStaticGL_(false);

#ifdef _WIN32

#undef WGL_MACRO
#define WGL_MACRO(proc,proctype) template<class VEC4, const GLenum gl_type> PFN##proctype##PROC lgl<VEC4, gl_type>::proc = NULL;
#include "glvertex_wgl.h"

#endif

//! output operator
template<class VEC4, const GLenum gl_type>
std::ostream& operator << (std::ostream &out, const lgl<VEC4, gl_type> &vbo)
{
   for (unsigned int i=0; i<vbo.size_; i++)
   {
      if (vbo.storagetype_) out << "vertex #" << i+1 << ": " << vbo.vertices_[i] << std::endl;
      else out << "vertex #" << i+1 << ": " << vbo.verticesf_[i] << std::endl;

      if (vbo.hascolor_) out << "color #" << i+1 << ": " << vbo.colors_[i] << std::endl;
      if (vbo.hasnormal_) out << "normal #" << i+1 << ": " << vbo.normals_[i] << std::endl;
      if (vbo.hastexcoord_) out << "texcoord #" << i+1 << ": " << vbo.texcoords_[i] << std::endl;
   }

   return(out);
}

// LGL vbo storage type
#ifndef LGL_GLES
#   define LGL_VEC4 vec4
#   define LGL_GL_TYPE GL_DOUBLE
#else
#   define LGL_VEC4 vec4f
#   define LGL_GL_TYPE GL_FLOAT
#endif

// LGL vbo container type
typedef lgl<LGL_VEC4, LGL_GL_TYPE> LGL_VBO_TYPE;

//! LGL API: immediate mode class definition
class GL: public LGL_VBO_TYPE
{
public:
   GL() : LGL_VBO_TYPE("lgl", true) {}
};

//! LGL API: vbo class definition
class lglVBO: public LGL_VBO_TYPE
{
public:
   lglVBO(const std::string &name = "", bool storagetype = true)
      : LGL_VBO_TYPE(name, false, storagetype) {}
};

//! LGL API: vbo class definition (storage type: float)
class lglVBOf: public lglVBO
{
public:
   lglVBOf(const std::string &name = "")
      : lglVBO(name, false) {}
};

#endif
