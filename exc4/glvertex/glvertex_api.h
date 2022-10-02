// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL immediate mode API (as specified by OpenGL 1.2 plus extensions)
#ifndef GLVERTEX_API_H
#define GLVERTEX_API_H

#include "glvertex_core.h"

#include <time.h> // for seed

static GL LGL; // LGL immediate mode singleton
inline GL *getGL() {return(&LGL);}

// legacy OpenGL API (as specified by OpenGL 1.2):

//! begin vertex series (as defined by OpenGL 1.2)
inline void lglBegin(lgl_primitive_enum primitive)
   {LGL.lglBegin(primitive);}

//! specify vertex (as defined by OpenGL 1.2)
inline void lglVertex(const vec4 &v)
   {LGL.lglVertex(v);}

//! specify vertex (as defined by OpenGL 1.2)
inline void lglVertex(double x, double y, double z, double w = 1)
   {LGL.lglVertex(x,y,z,w);}

//! specify vertex color attribute (as defined by OpenGL 1.2)
inline void lglColor(const vec4f &c)
   {LGL.lglColor(c);}

//! specify vertex color attribute (as defined by OpenGL 1.2)
inline void lglColor(float r, float g, float b, float a = 1)
   {LGL.lglColor(r,g,b,a);}

//! specify vertex normal attribute (as defined by OpenGL 1.2)
inline void lglNormal(const vec3f &n)
   {LGL.lglNormal(n);}

//! specify vertex normal attribute (as defined by OpenGL 1.2)
inline void lglNormal(float x, float y, float z)
   {LGL.lglNormal(x,y,z);}

//! specify vertex texture coordinate attribute (as defined by OpenGL 1.2)
inline void lglTexCoord(const vec4f &t)
   {LGL.lglTexCoord(t);}

//! specify vertex texture coordinate attribute (as defined by OpenGL 1.2)
inline void lglTexCoord(float s, float t = 0, float r = 0, float w = 1)
   {LGL.lglTexCoord(s,t,r,w);}

//! end vertex series (as defined by OpenGL 1.2)
inline void lglEnd()
   {LGL.lglEnd();}

//! specify matrix mode (as defined by OpenGL 1.2)
inline void lglMatrixMode(lgl_matrixmode_enum mode = LGL_MODELVIEW)
   {LGL.lglMatrixMode(mode);}

//! load identity matrix (as defined by OpenGL 1.2)
inline void lglLoadIdentity()
   {LGL.lglLoadIdentity();}

//! load matrix (as defined by OpenGL 1.2)
inline void lglLoadMatrix(const mat4 &matrix)
   {LGL.lglLoadMatrix(matrix);}

//! multiply with matrix (as defined by OpenGL 1.2)
inline void lglMultMatrix(const mat4 &matrix)
   {LGL.lglMultMatrix(matrix);}

//! push matrix (as defined by OpenGL 1.2)
inline void lglPushMatrix()
   {LGL.lglPushMatrix();}

//! pop matrix (as defined by OpenGL 1.2)
inline void lglPopMatrix()
   {LGL.lglPopMatrix();}

//! multiply with scale matrix (as defined by OpenGL 1.2)
inline void lglScale(const vec4 &c)
   {LGL.lglScale(c);}

//! multiply with scale matrix (as defined by OpenGL 1.2)
inline void lglScale(double s, double t, double r, double w = 1)
   {LGL.lglScale(s,t,r,w);}

//! multiply with scale matrix (as defined by OpenGL 1.2)
inline void lglScale(double s, double w = 1)
   {LGL.lglScale(s,w);}

//! multiply with translation matrix (as defined by OpenGL 1.2)
inline void lglTranslate(const vec4 &v)
   {LGL.lglTranslate(v);}

//! multiply with translation matrix (as defined by OpenGL 1.2)
inline void lglTranslate(double x, double y, double z, double w = 1)
   {LGL.lglTranslate(x,y,z,w);}

//! multiply with rotation matrix (as defined by OpenGL 1.2)
inline void lglRotate(double angle, const vec3 &v)
   {LGL.lglRotate(angle, v);}

//! multiply with rotation matrix (as defined by OpenGL 1.2)
inline void lglRotate(double angle, double vx, double vy, double vz)
   {LGL.lglRotate(angle, vx,vy,vz);}

//! multiply with orthgraphic matrix (as defined by OpenGL 1.2)
inline void lglOrtho(double left, double right, double bottom, double top, double nearp = -1, double farp = 1)
   {LGL.lglOrtho(left, right, bottom, top, nearp, farp);}

//! multiply with frustum matrix (as defined by OpenGL 1.2)
inline void lglFrustum(double left, double right, double bottom, double top, double nearp, double farp)
   {LGL.lglFrustum(left, right, bottom, top, nearp, farp);}

//! multiply with perspective matrix (as defined by OpenGL 1.2)
inline void lglPerspective(double fovy, double aspect, double nearp, double farp)
   {LGL.lglPerspective(fovy, aspect, nearp, farp);}

//! multiply with lookat matrix (as defined by OpenGL 1.2)
inline void lglLookAt(const vec3 &eye, const vec3 &at, const vec3 &up = vec3(0,1,0))
   {LGL.lglLookAt(eye, at, up);}

