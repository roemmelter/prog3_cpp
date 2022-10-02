// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_SHADERS_H
#define GLVERTEX_SHADERS_H

#include "glvertex_gl.h"

#ifdef LGL_GLES
#define LGL_GLSL_VERSION "100"
#else
#define LGL_GLSL_VERSION "120"
#endif

static const char lgl_plain_vertex_shader_header[] =
   "#version " LGL_GLSL_VERSION "\n"
   "attribute vec4 vertex_position;\n"
   "uniform mat4 mvp;\n";

static const char lgl_plain_fragment_shader_header[] =
   "#version " LGL_GLSL_VERSION "\n"
   "uniform vec4 color;\n";

static const char lgl_plain_vertex_shader[] =
   "void main()\n"
   "{\n"
   "   gl_Position = mvp * vertex_position;\n"
   "}\n";

static const char lgl_plain_fragment_shader[] =
   "void main()\n"
   "{\n"
   "   gl_FragColor = color;\n"
   "}\n";

static const char lgl_default_vertex_shader_header[] =
   "#version " LGL_GLSL_VERSION "\n"
   "attribute vec4 vertex_position;\n"
   "attribute vec4 vertex_color;\n"
   "attribute vec3 vertex_normal;\n"
   "attribute vec4 vertex_texcoord;\n"
   "attribute vec3 vertex_barycentric;\n"
   "uniform mat4 mv;\n"
   "uniform mat4 mvp;\n"
   "uniform mat4 mvit;\n"
   "uniform mat4 tm;\n"
   "uniform vec4 light;\n"
   "varying vec3 frag_barycentric;\n"
   "vec3 normalize0(const vec3 v) {float l = length(v); return((l>0.0f)?v/l:v);}\n"
   "vec3 flightvec(vec4 mv_pos) {return((light.w==0.0f)? vec3(light) : vec3(light - mv_pos));}\n"
   "vec3 flightdir(vec4 mv_pos) {return((light.w==0.0f)? vec3(light) : normalize(vec3(light - mv_pos)));}\n"
   "vec4 fvertex() {return(mvp * vertex_position);}\n"
   "vec3 fnormal() {return(normalize0(mat3(mvit) * vertex_normal));}\n"
   "vec3 fhalfway(vec4 mv_pos) {return(normalize(flightdir(mv_pos) - normalize(vec3(mv_pos))));}\n"
   "vec4 ftexcoord() {return(tm * vertex_texcoord);}\n";

static const char lgl_default_fragment_shader_header[] =
   "#version " LGL_GLSL_VERSION "\n"
#ifdef LGL_GLES
   "#extension GL_OES_texture_3D : enable\n"
   "#extension GL_OES_standard_derivatives : enable\n"
   "precision highp float;\n"
   "precision highp sampler2D;\n"
#endif
#ifdef LGL_GLES3
   "precision highp sampler3D;\n"
#endif
   "uniform vec4 color;\n"
   "uniform float fogdensity;\n"
   "uniform vec4 fogcolor;\n"
   "uniform float wireframe;\n"
   "varying vec3 frag_barycentric;\n"
   "vec4 ffog(vec4 color)\n"
   "{\n"
   "   if (fogdensity > 0.0f)\n"
   "   {\n"
   "      float z = 1.0f/gl_FragCoord.w;\n"
   "      float f = 1.0f-exp(-fogdensity*z*z);\n"
   "      return((1.0f-f)*color + f*fogcolor);\n"
   "   }\n"
   "   return(color);\n"
   "}\n"
   "float fbarycentric()\n"
   "{\n"
   "   vec3 b = frag_barycentric;\n"
   "   vec3 db = fwidth(frag_barycentric);\n"
   "   vec3 b1 = smoothstep(vec3(0), db, b);\n"
   "   float w1 = min(min(b1.x, b1.y), b1.z);\n"
   "   vec3 b2 = smoothstep(vec3(0), db, min(vec3(1)-b, vec3(1)));\n"
   "   float w2 = min(min(b2.x, b2.y), b2.z);\n"
   "   return(min(w1, w2));\n"
   "}\n"
   "void fwireframe()\n"
   "{\n"
   "   if (wireframe > 0.0f)\n"
   "      if (fbarycentric() >= 1.0f) discard;\n"
   "}\n";

