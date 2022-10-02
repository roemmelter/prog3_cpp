// (c) by Stefan Roettger, licensed under MIT license

#ifndef QGLGLSLWINDOW_H
#define QGLGLSLWINDOW_H

#include <string>

#include "glvertex_qt_glwidget.h"

class QGLGLSLWindow: public lgl_Qt_GLWidget
{
   Q_OBJECT

public:

   //! default ctor
   QGLGLSLWindow(QWidget *parent = NULL);

   //! dtor
   virtual ~QGLGLSLWindow();

   QSize minimumSizeHint() const
   {
      return(QSize(512, 512));
   }

   void loadObj(const char *path);
   void resetObj();

   void setGLSLVertexShader(std::string shader);
   void setGLSLFragmentShader(std::string shader);

protected:

   lglVBO *vbo;

   std::string vtx_shader, frg_shader;
   bool vtx_modified, frg_modified;
   int shader_id;

   bool buttonDown;
   QPoint lastPos;

   void bindShaders();

   void initializeOpenGL();
   void renderOpenGL(double dt);

   void keyPressEvent(QKeyEvent *event);
   void mousePressEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);

signals:

   void vtxstatus(QString);
   void frgstatus(QString);
   void lnkstatus(QString);
   void wrnstatus(QString);
};

#endif
