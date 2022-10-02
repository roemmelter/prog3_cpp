// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_QT_GLSLUI_H
#define GLVERTEX_QT_GLSLUI_H

#include "glvertex_qt_glui.h"

//! Qt GLSL convenience class
class lgl_Qt_GLSLUI: public lgl_Qt_GLUI
{
public:

   lgl_Qt_GLSLUI(QString name, QString filename,
                 QWidget *parent = NULL, bool core = true)
      : lgl_Qt_GLUI(parent, core),
        name_(name),
        filename_(filename)
   {
      cube_ = NULL;
      sphere_ = NULL;

      id_ = 0;
      editor_ = NULL;
   }

   virtual ~lgl_Qt_GLSLUI()
   {
      if (cube_)
         delete cube_;

      if (sphere_)
         delete sphere_;

      if (id_ != 0)
         lglDeleteGLSLProgram(id_);
   }

protected:

   QString name_;
   QString filename_;

   lglVBO *cube_;
   lglVBO *sphere_;

   GLuint id_;
   lgl_Qt_ShaderEditor *editor_;

   void initializeOpenGL()
   {
      // setup OpenGL
      lglInitializeOpenGL(1,1,1);

      // create cube
      cube_ = new lglCube(true);

      // create sphere
      sphere_ = new lglSphere(true);

      // compile shader
      id_ = lglCompileGLSLProgram(shader);

      // create shader editor
      if (name_ != "")
      {
         editor_ = new lgl_Qt_ShaderEditor(name_, filename_, &id_, this, Qt::Window);
         editor_->resize(640, 512);
         editor_->show();
      }

      // enable shader
      cube_->lglUseProgram(id_);
      sphere_->lglUseProgram(id_);

      initializeGLSL(id_, cube_, sphere_);
   }

   //! reimplement to initialize OpenGL resp. GLSL state
   virtual void initializeGLSL(GLuint id, lglVBO *cube, lglVBO *sphere)
   {}

   void renderOpenGL(double dt)
   {
      // clear frame buffer
      lglClear();

      renderGLSL(dt, id_, cube_, sphere_);

      // check for errors
      std::string error = lglGetError();
      if (!error.empty()) lglFatal("OpenGL error detected - exiting");
   }

   //! reimplement to render OpenGL resp. GLSL scene
   virtual void renderGLSL(double dt, GLuint id, lglVBO *cube, lglVBO *sphere)
   {
      static double angle=0; // rotation angle in degrees
      static const double omega=10; // rotation speed in degrees/s
      angle+=dt*omega;

      // projection setup
      double fovy = 60;
      double aspect = (double)width()/height();
      double nearp = 0.1;
      double farp = 10;
      lglProjection(fovy, aspect, nearp, farp);

      // viewing setup
      lglView(0,0,0, 0,0,-3, 0,1,0);

      // reenable shader /wo clearing uniforms
      cube_->lglUseProgram(id, false);
      sphere_->lglUseProgram(id, false);

      // render glsl scene
      lglPushMatrix();
      lglTranslate(vec3(0,0,-3));
      lglRotate(angle, vec3(0,1,0));
      cube->lglColor(0,1,0);
      cube->lglRender();
      lglPopMatrix();
      lglPushMatrix();
      lglTranslate(vec3(-1.3,0,-3));
      lglRotate(angle, vec3(0,1,0));
      sphere->lglColor(1,0,0);
      sphere->lglRender();
      lglPopMatrix();
      lglPushMatrix();
      lglTranslate(vec3(1.3,0,-3));
      lglRotate(angle, vec3(0,1,0));
      sphere->lglColor(0,0,1);
      sphere->lglRender();
      lglPopMatrix();
   }

   void keyPressed(char key) {}
};

#define lgl_Qt_GLSLUI(name) lgl_Qt_GLSLUI(name, __FILE__)

#endif