//! multiply with lookat matrix (as defined by OpenGL 1.2)
inline void lglLookAt(double eye_x, double eye_y, double eye_z,
                      double at_x, double at_y, double at_z,
                      double up_x = 0, double up_y = 1, double up_z = 0)
   {LGL.lglLookAt(eye_x,eye_y,eye_z, at_x,at_y,at_z, up_x,up_y,up_z);}

//! specify viewport (as defined by OpenGL 1.2)
inline void lglViewport(int ax, int ay, int bx, int by)
   {LGL.lglViewport(ax, ay, bx, by);}

//! specify clear color (as defined by OpenGL 1.2)
inline void lglClearColor(float w=0, float a=1)
   {LGL.lglClearColor(w,w,w,a);}

//! specify clear color (as defined by OpenGL 1.2)
inline void lglClearColor(float r, float g, float b, float a=1)
   {LGL.lglClearColor(r,g,b,a);}

//! clear color and depth buffer (as defined by OpenGL 1.2)
inline void lglClear(GLuint bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
   {LGL.lglClear(bits);}

//! specify clip plane (as defined by OpenGL 1.2)
inline void lglClipPlane(vec4 equation = vec4(0,0,0,0), unsigned int n = 0, bool camera_plane = false)
   {LGL.lglClipPlane(equation, n, camera_plane);}

//! specify clip plane (as defined by OpenGL 1.2)
inline void lglClipPlane(double a, double b, double c, double d, unsigned int n = 0, bool camera_plane = false)
   {LGL.lglClipPlane(a, b, c, d, n, camera_plane);}

//! specify clip plane (as defined by OpenGL 1.2)
inline void lglClipPlane(vec3 point, vec3 normal, unsigned int n = 0, bool camera_plane = false)
   {LGL.lglClipPlane(point, normal, n, camera_plane);}

//! specify fog (as defined by OpenGL 1.2)
inline void lglFog(float density = 0.0f, vec4f color = vec4f(1))
   {LGL.lglFog(density, color);}

//! specify line width (as defined by OpenGL 1.2)
inline void lglLineWidth(float width = 1.0f)
   {LGL.lglLineWidth(width);}

//! specify polygon mode (as defined by OpenGL 1.2)
inline void lglPolygonMode(lgl_polygonmode_enum mode)
   {LGL.lglPolygonMode(mode);}

//! get error string (as defined by OpenGL 1.2)
inline std::string lglGetError()
   {return(LGL.lglGetError());}

// extended OpenGL API:

//! get actual color
inline vec4 lglGetColor()
   {return(LGL.lglGetColor());}

//! specify vertex attribute
inline void lglAttribute(const vec4f &a, unsigned int n = 0)
   {LGL.lglAttribute(a,n);}

//! specify vertex attribute
inline void lglAttribute(float x, float y, float z, float w = 1.0f, unsigned int n = 0)
   {LGL.lglAttribute(x,y,z,w, n);}

//! get actual matrix mode
inline lgl_matrixmode_enum lglGetMatrixMode()
   {return(LGL.lglGetMatrixMode());}

//! get actual matrix
inline mat4 lglGetMatrix()
   {return(LGL.lglGetMatrix());}

//! get actual projection matrix
inline mat4 lglGetProjectionMatrix()
   {return(LGL.lglGetProjectionMatrix());}

//! get actual modelview matrix
inline mat4 lglGetModelViewMatrix()
   {return(LGL.lglGetModelViewMatrix());}

//! get inverse of modelview matrix
inline mat4 lglGetInverseModelViewMatrix()
   {return(LGL.lglGetInverseModelViewMatrix());}

//! get inverse transpose of modelview matrix
inline mat4 lglGetInverseTransposeModelViewMatrix()
   {return(LGL.lglGetInverseTransposeModelViewMatrix());}

//! get combined modelview and projection matrix
inline mat4 lglGetModelViewProjectionMatrix()
   {return(LGL.lglGetModelViewProjectionMatrix());}

//! get texture matrix
inline mat4 lglGetTextureMatrix()
   {return(LGL.lglGetTextureMatrix());}

//! multiply with rotation matrix (rotation about x-axis)
inline void lglRotateX(double angle)
   {LGL.lglRotateX(angle);}

//! multiply with rotation matrix (rotation about y-axis)
inline void lglRotateY(double angle)
   {LGL.lglRotateY(angle);}

//! multiply with rotation matrix (rotation about z-axis)
inline void lglRotateZ(double angle)
   {LGL.lglRotateZ(angle);}

//! multiply with parallel projection matrix
inline void lglParallel(const vec3 &p,const vec3 &n,const vec3 &d)
   {LGL.lglParallel(p, n, d);}

//! reset the actual projection matrix
inline void lglProjection()
   {LGL.lglProjection();}

//! set the actual projection matrix
inline void lglProjection(const mat4 &projection)
   {LGL.lglProjection(projection);}

//! set the actual projection matrix
inline void lglProjection(double left, double right, double bottom, double top, double nearp, double farp)
   {LGL.lglProjection(left, right, bottom, top, nearp, farp);}

//! set the actual projection matrix
inline void lglProjection(double fovy, double aspect, double nearp, double farp)
   {LGL.lglProjection(fovy, aspect, nearp, farp);}

//! get fovy parameter for last projection transformation
inline double getFovy()
   {return(LGL.getFovy());}

//! get aspect parameter for last projection transformation
inline double getAspect()
   {return(LGL.getAspect());}

//! get near parameter for last projection transformation
inline double getNear()
   {return(LGL.getNear());}

//! get far parameter for last projection transformation
inline double getFar()
   {return(LGL.getFar());}

//! reset the actual modelview matrix
inline void lglModelView()
   {LGL.lglModelView();}

//! set the actual modelview matrix
inline void lglModelView(const mat4 &modelview)
   {LGL.lglModelView(modelview);}

//! set the actual modelview matrix
inline void lglView(const vec3 &eye, const vec3 &at, const vec3 &up = vec3(0,1,0))
   {LGL.lglView(eye, at, up);}

//! set the actual modelview matrix
inline void lglView(double eye_x, double eye_y, double eye_z,
                    double at_x, double at_y, double at_z,
                    double up_x = 0, double up_y = 1, double up_z = 0)
   {LGL.lglView(eye_x,eye_y,eye_z, at_x,at_y,at_z, up_x,up_y,up_z);}

//! get eye point for last lookat transformation
inline vec3 getEye()
   {return(LGL.getEye());}

//! get lookat point for last lookat transformation
inline vec3 getLookAt()
   {return(LGL.getLookAt());}

//! get up vector for last lookat transformation
inline vec3 getUp()
   {return(LGL.getUp());}

//! enable or disable the manipulator matrix
inline void lglManip(bool on = true)
   {LGL.lglManip(on);}

//! update the manipulator matrix by multiplying it with a custom matrix
inline void lglManip(const mat4 &manip)
   {LGL.lglManip(manip);}

//! update the manipulator matrix by applying a rotation about a focus point and a zoom factor
inline void lglManip(double focus, double angle, double tilt, double zoom)
   {LGL.lglManip(focus, angle, tilt, zoom);}

//! update the manipulator matrix by applying a screen-space delta
inline void lglManip(double dx, double dy, double zoom = 1)
   {LGL.lglManip(dx, dy, zoom);}

//! update the manipulator matrix by applying a screen-space delta
inline void lglManip(vec2 delta, double zoom = 1)
   {LGL.lglManip(delta, zoom);}

//! reset the manipulator matrix
inline void lglResetManip()
   {LGL.lglResetManip();}

//! get the manipulator matrix
inline mat4 lglGetManip()
   {return(LGL.lglGetManip());}

//! is the manipulator matrix applied?
inline bool lglIsManipApplied()
   {return(LGL.lglIsManipApplied());}

//! get the inverse transpose of the manipulator matrix
inline mat4 lglGetInverseTransposeManip()
   {return(LGL.lglGetInverseTransposeManip());}

//! set the vbo modeling matrix
inline void lglModel(const mat4 &model)
   {LGL.lglModel(model);}

//! get the vbo modeling matrix
inline mat4 lglGetModelMatrix()
   {return(LGL.lglGetModelMatrix());}

//! get the inverse transpose of the vbo modeling matrix
inline mat4 lglGetInverseTransposeModelMatrix()
   {return(LGL.lglGetInverseTransposeModelMatrix());}

//! set the vbo texturing matrix
inline void lglTex(const mat4 &tex)
   {LGL.lglTex(tex);}

//! set the vbo texturing matrix
inline void lglTex(const vec4 &scale, const vec4 &offset)
   {LGL.lglTex(scale, offset);}

//! get the vbo texturing matrix
inline mat4 lglGetTexMatrix()
   {return(LGL.lglGetTexMatrix());}

//! specify lighting parameters
inline void lglLight(vec4f light = vec4f(0,0,1,0), bool camera_light = true,
                     vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                     vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                     float exponent = 30, vec3f falloff = vec3f(1,0,0))
   {LGL.lglLight(light, camera_light, ka, kd, ks, Ia, Id, Is, exponent, falloff);}

//! initialize OpenGL state (clear color, depth test, back-face culling)
inline void lglInitializeOpenGL(float r=0, float g=0, float b=0, float a=1,
                                bool ztest = true, bool culling = false)
   {LGL.lglInitializeOpenGL(r,g,b,a, ztest, culling);}

//! change OpenGL state (rgba write)
inline void lglRGBAWrite(bool r = true, bool g = true, bool b = true, bool a = true)
   {LGL.lglRGBAWrite(r,g,b,a);}

//! get OpenGL state (rgba write)
inline bool lglGetRGBAWrite()
   {return(LGL.lglGetRGBAWrite());}

//! change OpenGL state (depth write)
inline void lglZWrite(bool on = true)
   {LGL.lglZWrite(on);}

//! get OpenGL state (depth write)
inline bool lglGetZWrite()
   {return(LGL.lglGetZWrite());}

//! change OpenGL state (depth test)
inline void lglDepthTest(bool on = false)
   {LGL.lglDepthTest(on);}

//! get OpenGL state (depth test)
inline bool lglGetDepthTest()
   {return(LGL.lglGetDepthTest());}

//! change OpenGL state (back-face culling)
inline void lglBackFaceCulling(bool on = true)
   {LGL.lglBackFaceCulling(on);}

//! get OpenGL state (back-face culling)
inline bool lglGetBackFaceCulling()
   {return(LGL.lglGetBackFaceCulling());}

//! change OpenGL state (blending)
inline void lglBlendMode(lgl_blendmode_enum mode)
   {LGL.lglBlendMode(mode);}

//! get OpenGL state (blending)
inline bool lglGetBlending()
   {return(LGL.lglGetBlending());}

//! get OpenGL state (blend mode)
inline lgl_blendmode_enum lglGetBlendMode()
   {return(LGL.lglGetBlendMode());}

//! change OpenGL state (alpha test)
inline void lglAlphaTest(bool on = false, float value = 0.0f, bool greater = true, bool equal = false)
   {LGL.lglAlphaTest(on, value, greater, equal);}

//! get OpenGL state (alpha test)
inline bool lglGetAlphaTest()
   {return(LGL.lglGetAlphaTest());}

//! get OpenGL state (alpha test value)
inline float lglGetAlphaTestValue()
   {return(LGL.lglGetAlphaTestValue());}

//! get OpenGL state (alpha test comparison mode)
inline bool lglGetAlphaTestGreater()
   {return(LGL.lglGetAlphaTestGreater());}

//! get OpenGL state (alpha test equality mode)
inline bool lglGetAlphaTestEqual()
   {return(LGL.lglGetAlphaTestEqual());}

//! get OpenGL state (clip plane)
inline bool lglGetClipPlane(unsigned int n = 0)
   {return(LGL.lglGetClipPlane(n));}

//! get OpenGL state (clip plane equation)
inline vec4 lglGetClipPlaneEquation(unsigned int n = 0)
   {return(LGL.lglGetClipPlaneEquation(n));}

//! compute OpenGL parameters (clip plane equation)
inline vec4 lglGetClipPlaneEquation(vec3 point, vec3 normal)
   {return(LGL.lglGetClipPlaneEquation(point, normal));}

//! get OpenGL state (fog)
inline bool lglGetFog()
   {return(LGL.lglGetFog());}

//! get OpenGL state (fog density)
inline float lglGetFogDensity()
   {return(LGL.lglGetFogDensity());}

//! get OpenGL state (fog color)
inline vec4f lglGetFogColor()
   {return(LGL.lglGetFogColor());}

//! get OpenGL state (line width)
inline float lglGetLineWidth()
   {return(LGL.lglGetLineWidth());}

//! toggle OpenGL state (polygon mode)
inline void lglTogglePolygonMode()
   {LGL.lglTogglePolygonMode();}

//! get OpenGL state (polygon mode)
inline lgl_polygonmode_enum lglGetPolygonMode()
   {return(LGL.lglGetPolygonMode());}

//! change OpenGL state (interlacing mode)
inline void lglInterlacingMode(lgl_interlacing_enum mode)
   {LGL.lglInterlacingMode(mode);}

//! change OpenGL state (complementary interlacing mode)
inline void lglComplementaryInterlacingMode(lgl_interlacing_enum mode)
   {LGL.lglComplementaryInterlacingMode(mode);}

//! get OpenGL state (interlacing mode)
inline lgl_interlacing_enum lglGetInterlacingMode()
   {return(LGL.lglGetInterlacingMode());}

//! get GLSL support
inline GLuint lglSupportsGLSL()
   {return(LGL.lglSupportsGLSL());}

//! get GLSL version string
inline std::string lglGetGLSLVersionString()
   {return(LGL.lglGetGLSLVersionString());}

//! get the LGL plain GLSL vertex shader
inline std::string lglPlainGLSLVertexShader()
   {return(LGL.lglPlainGLSLVertexShader());}

//! get the LGL plain GLSL fragment shader
inline std::string lglPlainGLSLFragmentShader()
   {return(LGL.lglPlainGLSLFragmentShader());}

//! get the combined LGL plain GLSL program
inline std::string lglPlainGLSLProgram()
   {return(LGL.lglPlainGLSLProgram());}

//! get the LGL default vertex shader
inline std::string lglBasicGLSLVertexShader(bool colors = false, bool normals = false, bool texcoords = false)
   {return(LGL.lglBasicGLSLVertexShader(colors, normals, texcoords));}

//! get the LGL default fragment shader
inline std::string lglBasicGLSLFragmentShader(bool colors = false, bool normals = false, bool texcoords = false, bool tex3D = false)
   {return(LGL.lglBasicGLSLFragmentShader(colors, normals, texcoords, tex3D));}

//! compile GLSL vertex shader
inline GLuint lglCompileGLSLVertexShader(std::string shader)
   {return(LGL.lglCompileGLSLVertexShader(shader));}

//! compile GLSL fragment shader
inline GLuint lglCompileGLSLFragmentShader(std::string shader)
   {return(LGL.lglCompileGLSLFragmentShader(shader));}

//! link GLSL program
inline GLuint lglLinkGLSLProgram(GLuint vertex_shader_id, GLuint fragment_shader_id, GLuint custom_shader_program = 0)
   {return(LGL.lglLinkGLSLProgram(vertex_shader_id, fragment_shader_id, custom_shader_program));}

//! compile GLSL program
inline GLuint lglCompileGLSLProgram(std::string vertex_shader, std::string fragment_shader)
   {return(LGL.lglCompileGLSLProgram(vertex_shader, fragment_shader));}

//! load GLSL program
inline GLuint lglLoadGLSLProgram(const char *vertex_shader_file, const char *fragment_shader_file)
   {return(LGL.lglLoadGLSLProgram(vertex_shader_file, fragment_shader_file));}

//! combine GLSL program
inline std::string lglCombineGLSLProgram(const std::string &vertex_shader, const std::string &fragment_shader)
   {return(LGL.lglCombineGLSLProgram(vertex_shader, fragment_shader));}

//! split GLSL program
inline bool lglSplitGLSLProgram(std::string shader, std::string &vertex_shader, std::string &fragment_shader)
   {return(LGL.lglSplitGLSLProgram(shader, vertex_shader, fragment_shader));}

//! compile combined GLSL program
inline GLuint lglCompileGLSLProgram(std::string shader)
   {return(LGL.lglCompileGLSLProgram(shader));}

//! load combined GLSL program
inline GLuint lglLoadGLSLProgram(const char *shader_file)
   {return(LGL.lglLoadGLSLProgram(shader_file));}

//! delete GLSL shader
inline void lglDeleteGLSLShader(GLuint shader_id)
   {LGL.lglDeleteGLSLShader(shader_id);}

//! delete GLSL program
inline void lglDeleteGLSLProgram(GLuint program)
   {LGL.lglDeleteGLSLProgram(program);}

//! use GLSL program
inline void lglUseProgram(GLuint program, bool clear = true)
   {LGL.lglUseProgram(program, clear);}

//! use default GLSL program
inline void lglUseDefaultProgram(bool clear = true)
   {LGL.lglUseDefaultProgram(clear);}

//! replace default GLSL program
inline void lglReplaceDefaultProgram(GLuint program, bool colors = false, bool normals = true, bool texcoords = false, bool tex3D = false)
   {LGL.lglReplaceDefaultProgram(program, colors, normals, texcoords, tex3D);}

//! get actual GLSL program
inline GLuint lglGetProgram()
   {return(LGL.lglGetProgram());}

//! get active GLSL program
inline GLuint lglGetActiveProgram()
   {return(LGL.lglGetActiveProgram());}

//! reuse GLSL program
inline void lglReuseProgram(GLuint program)
   {LGL.lglReuseProgram(program);}

//! specify GLSL program uniform (undefined)
inline unsigned int lglUniform(std::string uniform)
   {return(LGL.lglUniform(uniform));}

//! specify GLSL program uniform (integer)
inline unsigned int lglUniformi(std::string uniform, int value)
   {return(LGL.lglUniformi(uniform, value));}

//! specify GLSL program uniform (integer, indexed)
inline void lglUniformi(unsigned int index, int value)
   {LGL.lglUniformi(index, value);}

//! specify GLSL program uniform (float)
inline unsigned int lglUniformf(std::string uniform, double value)
   {return(LGL.lglUniformf(uniform, value));}

//! specify GLSL program uniform (float, indexed)
inline void lglUniformf(unsigned int index, double value)
   {LGL.lglUniformf(index, value);}

//! specify GLSL program uniform (4xfloat)
inline unsigned int lglUniformf(std::string uniform, double x, double y, double z, double w = 1)
   {return(LGL.lglUniformf(uniform, x, y, z, w));}

//! specify GLSL program uniform (4xfloat, indexed)
inline void lglUniformf(unsigned int index, double x, double y, double z, double w = 1)
   {LGL.lglUniformf(index, x, y, z, w);}

//! specify GLSL program uniform (vec2f)
inline unsigned int lglUniformfv(std::string uniform, const vec2f &value)
   {return(LGL.lglUniformfv(uniform, value));}

//! specify GLSL program uniform (vec2f, indexed)
inline void lglUniformfv(unsigned int index, const vec2f &value)
   {LGL.lglUniformfv(index, value);}

inline unsigned int lglUniformfv(std::string uniform, const vec2 &value)
   {return(LGL.lglUniformfv(uniform, value));}

inline void lglUniformfv(unsigned int index, const vec2 &value)
   {LGL.lglUniformfv(index, value);}

//! specify GLSL program uniform (vec3f)
inline unsigned int lglUniformfv(std::string uniform, const vec3f &value)
   {return(LGL.lglUniformfv(uniform, value));}

//! specify GLSL program uniform (vec3f, indexed)
inline void lglUniformfv(unsigned int index, const vec3f &value)
   {LGL.lglUniformfv(index, value);}

inline unsigned int lglUniformfv(std::string uniform, const vec3 &value)
   {return(LGL.lglUniformfv(uniform, value));}

inline void lglUniformfv(unsigned int index, const vec3 &value)
   {LGL.lglUniformfv(index, value);}

//! specify GLSL program uniform (vec4f)
inline unsigned int lglUniformfv(std::string uniform, const vec4f &value)
   {return(LGL.lglUniformfv(uniform, value));}

//! specify GLSL program uniform (vec4f, indexed)
inline void lglUniformfv(unsigned int index, const vec4f &value)
   {LGL.lglUniformfv(index, value);}

inline unsigned int lglUniformfv(std::string uniform, const vec4 &value)
   {return(LGL.lglUniformfv(uniform, value));}

inline void lglUniformfv(unsigned int index, const vec4 &value)
   {LGL.lglUniformfv(index, value);}

//! specify GLSL program uniform matrix (mat2f)
inline unsigned int lglUniformfv(std::string uniform, const mat2f &value)
   {return(LGL.lglUniformfv(uniform, value));}

//! specify GLSL program uniform matrix (mat2f, indexed)
inline void lglUniformfv(unsigned int index, const mat2f &value)
   {LGL.lglUniformfv(index, value);}

inline unsigned int lglUniformfv(std::string uniform, const mat2 &value)
   {return(LGL.lglUniformfv(uniform, value));}

inline void lglUniformfv(unsigned int index, const mat2 &value)
   {LGL.lglUniformfv(index, value);}

//! specify GLSL program uniform matrix (mat3f)
inline unsigned int lglUniformfv(std::string uniform, const mat3f &value)
   {return(LGL.lglUniformfv(uniform, value));}

//! specify GLSL program uniform matrix (mat3f, indexed)
inline void lglUniformfv(unsigned int index, const mat3f &value)
   {LGL.lglUniformfv(index, value);}

inline unsigned int lglUniformfv(std::string uniform, const mat3 &value)
   {return(LGL.lglUniformfv(uniform, value));}

inline void lglUniformfv(unsigned int index, const mat3 &value)
   {LGL.lglUniformfv(index, value);}

//! specify GLSL program uniform matrix (mat4f)
inline unsigned int lglUniformfv(std::string uniform, const mat4f &value)
   {return(LGL.lglUniformfv(uniform, value));}

//! specify GLSL program uniform matrix (mat4f, indexed)
inline void lglUniformfv(unsigned int index, const mat4f &value)
   {LGL.lglUniformfv(index, value);}

inline unsigned int lglUniformfv(std::string uniform, const mat4 &value)
   {return(LGL.lglUniformfv(uniform, value));}

inline void lglUniformfv(unsigned int index, const mat4 &value)
   {LGL.lglUniformfv(index, value);}

//! specify GLSL program uniform sampler
inline unsigned int lglSampler(std::string sampler, int value = 0, bool warn = true)
   {return(LGL.lglSampler(sampler, value, warn));}

//! specify GLSL program uniform sampler (indexed)
inline void lglSampler(unsigned int index, int value = 0, bool warn = true)
   {LGL.lglSampler(index, value, warn);}

//! bind 2D sampler
inline unsigned int lglSampler2D(std::string sampler, GLuint texid2D, int value = 0, bool warn = true)
   {return(LGL.lglSampler2D(sampler, texid2D, value, warn));}

//! bind 2D sampler (indexed)
inline void lglSampler2D(unsigned int index, GLuint texid2D, int value = 0, bool warn = true)
   {LGL.lglSampler2D(index, texid2D, value, warn);}

//! bind 3D sampler
inline unsigned int lglSampler3D(std::string sampler, GLuint texid3D, int value = 0, bool warn = true)
   {return(LGL.lglSampler3D(sampler, texid3D, value, warn));}

//! bind 3D sampler (indexed)
inline void lglSampler3D(unsigned int index, GLuint texid3D, int value = 0, bool warn = true)
   {LGL.lglSampler3D(index, texid3D, value, warn);}

//! print GLSL uniforms
inline void lglPrintUniforms()
   {LGL.lglPrintUniforms();}

//! set light direction
inline void lglLightDirection(vec3f light = vec3f(0,0,1), bool camera_light = true)
   {LGL.lglLightDirection(light, camera_light);}

//! set light position
inline void lglLightPosition(vec3f light = vec3f(0,0,0), bool camera_light = true)
   {LGL.lglLightPosition(light, camera_light);}

//! set light vector
inline void lglLightVector(vec4f light = vec4f(0,0,1,0), bool camera_light = true)
   {LGL.lglLightVector(light, camera_light);}

//! get light vector (in camera coordinates)
inline vec4f lglGetLightVector()
   {return(LGL.lglGetLightVector());}

//! set light parameters
inline void lglLightParameters(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                               vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                               float exponent = 30, vec3f falloff = vec3f(1,0,0))
   {LGL.lglLightParameters(ka, kd, ks, Ia, Id, Is, exponent, falloff);}

//! get light parameters
inline void lglGetLightParameters(vec3f &ka, vec3f &kd, vec3f &ks,
                                  vec3f &Ia, vec3f &Id, vec3f &Is,
                                  float &exponent, vec3f &falloff)
   {LGL.lglGetLightParameters(ka, kd, ks, Ia, Id, Is, exponent, falloff);}

//! set light source parameters
inline void lglLightSourceParameters(vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                                     vec3f falloff = vec3f(1,0,0))
   {LGL.lglLightSourceParameters(Ia, Id, Is, falloff);}

//! get light source parameters
inline void lglGetLightSourceParameters(vec3f &Ia, vec3f &Id, vec3f &Is,
                                        vec3f &falloff)
   {LGL.lglGetLightSourceParameters(Ia, Id, Is, falloff);}

//! set material parameters
inline void lglMaterialParameters(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                                  float exponent = 30)
   {LGL.lglMaterialParameters(ka, kd, ks, exponent);}

//! get material parameters
inline void lglGetMaterialParameters(vec3f &ka, vec3f &kd, vec3f &ks,
                                     float &exponent)
   {LGL.lglGetMaterialParameters(ka, kd, ks, exponent);}

//! bind 2D texture
inline void lglTexture2D(GLuint texid)
   {LGL.lglTexture2D(texid);}

//! get 2D texture
inline GLuint lglGetTexture2D()
   {return(LGL.lglGetTexture2D());}

//! bind 3D texture
inline void lglTexture3D(GLuint texid)
   {LGL.lglTexture3D(texid);}

//! get 3D texture
inline GLuint lglGetTexture3D()
   {return(LGL.lglGetTexture3D());}

//! enable texture coordinate generation
inline void lglTexCoordGen(lgl_texgenmode_enum mode = LGL_TEXGEN_LINEAR)
   {LGL.lglTexCoordGen(mode);}

//! check for texture coordinate generation
inline lgl_texgenmode_enum lglGetTexCoordGen()
   {return(LGL.lglGetTexCoordGen());}

//! enable coloring
inline void lglColoring(bool on)
   {LGL.lglColoring(on);}

//! check for coloring
inline bool lglGetColoring()
   {return(LGL.lglGetColoring());}

//! enable lighting
inline void lglLighting(bool on)
   {LGL.lglLighting(on);}

//! check for lighting
inline bool lglGetLighting()
   {return(LGL.lglGetLighting());}

//! enable texturing
inline void lglTexturing(bool on)
   {LGL.lglTexturing(on);}

//! check for texturing
inline bool lglGetTexturing()
   {return(LGL.lglGetTexturing());}

//! entirely disable vertex colors
inline void lglDisableColoring(bool off)
   {LGL.lglDisableColoring(off);}

//! entirely disable vertex normals
inline void lglDisableLighting(bool off)
   {LGL.lglDisableLighting(off);}

//! entirely disable texture coordinates
inline void lglDisableTexturing(bool off)
   {LGL.lglDisableTexturing(off);}

//! render vbo (by copying the global state)
inline void lglRender(lglVBO *vbo, const GL *gl = &LGL)
   {vbo->lglRender(gl);}

//! render vbo (by copying the global state)
inline void lglRender(lglVBO &vbo, const GL *gl = &LGL)
   {vbo.lglRender(gl);}

//! clone state from vbo (coloring, lighting and texturing)
inline void lglCloneState(const GL *gl)
   {LGL.lglCloneState(gl);}

//! clone state from vbo (coloring, lighting and texturing)
inline void lglCloneState(const lglVBO *vbo)
   {LGL.lglCloneState(vbo);}

//! copy uniforms from vbo
inline void lglCopyUniforms(const GL *gl, bool warn = false)
   {LGL.lglCopyUniforms(gl);}

//! copy uniforms from vbo
inline void lglCopyUniforms(const lglVBO *vbo, bool warn = false)
   {LGL.lglCopyUniforms(vbo);}

//! read frame buffer
inline unsigned char *lglReadRGBPixels(int x, int y, int width, int height)
   {return(LGL.lglReadRGBPixels(x, y, width, height));}

//! begin ray casting in view coordinates
inline void lglBeginRayCast(vec3 origin, vec3 direction, double mindist = 0)
   {LGL.lglBeginRayCast(origin, direction, mindist);}

//! cast a ray and intersect it with the transformed triangles contained in the vbo
inline void lglRayCast(lglVBO *vbo)
   {LGL.lglRayCast(vbo);}

//! end ray casting
inline LGL_VBO_TYPE *lglEndRayCast()
   {return(LGL.lglEndRayCast());}

//! get distance to front-most hit point determined by ray casting
inline double lglGetRayCastDistance()
   {return(LGL.lglGetRayCastDistance());}

//! enable view culling
inline void lglEnableViewCulling(int minsize = 0)
   {LGL.lglEnableViewCulling(minsize);}

//! disable view culling
inline void lglDisableViewCulling()
   {LGL.lglDisableViewCulling();}

//! begin scene export
inline void lglBeginExport(void (*callback)(LGL_VBO_TYPE *vbo, void *data), void *data, mat4 m = mat4(1))
   {LGL.lglBeginExport(callback, data, m);}

//! end scene export
inline void lglEndExport()
   {LGL.lglEndExport();}

//! get OpenGL version
inline int lglGetGLVersion()
   {return(LGL.lglGetGLVersion());}

//! map gl version to glsl version
inline int lglMapGL2GLSLVersion(int glversion)
   {return(LGL.lglMapGL2GLSLVersion(glversion));}

//! get GLSL version
inline int lglGetGLSLVersion()
   {return(LGL.lglGetGLSLVersion());}

//! get OpenGL renderer
inline std::string lglGetRenderer()
   {return(LGL.lglGetRenderer());}

//! get OpenGL vendor
inline lgl_vendor_enum lglGetVendor()
   {return(LGL.lglGetVendor());}

//! get OpenGL vendor name
inline std::string lglGetVendorName()
   {return(LGL.lglGetVendorName());}

// wrapped extension function
inline void lglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
   {LGL.lglTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);}

