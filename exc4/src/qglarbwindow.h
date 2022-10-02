// (c) by Stefan Roettger, licensed under MIT license

#ifndef QGLARBWINDOW_H
#define QGLARBWINDOW_H

#include <string>

#include "qoglwidget.h"

class QGLARBWindow: public QOGLWidget
{
   Q_OBJECT

public:

   //! default ctor
   QGLARBWindow(QWidget *parent = NULL);

   //! dtor
   virtual ~QGLARBWindow();

   QSize minimumSizeHint() const
   {
      return(QSize(512, 512));
   }

   void enableTeapot(bool on);

   void setARBVertexProgram(std::string shader);
   void setARBFragmentProgram(std::string shader);

protected:

   bool teapot_;

   std::string vtx_shader, frg_shader;
   bool vtx_modified, frg_modified;
   int vtx_id, frg_id;

   void bindPrograms();

   void initializeOpenGL();
   void renderOpenGL(double dt);

signals:

   void vtxstatus(QString);
   void frgstatus(QString);
};

#endif
