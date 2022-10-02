// (c) by Stefan Roettger, licensed under MIT license

#include "qglwindow.h"

void QGLWindow::initializeOpenGL()
{
   glClearColor(0,0,0,1);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
}

void QGLWindow::renderOpenGL(double dt)
{
   // clear frame buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // setup perspective matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(90, (float)width()/height(), 0.1, 10);

   // setup model-view matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0,0,0, 0,0,-2, 0,1,0);

   // define local rotated coordinate system
   static double angle=0; // rotation angle in degrees
   static const double omega=180; // rotation speed in degrees/s
   glTranslated(0,0,-2);
   glRotated(angle, 0,1,0);

   // render triangles
   glBegin(GL_TRIANGLES);
      glColor3d(0,0.75,0);
      glVertex3d(-0.5,-0.5,0);
      glVertex3d(0.5,-0.5,0);
      glVertex3d(0,0.5,0);
      glColor3d(0.75,0.75,0.75);
      glVertex3d(0,-0.5,-0.5);
      glVertex3d(0,-0.5,0.5);
      glVertex3d(0,0.5,0);
   glEnd();

   // angle delta equals time delta times omega
   angle+=dt*omega;
}