// wrapped extension function
inline void lglTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
   {LGL.lglTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);}

//! generate an error message
inline void lglError(std::string e)
   {LGL.lglError(e);}

//! generate a warning message
inline void lglWarning(std::string w)
   {LGL.lglWarning(w);}

//! generate a non-recoverable error
inline void lglFatal(std::string e)
   {LGL.lglFatal(e);}

//! get warning message
inline std::string lglGetWarning()
   {return(LGL.lglGetWarning());}

//! change verbosity level
inline void lglVerbosity(lgl_verbosity_enum v)
   {LGL.lglVerbosity(v);}

// scoped matrix stack
class lglScopedMatrixStack
{
public:

   lglScopedMatrixStack()
      {lglPushMatrix();}

   ~lglScopedMatrixStack()
      {lglPopMatrix();}

};

//! scoped replacement for lglPushMatrix/lglPopMatrix pairs
#define lglPushMatrixScoped() lglScopedMatrixStack __lglPushMatrixScoped__

//! random macro
#define lglRandom() drand48()

//! seed macro
#define lglSeed() srand48((unsigned int)time(NULL))

//! warning macro that outputs the file name and the line number of the occurrence
#define LGL_WARN(s) lglWarn(std::string(s)+" @ "+__FILE__+":"+glslmath::to_string(__LINE__))

