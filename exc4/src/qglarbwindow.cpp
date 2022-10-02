// (c) by Stefan Roettger, licensed under MIT license

#include "math.h"

#include "qglarbwindow.h"

#include "gfx/gl.h"
#include "gfx/shaderprogram.h"
#include "gfx/teapot.h"

QGLARBWindow::QGLARBWindow(QWidget *parent)
   : QOGLWidget(parent),
     teapot_(false),
     vtx_modified(false), frg_modified(false),
     vtx_id(0), frg_id(0)
{}

QGLARBWindow::~QGLARBWindow()
{
   if (vtx_id != 0) deletevtxprog(vtx_id);
   if (frg_id != 0) deletefrgprog(frg_id);
}

void QGLARBWindow::enableTeapot(bool on)
{
   if (on != teapot_)
   {
      teapot_ = on;
      update();
   }
}

void QGLARBWindow::setARBVertexProgram(std::string shader)
{
   if (shader != vtx_shader)
   {
      vtx_shader = shader;
      vtx_modified = true;
      update();
   }
}

void QGLARBWindow::setARBFragmentProgram(std::string shader)
{
   if (shader != frg_shader)
   {
      frg_shader = shader;
      frg_modified = true;
      update();
   }
}

void QGLARBWindow::bindPrograms()
{
   if (vtx_modified)
   {
      if (vtx_id != 0) deletevtxprog(vtx_id);
      vtx_id = buildvtxprog(vtx_shader.c_str(),false);
      vtx_modified = false;
   }

   if (frg_modified)
   {
      if (frg_id != 0) deletefrgprog(frg_id);
      frg_id = buildfrgprog(frg_shader.c_str(),false);
      frg_modified = false;
   }

   bindvtxprog(vtx_id);
   bindfrgprog(frg_id);

   emit vtxstatus(getvtxerror());
   emit frgstatus(getfrgerror());
}

void QGLARBWindow::initializeOpenGL()
{
   glClearColor(0,0,0,1);
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);

   GLubyte checkerboard[8*8] =
      {234,0,234,0,234,0,234,0,
       0,234,0,234,0,234,0,234,
       234,0,234,0,234,0,234,0,
       0,234,0,234,0,234,0,234,
       234,0,234,0,234,0,234,0,
       0,234,0,234,0,234,0,234,
       234,0,234,0,234,0,234,0,
       0,234,0,234,0,234,0,234};

   glTexImage2D(GL_TEXTURE_2D,    // 2D texture
                0,                // level 0 (for mipmapping)
                GL_LUMINANCE,     // pixel format of texture
                8,8,              // texture size
                0,                // border size 0
                GL_LUMINANCE,     // pixel format of data supplied
                GL_UNSIGNED_BYTE, // pixel storage type of data supplied
                checkerboard);    // pointer to data chunk

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

   const int ts = 64;
   GLubyte volume[ts*ts*ts];

   for (int i=0; i<ts; i++)
      for (int j=0; j<ts; j++)
         for (int k=0; k<ts; k++)
         {
             double v = 0.8+0.2*drand48();

             if (k%(ts/2) == 0) v = 0.25;
             if (k%(ts/2) == 1) v = 0.25;

             if (j%(ts/4) == 0) v = 0.25;
             if (j%(ts/4) == 1) v = 0.25;

             if ((j/(ts/4))%2 == (k/(ts/2))%2)
             {
                if (i%(ts/2) == 0) v = 0.25;
                if (i%(ts/2) == 1) v = 0.25;
             }
             else
             {
                if (i%(ts/2) == ts/4) v = 0.25;
                if (i%(ts/2) == ts/4+1) v = 0.25;
             }

             volume[i+(j+k*ts)*ts] = floor(255*v+0.5);
         }

   glActiveTexture(GL_TEXTURE1);

   glTexImage3D(GL_TEXTURE_3D,    // 3D texture
                0,                // level 0 (for mipmapping)
                GL_LUMINANCE,     // pixel format of texture
                ts,ts,ts,         // texture size
                0,                // border size 0
                GL_LUMINANCE,     // pixel format of data supplied
                GL_UNSIGNED_BYTE, // pixel storage type of data supplied
                volume);          // pointer to data chunk

   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_REPEAT);

   glActiveTexture(GL_TEXTURE0);
}

