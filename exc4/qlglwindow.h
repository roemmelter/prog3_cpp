// (c) by Stefan Roettger, licensed under MIT license

#ifndef QLGLWINDOW_H
#define QLGLWINDOW_H

#include "v3d.h"

#include "dicom/dicombase.h"
#include "pvm/ddsbase.h"

#include "glvertex_qt_glui.h"

class QLGLWindow: public lgl_Qt_GLUI
{
public:

   //! default ctor
   QLGLWindow(QWidget *parent = NULL)
      : lgl_Qt_GLUI(parent)
   {}

   //! dtor
   virtual ~QLGLWindow()
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
