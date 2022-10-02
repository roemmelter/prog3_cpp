// (c) by Stefan Roettger, licensed under MIT license

#include "qglglslwindow.h"

QGLGLSLWindow::QGLGLSLWindow(QWidget *parent)
   : lgl_Qt_GLWidget(parent, false),
     vbo(NULL),
     vtx_modified(false), frg_modified(false),
     shader_id(0)
{
   buttonDown = false;
}

QGLGLSLWindow::~QGLGLSLWindow()
{
   if (vbo)
      delete vbo;

   if (shader_id != 0)
      lglDeleteGLSLProgram(shader_id);
}

void QGLGLSLWindow::loadObj(const char *path)
{
   if (vbo)
      delete vbo;

   vbo = lglLoadObj(path);
   update();
}

void QGLGLSLWindow::resetObj()
{
   if (vbo)
      delete vbo;

   vbo = NULL;
   update();
}

void QGLGLSLWindow::setGLSLVertexShader(std::string shader)
{
   if (shader != vtx_shader)
   {
      vtx_shader = shader;
      vtx_modified = true;
      update();
   }
}

void QGLGLSLWindow::setGLSLFragmentShader(std::string shader)
{
   if (shader != frg_shader)
   {
      frg_shader = shader;
      frg_modified = true;
      update();
   }
}

void QGLGLSLWindow::bindShaders()
{
   if (vtx_modified || frg_modified)
   {
      if (shader_id != 0)
      {
         lglDeleteGLSLProgram(shader_id);
         if (vbo) vbo->lglUseDefaultProgram();
         shader_id = 0;
      }

      GLuint vertex_shader_id = lglCompileGLSLVertexShader(vtx_shader.c_str());
      emit vtxstatus(lglGetError().c_str());

      GLuint fragment_shader_id = lglCompileGLSLFragmentShader(frg_shader.c_str());
      emit frgstatus(lglGetError().c_str());

      if (vertex_shader_id!=0 && fragment_shader_id!=0)
      {
         shader_id = lglLinkGLSLProgram(vertex_shader_id, fragment_shader_id);
         emit lnkstatus(lglGetError().c_str());

         lglDeleteGLSLShader(vertex_shader_id);
         lglDeleteGLSLShader(fragment_shader_id);
      }
      else
         emit lnkstatus("unable to link");

      vtx_modified = frg_modified = false;
   }

   if (shader_id != 0)
      if (vbo) vbo->lglUseProgram(shader_id);
}

void QGLGLSLWindow::initializeOpenGL()
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

void QGLGLSLWindow::renderOpenGL(double dt)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // setup perspective matrix
   lglMatrixMode(LGL_PROJECTION);
   lglLoadIdentity();
   lglPerspective(60, (float)width()/height(), 0.1, 10);

   // setup modelview matrix
   lglMatrixMode(LGL_MODELVIEW);
   lglLoadIdentity();
   lglLookAt(0,0,2, 0,0,0, 0,1,0);

   // define local rotated coordinate system
   static double angle=0; // rotation angle in degrees
   static const double omega=10; // rotation speed in degrees/s
   lglRotate(angle/50, 1,0,0);
   lglRotate(angle, 0,1,0);

   // create unit cube
   if (!vbo)
      vbo = new lglCube(true);

   // bind vertex and fragment shaders
   bindShaders();

   // vertex color
   lglColor(1,1,1);

   // render vbo
   lglPushMatrixScoped();
   lglScale(2.0/vbo->lglGetExtent().length());
   lglTranslate(-vbo->lglGetCenter());
   vbo->lglColor(1,1,1);
   vbo->lglSampler("tex2D", 0, false);
   vbo->lglSampler("tex3D", 1, false);
   vbo->lglRender();

   emit wrnstatus(lglGetWarning().c_str());

   // angle delta equals time delta times omega
   angle+=dt*omega;
}

void QGLGLSLWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Space)
      toggle_animation();
   else
      lgl_Qt_GLWidget::keyPressEvent(event);
}

void QGLGLSLWindow::mousePressEvent(QMouseEvent *event)
{
   if (event->buttons() & Qt::LeftButton)
      buttonDown = true;
   else
      event->ignore();

   lastPos = event->pos();
}

void QGLGLSLWindow::mouseReleaseEvent(QMouseEvent *event)
{
   if (!(event->buttons() & Qt::LeftButton))
      buttonDown = false;
   else
      event->ignore();
}

void QGLGLSLWindow::mouseMoveEvent(QMouseEvent *event)
{
   if (buttonDown)
   {
      double dx = (double)(event->x()-lastPos.x())/width();
      double dy = (double)(event->y()-lastPos.y())/height();

      lglManip(-dx, -dy);
      update();

      lastPos = event->pos();
   }
}