void QGLARBWindow::renderOpenGL(double dt)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // setup perspective matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, (float)width()/height(), 0.1, 10);

   // setup modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0,0,2, 0,0,0, 0,1,0);

   // define local rotated coordinate system
   static double angle=0; // rotation angle in degrees
   static const double omega=10; // rotation speed in degrees/s
   glRotated(angle/50, 1,0,0);
   glRotated(angle, 0,1,0);

   // bind vertex and fragment programs
   bindPrograms();

   if (teapot_)
   {
      // render teapot
      glColor3f(0.6f,0.75f,0.9f);
      glTranslatef(0.0f,-0.25f,0.0f);
      glScalef(0.25f,0.25f,0.25f);
      drawTeapot();
   }
   else
   {
      // render cube
      glBegin(GL_QUADS);
         // left quad
         glColor3f(1,0,0);
         glNormal3f(-1,0,0);
         glTexCoord2f(0,0);
         glVertex3d(-0.5,-0.5,-0.5);
         glTexCoord2f(0,1);
         glVertex3d(-0.5,-0.5,0.5);
         glTexCoord2f(-1,1);
         glVertex3d(-0.5,0.5,0.5);
         glTexCoord2f(-1,0);
         glVertex3d(-0.5,0.5,-0.5);
         // right quad
         glColor3f(0,1,1);
         glNormal3f(1,0,0);
         glTexCoord2f(1,0);
         glVertex3d(0.5,-0.5,0.5);
         glTexCoord2f(1,1);
         glVertex3d(0.5,-0.5,-0.5);
         glTexCoord2f(2,1);
         glVertex3d(0.5,0.5,-0.5);
         glTexCoord2f(2,0);
         glVertex3d(0.5,0.5,0.5);
         // front quad
         glColor3f(0,1,0);
         glNormal3f(0,0,1);
         glTexCoord2f(0,0);
         glVertex3d(-0.5,-0.5,0.5);
         glTexCoord2f(1,0);
         glVertex3d(0.5,-0.5,0.5);
         glTexCoord2f(1,-1);
         glVertex3d(0.5,0.5,0.5);
         glTexCoord2f(0,-1);
         glVertex3d(-0.5,0.5,0.5);
         // back quad
         glColor3f(1,0,1);
         glNormal3f(0,0,-1);
         glTexCoord2f(1,1);
         glVertex3d(0.5,-0.5,-0.5);
         glTexCoord2f(0,1);
         glVertex3d(-0.5,-0.5,-0.5);
         glTexCoord2f(0,2);
         glVertex3d(-0.5,0.5,-0.5);
         glTexCoord2f(1,2);
         glVertex3d(0.5,0.5,-0.5);
         // bottom quad
         glColor3f(0,0,1);
         glNormal3f(0,-1,0);
         glTexCoord2f(0,1);
         glVertex3d(-0.5,-0.5,-0.5);
         glTexCoord2f(1,1);
         glVertex3d(0.5,-0.5,-0.5);
         glTexCoord2f(1,0);
         glVertex3d(0.5,-0.5,0.5);
         glTexCoord2f(0,0);
         glVertex3d(-0.5,-0.5,0.5);
         // top quad
         glColor3f(1,1,0);
         glNormal3f(0,1,0);
         glTexCoord2f(3,0);
         glVertex3d(-0.5,0.5,0.5);
         glTexCoord2f(2,0);
         glVertex3d(0.5,0.5,0.5);
         glTexCoord2f(2,1);
         glVertex3d(0.5,0.5,-0.5);
         glTexCoord2f(3,1);
         glVertex3d(-0.5,0.5,-0.5);
      glEnd();
   }

   // angle delta equals time delta times omega
   angle+=dt*omega;
}
