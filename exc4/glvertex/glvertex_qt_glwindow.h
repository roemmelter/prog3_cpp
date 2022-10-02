// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL Qt window
#ifndef GLVERTEX_QT_GLWINDOW_H
#define GLVERTEX_QT_GLWINDOW_H

#include "glvertex_qt.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))

#include "glvertex_qt_glwidget.h"

class lgl_Qt_GLWindow: public lgl_Qt_GLWidget
{
public:

   //! default ctor
   lgl_Qt_GLWindow(bool core = true)
      : lgl_Qt_GLWidget(NULL, core)
   {}

   //! dtor
   virtual ~lgl_Qt_GLWindow()
   {}

};

#else

#include <QTime>
#include <QElapsedTimer>
#include <QWindow>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QCoreApplication>

//! Qt OpenGL window base class
class lgl_Qt_GLWindow: public QWindow, protected QOpenGLFunctions
{
public:

   //! default ctor
   lgl_Qt_GLWindow(bool core = true)
      : QWindow(),
        core_(core),
        time_(0),
        speedup_(1),
        background_(false),
        red_(0), green_(0), blue_(0), alpha_(1),
        animation_(true),
        override_(false),
        ztest_(true),
        culling_(false),
        blending_(false),
        atest_(false),
        wireframe_(false),
        fullscreen_(false),
        timerid_(-1),
        m_context(NULL),
        m_update_pending(false)
   {
      setSurfaceType(QWindow::OpenGLSurface);

      QSurfaceFormat format;
      format.setDepthBufferSize(24);
      format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
      format.setSwapInterval(1);
#endif
      if (core_)
      {
#if (LGL_OPENGL_VERSION>=30) || defined(LGL_GLES)
#if (LGL_OPENGL_VERSION >= 30)
#if (LGL_OPENGL_VERSION >= 32) && !defined(__APPLE__)
         format.setVersion(3, 2);
#else
         format.setVersion(3, 0);
#endif
#else
         format.setVersion(2, 0);
#endif
#ifndef FORCE_COMPATIBILITY_PROFILE
         format.setProfile(QSurfaceFormat::CoreProfile);
#else
         format.setProfile(QSurfaceFormat::CompatibilityProfile);
#endif
#endif
      }
      setFormat(format);

#ifdef QT_DEBUG

#if (LGL_OPENGL_VERSION >= 30)
      std::cerr << "Creating QWindow with"
                << std::endl
                << " opengl "
                << (format.profile()==QSurfaceFormat::CoreProfile? "core" : "compatibility")
                << " profile"
                << std::endl
                << " version "
                << format.majorVersion() << "." << format.minorVersion()
                << std::endl
                << std::endl;
#endif

#endif

      setFPS(30);
   }

   //! dtor
   virtual ~lgl_Qt_GLWindow()
   {
      finish();
   }

   //! update
   void update()
   {
      renderLater();
   }

   //! set frame rate
   void setFPS(double rate)
   {
      fps_ = rate;

      if (fps_ > 0)
         timerid_ = startTimer((int)(1000/fps_));
      else
      {
         if (timerid_ >= 0) killTimer(timerid_);
         timerid_ = -1;
      }

      timer_.start();
      update();
   }