//! error macro that outputs the file name and the line number of the occurrence
#define LGL_ERROR(s) lglError(std::string(s)+" @ "+__FILE__+":"+glslmath::to_string(__LINE__))

#ifdef LGL_CORE

// explicitly disallow the usage of all OpenGL legacy functions
#define glVertex2f(x,y) LGL_ERROR("use lglVertex instead of glVertex2f")
#define glVertex2d(x,y) LGL_ERROR("use lglVertex instead of glVertex2d")
#define glVertex3f(x,y,z) LGL_ERROR("use lglVertex instead of glVertex3f")
#define glVertex3d(x,y,z) LGL_ERROR("use lglVertex instead of glVertex3d")
#define glVertex4f(x,y,z,w) LGL_ERROR("use lglVertex instead of glVertex4f")
#define glVertex4d(x,y,z,w) LGL_ERROR("use lglVertex instead of glVertex4d")
#define glColor3f(x,y,z) LGL_ERROR("use lglColor instead of glColor3f")
#define glColor3d(x,y,z) LGL_ERROR("use lglColor instead of glColor3d")
#define glNormal3f(x,y,z) LGL_ERROR("use lglNormal instead of glNormal3f")
#define glNormal3d(x,y,z) LGL_ERROR("use lglNormal instead of glNormal3d")
#define glTexCoord2f(x,y) LGL_ERROR("use lglTexCoord instead of glTexCoord2f")
#define glTexCoord2d(x,y) LGL_ERROR("use lglTexCoord instead of glTexCoord2d")
#define glTexCoord3f(x,y,z) LGL_ERROR("use lglTexCoord instead of glTexCoord3f")
#define glTexCoord3d(x,y,z) LGL_ERROR("use lglTexCoord instead of glTexCoord3d")
#define glTexCoord4f(x,y,z,w) LGL_ERROR("use lglTexCoord instead of glTexCoord4f")
#define glTexCoord4d(x,y,z,w) LGL_ERROR("use lglTexCoord instead of glTexCoord4d")
#define glMatrixMode(m) LGL_ERROR("use lglMatrixMode instead of glMatrixMode")
#define glLoadIdentity(m) LGL_ERROR("use lglLoadIdentity instead of glLoadIdentity")
#define glLoadMatrixf(m) LGL_ERROR("use lglLoadMatrix instead of glLoadMatrixf")
#define glLoadMatrixd(m) LGL_ERROR("use lglLoadMatrix instead of glLoadMatrixd")
#define glMultMatrixf(m) LGL_ERROR("use lglMultMatrix instead of glMultMatrixf")
#define glMultMatrixd(m) LGL_ERROR("use lglMultMatrix instead of glMultMatrixd")
#define glPushMatrix() LGL_ERROR("use lglPushMatrix instead of glPushMatrix")
#define glPopMatrix() LGL_ERROR("use lglPopMatrix instead of glPopMatrix")
#define glTranslatef(x,y,z) LGL_ERROR("use lglTranslate instead of glTranslatef")
#define glTranslated(x,y,z) LGL_ERROR("use lglTranslate instead of glTranslated")
#define glRotatef(a,x,y,z) LGL_ERROR("use lglRotate instead of glRotatef")
#define glRotated(a,x,y,z) LGL_ERROR("use lglRotate instead of glRotated")
#define glScalef(x,y,z) LGL_ERROR("use lglScale instead of glScalef")
#define glScaled(x,y,z) LGL_ERROR("use lglScale instead of glScaled")
#define glOrtho(l,r,b,t,n,f) LGL_ERROR("use lglOrtho instead of glOrtho")

#ifdef GLU_VERSION
#define gluPerspective(fy,ar,np,fp) LGL_ERROR("use lglPerspective instead of gluPerspective")
#define gluLookAt(ex,ey,ez,ax,ay,az,ux,uy,uz) LGL_ERROR("use lglLookAt instead of gluLookAt")
#endif

#endif

#endif
