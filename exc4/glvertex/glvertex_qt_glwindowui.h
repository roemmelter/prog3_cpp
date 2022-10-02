// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL Qt window ui
#ifndef GLVERTEX_QT_GLWINDOWUI_H
#define GLVERTEX_QT_GLWINDOWUI_H

#include <locale.h>

#include <QApplication>

#include "glvertex_qt_glwindow.h"

//! Qt OpenGL window class /w event handling
class lgl_Qt_GLWindowUI: public lgl_Qt_GLWindow
{
public:

   lgl_Qt_GLWindowUI(bool core = true)
      : lgl_Qt_GLWindow(core),
        buttonDown_(false),
        spin_(0)
   {
      setlocale(LC_NUMERIC, "C");
   }

   virtual ~lgl_Qt_GLWindowUI()
   {}

protected:

   bool buttonDown_;
   QPoint lastPos_;
   vec2 lastDelta_;
   vec2 spin_;

   void keyPressEvent(QKeyEvent *event)
   {
#ifndef __APPLE__
      Qt::KeyboardModifier ctrl = Qt::ControlModifier;
#else
      Qt::KeyboardModifier ctrl = Qt::MetaModifier;
#endif

      if (event->key() == Qt::Key_Escape)
         qApp->quit();
      else if (event->key()==Qt::Key_Space)
         toggle_animation();
      else if (event->modifiers() & ctrl)
         if (event->key()==Qt::Key_R)
         {
            lglResetManip();
            update();
         }
         else if (event->key()==Qt::Key_Q)
            qApp->quit();
         else
            lgl_Qt_GLWindow::keyPressEvent(event);
      else
         lgl_Qt_GLWindow::keyPressEvent(event);

      interaction();
   }

   void keyReleaseEvent(QKeyEvent *event)
   {
      if (event->key() == Qt::Key_Back)
         qApp->quit();
      else
         lgl_Qt_GLWindow::keyReleaseEvent(event);

      interaction();
   }

   void mousePressEvent(QMouseEvent *event)
   {
      if (event->buttons() & Qt::LeftButton)
         buttonDown_ = true;
      else
         event->ignore();

      if (buttonDown_)
      {
         double x = (event->x()+0.5)/width();
         double y = (event->y()+0.5)/height();

         mouseClick(x, y);
      }

      spin_ = vec2(0);

      lastPos_ = event->pos();
      lastDelta_ = vec2(0);

      interaction();
   }

   void mouseReleaseEvent(QMouseEvent *event)
   {
      if (!(event->buttons() & Qt::LeftButton))
      {
         buttonDown_ = false;
         spin_ = lastDelta_;

         if (spin_.length() < 3.0/width()) spin_ = vec2(0);

         if (spin_.norm() > 0) update();
      }
      else
         event->ignore();

      interaction();
   }

   void mouseMoveEvent(QMouseEvent *event)
   {
      if (buttonDown_)
      {
         double dx = (double)(event->x()-lastPos_.x())/width();
         double dy = (double)(event->y()-lastPos_.y())/height();

         mouseMoved(dx, dy);

         lglManip(-dx, -dy);
         update();

         lastPos_ = event->pos();
         lastDelta_ = vec2(dx, dy);
      }

      interaction();
   }

   void mouseDoubleClickEvent(QMouseEvent *event)
   {
      if (event->buttons() & Qt::LeftButton)
      {
         double x = (event->x()+0.5)/width();
         double y = (event->y()+0.5)/height();

         doubleClick(x, y);
         update();
      }

      interaction();
   }

   void wheelEvent(QWheelEvent *event)
   {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      double numDegrees = event->delta()/16.0;

      if (event->orientation() == Qt::Vertical)
#else
      double numDegrees = event->angleDelta().y()/16.0;

      if (numDegrees != 0)
#endif
      {
         lglManip(0, 0, 1+numDegrees/360.0);
         update();
      }

      event->accept();

      interaction();
   }

   // apply manipulator spin
   virtual void updateOpenGL(double dt)
   {
      if (spin_.norm() > 0)
      {
         lglManip(-spin_);
         update();
      }
   }

   //! check for manipulator spin
   virtual bool isSpinning()
   {
      return(spin_.norm() > 0);
   }

   //! reimplement to be called in case of a mouse move
   virtual void mouseMoved(double dx, double dy)
   {}

   //! reimplement to be called in case of a mouse click
   virtual void mouseClick(double x, double y)
   {}

   //! reimplement to be called in case of a double click
   virtual void doubleClick(double x, double y)
   {
      toggle_wireframe();
   }

   //! reimplement to be called in case of a user interaction
   virtual void interaction()
   {}

};

#endif