   //! set time lapse speedup
   void setTimeLapse(double speedup)
   {
      speedup_ = speedup;
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

   //! toggle background color
   void toggle_background()
   {
      if (red_+green_+blue_ > 1.5)
         background(0,0,0);
      else
         background(1,1,1);
   }

   //! enable animation
   void animation(bool on)
   {
      animation_ = on;
      if (animation_) timer_.start();
      update();
   }

   //! toggle animation
   void toggle_animation()
   {
      animation(!animation_);
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

   //! enable back-face culling
   void culling(bool on)
   {
      if (on != culling_)
      {
         override_ = true;
         culling_ = on;
         update();
      }
   }

   //! toggle back-face culling
   void toggle_culling()
   {
      culling(!culling_);
   }

   //! enable alpha blending
   void blending(bool on)
   {
      if (on != blending_)
      {
         override_ = true;
         blending_ = on;
         update();
      }
   }

   //! toggle alpha blending
   void toggle_blending()
   {
      blending(!blending_);
   }

   //! enable alpha testing
   void atest(bool on)
   {
      if (on != atest_)
      {
         override_ = true;
         atest_ = on;
         update();
      }
   }

   //! toggle alpha testing
   void toggle_atest()
   {
      atest(!atest_);
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

   //! enable fullscreen mode
   void fullscreen(bool on)
   {
      if (on != fullscreen_)
      {
         fullscreen_ = on;
         if (fullscreen_) showFullScreen();
         else showNormal();
         requestActivate();
         update();
      }
   }

   //! toggle fullscreen mode
   void toggle_fullscreen()
   {
      fullscreen(!fullscreen_);
   }

   //! restart time
   void restart()
   {
      time_ = 0;
   }

public slots:

   void renderNow()
   {
      if (!isExposed())
         return;

      bool needsInitialize = false;

      if (!m_context)
      {
         m_context = new QOpenGLContext(this);
         m_context->setFormat(requestedFormat());
         m_context->create();

         needsInitialize = true;
      }

      m_context->makeCurrent(this);

      if (needsInitialize)
      {
         initialize();
         initializeOpenGLFunctions();
         initializeOpenGL();
      }

      if (override_)
      {
         lglDepthTest(ztest_);
         lglBackFaceCulling(culling_);
         lglBlendMode(blending_?LGL_BLEND_ALPHA:LGL_BLEND_NONE);
         lglAlphaTest(atest_);
         lglPolygonMode(wireframe_?LGL_LINE:LGL_FILL);
      }

      float scale = devicePixelRatio();
      lglViewport(0, 0, scale*width(), scale*height());

      if (background_)
      {
         lglClearColor(red_, green_, blue_, alpha_);
         lglClear();
      }

      double dt = 0.001*timer_.elapsed();
      if (!animation_) dt = 0;
      else dt *= speedup_;

      timer_.start();

      prepareOpenGL(dt);
      renderOpenGL(dt);
      updateOpenGL(dt);
      finishOpenGL(time_+=dt);

      m_context->swapBuffers(this);

      if (fps_ <= 0)
         if (animation_)
            update();
   }

   void renderLater()
   {
      if (!m_update_pending)
      {
         m_update_pending = true;
         QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
      }
   }

protected:

   bool core_; // OpenGL core profile?

   double time_; // elapsed time

   double fps_; // animated frames per second
   double speedup_; // time lapse speedup

   bool background_;
   double red_, green_, blue_, alpha_;

   bool animation_;
   bool override_;
   bool ztest_;
   bool culling_;
   bool blending_;
   bool atest_;
   bool wireframe_;
   bool fullscreen_;

   QElapsedTimer timer_;
   int timerid_;

   bool event(QEvent *event)
   {
      switch (event->type())
      {
         case QEvent::UpdateRequest:
            m_update_pending = false;
            renderNow();
            return(true);
         default:
            return(QWindow::event(event));
      }
   }

   void exposeEvent(QExposeEvent *)
   {
      if (isExposed())
         renderNow();
   }

   void keyPressEvent(QKeyEvent *event)
   {
#ifndef __APPLE__
      Qt::KeyboardModifier ctrl = Qt::ControlModifier;
#else
      Qt::KeyboardModifier ctrl = Qt::MetaModifier;
#endif

      if (event->modifiers() & ctrl)
         if (event->key() == Qt::Key_Space)
         {
            animation(!animation_);
         }
         else if (event->key() == Qt::Key_Z)
         {
            ztest(!ztest_);
         }
         else if (event->key() == Qt::Key_C)
         {
            culling(!culling_);
         }
         else if (event->key() == Qt::Key_B)
         {
            blending(!blending_);
         }
         else if (event->key() == Qt::Key_A)
         {
            atest(!atest_);
         }
         else if (event->key() == Qt::Key_W)
         {
            wireframe(!wireframe_);
         }
         else if (event->key() == Qt::Key_0)
         {
            background(0,0,0);
         }
         else if (event->key() == Qt::Key_1)
         {
            background(1,1,1);
         }
         else if (event->key() == Qt::Key_O)
         {
            override(!override_);
         }
         else if (event->key() == Qt::Key_F)
         {
            fullscreen(!fullscreen_);
         }
         else
            event->ignore();
      else
         event->ignore();
   }

   void timerEvent(QTimerEvent *)
   {
      if (animation_)
         update();
   }

   virtual void initialize() {lglInitializeOpenGL();}
   virtual void initializeOpenGL() = 0; //< reimplement for initializing OpenGL state
   virtual void prepareOpenGL(double dt) {}
   virtual void renderOpenGL(double dt) = 0; //< reimplement for rendering each frame
   virtual void updateOpenGL(double dt) {}
   virtual void finishOpenGL(double t) {}
   virtual void finish() {}

private:

   QOpenGLContext *m_context;
   bool m_update_pending;
};

#endif

#endif
