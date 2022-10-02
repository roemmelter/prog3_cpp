// (c) by Stefan Roettger, licensed under MIT license

#ifndef QOGLWIDGET_H
#define QOGLWIDGET_H

#include "glvertex_gl.h"

#include <qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#   define USE_QOPENGLWIDGET
#endif

#ifndef USE_QOPENGLWIDGET
#   include <QGLWidget>
#   define QGLWIDGET QGLWidget
#else
#   include <QOpenGLWidget>
#   define QGLWIDGET QOpenGLWidget
#endif

#include <QTime>
#include <QElapsedTimer>
#include <QKeyEvent>

class QOGLWidget: public QGLWIDGET
{
public:

   //! default ctor
   QOGLWidget(QWidget *parent = NULL)
      : QGLWIDGET(parent),
        background_(false),
        red_(0), green_(0), blue_(0), alpha_(1),
        animation_(true),
        override_(false),
        ztest_(true),
        wireframe_(false),
        dt_(0)
   {
      setFocusPolicy(Qt::ClickFocus);

      setFPS(30);
   }

   //! dtor
   virtual ~QOGLWidget()
   {}

   //! set frame rate
   void setFPS(double rate)
   {
      fps_ = rate;
      startTimer((int)(1000/fps_));
   }

   //! set background color
   void background(double red, double green, double blue, double alpha=1)
   {
      background_ = true;
      red_ = red;
      green_ = green;
      blue_ = blue;
      alpha_ = alpha;
      update();
   }

   //! enable animation
   void animation(bool on)
   {
      animation_ = on;
      if (!animation_) dt_ = 0;
      update();
   }

   //! enable z-buffer test
   void ztest(bool on)
   {
      if (on != ztest_)
      {
         override_ = true;
         ztest_ = on;
         update();
      }
   }

   //! toggle z-buffer test
   void toggle_ztest()
   {
      ztest(!ztest_);
   }

   //! enable wireframe mode
   void wireframe(bool on)
   {
      if (on != wireframe_)
      {
         override_ = true;
         wireframe_ = on;
         update();
      }
   }

   //! toggle wireframe mode
   void toggle_wireframe()
   {
      wireframe(!wireframe_);
   }

   //! enable override
   void override(bool on)
   {
      override_ = on;
      if (!override_) background_ = false;
      update();
   }

protected:

   double fps_; // animated frames per second

   bool background_;
   double red_, green_, blue_, alpha_;

   bool animation_;
   bool override_;
   bool ztest_;
   bool wireframe_;

   QElapsedTimer time_;
   double dt_;

   void initializeGL()
   {
#ifndef USE_QOPENGLWIDGET
      qglClearColor(Qt::black);
#else
      glClearColor(0,0,0,1);
#endif

      initializeOpenGL();
   }

   void resizeGL(int, int)
   {
      glViewport(0, 0, width(), height());
   }

   void paintGL()
   {
      if (override_)
      {
         if (ztest_)
            glEnable(GL_DEPTH_TEST);
         else
            glDisable(GL_DEPTH_TEST);

         if (wireframe_)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

         glDisable(GL_CULL_FACE);
      }

      if (background_)
      {
         glClearColor(red_, green_, blue_, alpha_);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }

      if (!animation_) dt_ = 0;
      else if (dt_ == 0) dt_ = 1/fps_;
      else dt_ = 0.001*time_.elapsed();

      time_.start();

      renderOpenGL(dt_);
   }

   void keyPressEvent(QKeyEvent *event)
   {
      if (event->key() == Qt::Key_Space)
      {
         animation(!animation_);
      }
      else if (event->key() == Qt::Key_Z)
      {
         ztest(!ztest_);
      }
      else if (event->key() == Qt::Key_W)
      {
         wireframe(!wireframe_);
      }
      else if (event->key() == Qt::Key_B)
      {
         background(0,0,0);
      }
      else if (event->key() == Qt::Key_N)
      {
         background(1,1,1);
      }
      else if (event->key() == Qt::Key_O)
      {
         override(!override_);
      }
      else
         event->ignore();
   }

   void timerEvent(QTimerEvent *)
   {
      if (animation_)
         update();
   }

   virtual void initializeOpenGL() = 0;
   virtual void renderOpenGL(double dt) = 0;
};

#endif
