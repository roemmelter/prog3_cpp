// (c) by Stefan Roettger, licensed under MIT license

#ifndef QGLWINDOW_H
#define QGLWINDOW_H

#include "v3d.h"

#include "gfx/gl.h"
#include "gfx/shaderprogram.h"

#include "dicom/dicombase.h"
#include "pvm/ddsbase.h"

#include "qoglwidget.h"

class QGLWindow: public QOGLWidget
{
public:

   //! default ctor
   QGLWindow(QWidget *parent = NULL)
      : QOGLWidget(parent)
   {}

   //! dtor
   virtual ~QGLWindow()
   {}

   //! return preferred minimum window size
   QSize minimumSizeHint() const
   {
      return(QSize(512, 512));
   }

protected:

   void initializeOpenGL();
   void renderOpenGL(double dt);
};

#endif