static const char lgl_default_vertex_shader_lighting[] =
   "varying vec3 frag_normal;\n"
   "varying vec3 frag_halfway;\n"
   "varying vec3 frag_lightvec;\n";

static const char lgl_default_fragment_shader_lighting[] =
   "uniform vec3 kaIa,kdId,ksIs;\n"
   "uniform float exponent;\n"
   "uniform vec3 falloff;\n"
   "varying vec3 frag_normal;\n"
   "varying vec3 frag_halfway;\n"
   "varying vec3 frag_lightvec;\n"
   "vec4 flighting(vec4 color)\n"
   "{\n"
   "   vec3 n = normalize(frag_normal);\n"
   "   vec3 h = normalize(frag_halfway);\n"
   "   vec3 l = normalize(frag_lightvec);\n"
   "   float d = length(frag_lightvec);\n"
   "   float diffuse = dot(l, n);\n"
   "   float specular = dot(h, n);\n"
   "   float attenuation = 1.0f / (falloff.x + falloff.y*d + falloff.z*d*d);\n"
   "   diffuse = (diffuse < 0.0f)? 0.0f : diffuse;\n"
   "   specular = pow((specular < 0.0f)? 0.0f : specular, exponent);\n"
   "   attenuation = (attenuation > 1.0f)? 1.0f : attenuation;\n"
   "   return(vec4(vec3(color) * (kaIa + kdId * diffuse * attenuation) + ksIs * specular * attenuation, color.a));\n"
   "}\n";

static const char lgl_default_vertex_shader_clipping[] =
   "uniform float clipping;\n"
   "uniform vec4 clipplane[8];\n"
   "varying float clipdist[8];\n"
   "void fclipdistance()\n"
   "{\n"
   "   if (clipping != 0.0f)\n"
   "   {\n"
   "      vec4 pos = mv * vertex_position;\n"
   "      for (int i=0; i<8; i++)\n"
   "         clipdist[i] = dot(clipplane[i], pos);\n"
   "   }\n"
   "}\n";

static const char lgl_default_fragment_shader_alphatesting[] =
   "uniform vec3 alphatest;\n"
   "vec4 falphatest(vec4 color)\n"
   "{\n"
   "   float alpha = color.a * alphatest.z;\n"
   "   if (alpha < alphatest.x || alpha == alphatest.y) discard;\n"
   "   return(color);\n"
   "}\n";

static const char lgl_default_fragment_shader_clipping[] =
   "uniform float clipping;\n"
   "varying float clipdist[8];\n"
   "void fclipping()\n"
   "{\n"
   "   if (clipping != 0.0f)\n"
   "      for (int i=0; i<8; i++)\n"
   "         if (clipdist[i] < 0.0f) discard;\n"
   "}\n";

static const char lgl_default_fragment_shader_interlacing[] =
   "uniform vec4 interlacing;\n"
   "void finterlacing()\n"
   "{\n"
   "   if (interlacing.x == interlacing.y) return;\n"
   "   vec2 pos = gl_FragCoord.xy * interlacing.xy + interlacing.zw;\n"
   "   if (fract(pos.x)<0.5 || fract(pos.y)<0.5) discard;\n"
   "}\n";

static const char lgl_default_vertex_shader1[] =
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader1[] =
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(color));\n"
   "}\n";

static const char lgl_default_vertex_shader2[] =
   "varying vec4 frag_color;\n"
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   frag_color = vertex_color;\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader2[] =
   "varying vec4 frag_color;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(color * frag_color));\n"
   "}\n";

