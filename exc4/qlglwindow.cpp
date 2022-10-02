// (c) by Stefan Roettger, licensed under MIT license

#include "qlglwindow.h"

void QLGLWindow::initializeOpenGL()
{
   glClearColor(0,0,0,1);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
}

void QLGLWindow::renderOpenGL(double dt)
{
   // clear frame buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // setup perspective matrix
   lglMatrixMode(LGL_PROJECTION);
   lglLoadIdentity();
   lglPerspective(90, (float)width()/height(), 0.1, 10);

   // setup model-view matrix
   lglMatrixMode(LGL_MODELVIEW);
   lglLoadIdentity();
   lglLookAt(0,0,0, 0,0,-2, 0,1,0);

   // define local rotated coordinate system
   static double angle=0; // rotation angle in degrees
   static const double omega=180; // rotation speed in degrees/s
   lglTranslate(0,0,-2);
   lglRotate(angle, 0,1,0);

   // render triangles
   lglBegin(LGL_TRIANGLES);
      lglColor(0,0.75,0);
      lglVertex(-0.5,-0.5,0);
      lglVertex(0.5,-0.5,0);
      lglVertex(0,0.5,0);
      lglColor(0.75,0.75,0.75);
      lglVertex(0,-0.5,-0.5);
      lglVertex(0,-0.5,0.5);
      lglVertex(0,0.5,0);
   lglEnd();

   // angle delta equals time delta times omega
   angle+=dt*omega;
}