static const char lgl_default_vertex_shader3[] =
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   vec4 mv_pos = mv * vertex_position;\n"
   "   frag_normal = fnormal();\n"
   "   frag_halfway = fhalfway(mv_pos);\n"
   "   frag_lightvec = flightvec(mv_pos);\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader3[] =
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(flighting(color)));\n"
   "}\n";

static const char lgl_default_vertex_shader4[] =
   "varying vec4 frag_color;\n"
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   vec4 mv_pos = mv * vertex_position;\n"
   "   frag_color = vertex_color;\n"
   "   frag_normal = fnormal();\n"
   "   frag_halfway = fhalfway(mv_pos);\n"
   "   frag_lightvec = flightvec(mv_pos);\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader4[] =
   "varying vec4 frag_color;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(flighting(color * frag_color)));\n"
   "}\n";

static const char lgl_default_vertex_shader5[] =
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   frag_texcoord = ftexcoord();\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader5_2D[] =
   "uniform sampler2D sampler;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(color * texture2D(sampler, frag_texcoord.xy)));\n"
   "}\n";

static const char lgl_default_fragment_shader5_3D[] =
   "uniform sampler3D sampler;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(color * texture3D(sampler, frag_texcoord.xyz)));\n"
   "}\n";

static const char lgl_default_vertex_shader6[] =
   "varying vec4 frag_color;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   frag_color = vertex_color;\n"
   "   frag_texcoord = ftexcoord();\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader6_2D[] =
   "uniform sampler2D sampler;\n"
   "varying vec4 frag_color;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(color * frag_color * texture2D(sampler, frag_texcoord.xy)));\n"
   "}\n";

static const char lgl_default_fragment_shader6_3D[] =
   "uniform sampler3D sampler;\n"
   "varying vec4 frag_color;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(color * frag_color * texture3D(sampler, frag_texcoord.xyz)));\n"
   "}\n";

static const char lgl_default_vertex_shader7[] =
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   vec4 mv_pos = mv * vertex_position;\n"
   "   frag_normal = fnormal();\n"
   "   frag_halfway = fhalfway(mv_pos);\n"
   "   frag_lightvec = flightvec(mv_pos);\n"
   "   frag_texcoord = ftexcoord();\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader7_2D[] =
   "uniform sampler2D sampler;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(flighting(color) * texture2D(sampler, frag_texcoord.xy)));\n"
   "}\n";

static const char lgl_default_fragment_shader7_3D[] =
   "uniform sampler3D sampler;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(flighting(color) * texture3D(sampler, frag_texcoord.xyz)));\n"
   "}\n";

static const char lgl_default_vertex_shader8[] =
   "varying vec4 frag_color;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipdistance();\n"
   "   vec4 mv_pos = mv * vertex_position;\n"
   "   frag_color = vertex_color;\n"
   "   frag_normal = fnormal();\n"
   "   frag_halfway = fhalfway(mv_pos);\n"
   "   frag_lightvec = flightvec(mv_pos);\n"
   "   frag_texcoord = ftexcoord();\n"
   "   frag_barycentric = vertex_barycentric;\n"
   "   gl_Position = fvertex();\n"
   "}\n";

static const char lgl_default_fragment_shader8_2D[] =
   "uniform sampler2D sampler;\n"
   "varying vec4 frag_color;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(flighting(color * frag_color) * texture2D(sampler, frag_texcoord.xy)));\n"
   "}\n";

static const char lgl_default_fragment_shader8_3D[] =
   "uniform sampler3D sampler;\n"
   "varying vec4 frag_color;\n"
   "varying vec4 frag_texcoord;\n"
   "void main()\n"
   "{\n"
   "   fclipping();\n"
   "   finterlacing();\n"
   "   fwireframe();\n"
   "   gl_FragColor = falphatest(ffog(flighting(color * frag_color) * texture3D(sampler, frag_texcoord.xyz)));\n"
   "}\n";

#endif
