// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL Qt convenience UI class
#ifndef QT_GLUI_H
#define QT_GLUI_H

#include <ctype.h>

#include "glvertex_qt_glwidgetui.h"
#include "glvertex_qt_shadereditor.h"

#include "glvertex_scene.h"
#include "glvertex_pnmformat.h"

//! Qt OpenGL widget convenience class
class lgl_Qt_GLUI: public lgl_Qt_GLWidgetUI
{
public:

   //! ctor
   lgl_Qt_GLUI(QWidget *parent = NULL, bool core = true)
      : lgl_Qt_GLWidgetUI(parent, core)
   {
      capture_ = "";
      export_ = "";
      print_ = false;
      startup_ = 10;

      // get argument list
      QStringList args = qApp->arguments();

      // scan for arguments and options
      for (int i=1; i<args.size(); i++)
         if (args[i].startsWith("--")) opt_.push_back(args[i].mid(2));
         else if (args[i].startsWith("-")) opt_.push_back(args[i].mid(1));
         else arg_.push_back(args[i]);
   }

   virtual ~lgl_Qt_GLUI()
   {}

   //! get number of arguments
   int args() const
   {
      return(arg_.size());
   }

   //! get argument
   QString getArg(int i) const
   {
      return(arg_[i]);
   }

   //! get number of options
   int opts() const
   {
      return(opt_.size());
   }

   //! get option
   QString getOption(int i) const
   {
      return(opt_[i]);
   }

   //! check for option
   bool hasOption(QString opt) const
   {
      for (int i=0; i<opts(); i++)
      {
         if (opt_[i] == opt)
            return(true);

         if (opt_[i].startsWith(opt+"="))
            return(true);
      }

      return(false);
   }

   //! get option string
   QString getOptionString(QString opt) const
   {
      for (int i=0; i<opts(); i++)
         if (opt_[i].startsWith(opt))
            return(opt_[i].mid(opt_[i].indexOf("=")+1));

      return("");
   }

   //! get option value
   double getOptionValue(QString opt) const
   {
      return(getOptionString(opt).toDouble());
   }

protected:

   QStringList arg_, opt_;

   void initialize()
   {
      lgl_Qt_GLWidgetUI::initialize();

      printKeyInfo();
      qDebug() << "";

      QString compatibility = QString("OpenGL compatibility: ") + QString::number(0.1*lglGetGLVersion());
      QString renderer = QString("OpenGL renderer: ") + lglGetRenderer().c_str();
      QString glsl = QString("GLSL version: ") + lglGetGLSLVersionString().c_str();

      qDebug("%s", compatibility.toStdString().c_str());
      qDebug("%s", renderer.toStdString().c_str());
      qDebug("%s", glsl.toStdString().c_str());

      // check for help option
      if (hasOption("help"))
      {
         std::cout << std::endl << getUsageInfo();
         exit(0);
      }

      // check for fullscreen option
      if (hasOption("fullscreen"))
         toggle_fullscreen();

      // check for wireframe option
      if (hasOption("wireframe"))
         toggle_wireframe();

      // check for pause option
      if (hasOption("pause"))
         toggle_animation();
   }

   void keyPressEvent(QKeyEvent *event)
   {
      char k = '\0';

      QString key = event->text();

      if (key != "")
         k = key[0].toLatin1();

      Qt::KeyboardModifier shift = Qt::ShiftModifier;

#ifndef __APPLE__
      Qt::KeyboardModifier ctrl = Qt::ControlModifier;
#else
      Qt::KeyboardModifier ctrl = Qt::MetaModifier;
#endif

      if (k != '\0')
         if (k!=0x11 && k!=0x1b && k!=' ')
         {
            if (event->modifiers() & ctrl)
            {
               if (event->key() == Qt::Key_S)
                  capture("screen");
               else if (event->key() == Qt::Key_T)
                  toggleCapture("screen");
               else if (event->key() == Qt::Key_E)
                  triggerExport("scene");
               else if (event->key() == Qt::Key_P)
                  toggleFPS();
               else if (event->key() == Qt::Key_K)
               {
                  qDebug();
                  printKeyInfo();
               }
            }
            else
            {
               if (event->modifiers() & shift)
                  k = toupper(k);

               keyPressed(k);
               update();
            }
         }

      lgl_Qt_GLWidgetUI::keyPressEvent(event);
   }

   void keyReleaseEvent(QKeyEvent *event)
   {
      char k = '\0';

      QString key = event->text();

      if (key != "")
         k = key[0].toLatin1();

      if (k != '\0')
         if (k!=0x11 && k!=0x1b && k!=' ')
            keyReleased(k);

      lgl_Qt_GLWidgetUI::keyReleaseEvent(event);
   }

   virtual void keyPressed(char key)
   {
      QString msg;

      if (isprint(key))
         msg = QString("key '")+key+"' pressed";
      else
         msg = QString("key 0x")+QString("%1").arg(key, 2, 16, QChar('0'))+" pressed";

      qDebug("%s", msg.toStdString().c_str());
   }

   virtual void keyReleased(char key)
   {}

public:

   //! capture a screen shot
   bool capture(std::string filename)
   {
      unsigned char *pixels = lglReadRGBPixels(0,0, width(), height());
      if (!pixels) return(false);

      bool ok = lglWritePnmImage(filename+".ppm", pixels, width(), height(), 3);
      free(pixels);
      return(ok);
   }

   //! toggle capturing a screen shot series
   void toggleCapture(std::string filename)
   {
      if (capture_ == "")
         capture_ = filename;
      else
         capture_ = "";
   }

   //! trigger OBJ scene export on next frame
   void triggerExport(std::string filename, mat4 m = mat4(1))
   {
      export_ = filename;
      export_matrix_ = m;
   }

   //! toggle fps printing
   void toggleFPS()
   {
      print_ = !print_;
   }

protected:

   void prepareOpenGL(double dt)
   {
      lgl_Qt_GLWidgetUI::prepareOpenGL(dt);

      if (export_ != "")
         beginExport();
   }

   void updateOpenGL(double dt)
   {
      lgl_Qt_GLWidgetUI::updateOpenGL(dt);

      if (export_ != "")
         endExport();

      if (print_)
         printFPS(dt);
   }

   void finishOpenGL(double t)
   {
      lgl_Qt_GLWidgetUI::finishOpenGL(t);

      if (capture_ != "")
      {
         QDateTime t = QDateTime::currentDateTime();
         QString time = t.toString("yyyyMMdd_HHmmsszzz");
         capture(capture_+"_"+time.toStdString());
      }
   }

   std::string capture_;

   std::string export_;
   FILE *export_file_;
   int export_index_;
   int export_nindex_;
   int export_tindex_;
   mat4 export_matrix_;

   bool print_;
   int startup_;

   //! reimplement to show usage info
   virtual std::string getUsageInfo()
   {
      const char usage[] =
         "Usage:\n"
         " --help -> this message\n"
         " --fullscreen -> toggle fullscreen mode\n"
         " --wireframe -> toggle wireframe mode\n"
         " --pause -> pause animations\n";

      return(usage);
   }

   //! reimplement to show key info
   virtual std::string getKeyInfo()
   {
      const char keys[] =
         "ESC: quit\n"
         "Ctrl-q: quit\n"
         "Ctrl-z: toggle Z-test\n"
         "Ctrl-c: toggle back-face culling\n"
         "Ctrl-b: toggle alpha blending\n"
         "Ctrl-a: toggle A-test\n"
         "Ctrl-w: toggle wire frame mode\n"
         "Ctrl-0: black background\n"
         "Ctrl-1: white background\n"
         "Ctrl-o: toggle override\n"
         "Ctrl-f: toggle full-screen\n"
         "Ctrl-r: reset manipulator\n"
         "Space: toggle animation\n"
         "\n"
         "Ctrl-s: capture screen\n"
         "Ctrl-t: toggle continuous screen capture\n"
         "Ctrl-e: trigger OBJ scene export\n"
         "Ctrl-p: toggle frame rate info\n"
         "Ctrl-k: print key info\n";

      return(keys);
   }

   void printKeyInfo()
   {
      QString info = QString("Available key commands:\n\n") + getKeyInfo().c_str();
      if (info.endsWith("\n")) info.chop(1);

      qDebug("%s", info.toStdString().c_str());
   }

   void beginExport()
   {
      std::string filename = export_+".obj";
      export_file_ = fopen(filename.c_str(), "wb");

      if (export_file_)
      {
         fprintf(export_file_, "# glVertex OBJ File\n\n");

         export_index_ = 1;
         export_nindex_ = 1;
         export_tindex_ = 1;

         lglBeginExport(callback, this, export_matrix_);
      }
   }

   static void callback(LGL_VBO_TYPE *vbo, void *data)
   {
      lgl_Qt_GLUI *glui = (lgl_Qt_GLUI *)data;
      FILE *file = glui->export_file_;
      int *index = &glui->export_index_;
      int *nindex = &glui->export_nindex_;
      int *tindex = &glui->export_tindex_;

      fprintf(file, "#");
      if (vbo->getName() != "") fprintf(file, " name=%s", vbo->getName().c_str());
      fprintf(file, " vertices=%d", vbo->lglGetVertexCount());
      fprintf(file, " primitives=%d\n", vbo->lglGetPrimitiveCount());
      lglSaveObjInto((lglVBO *)vbo, file, *index, *nindex, *tindex);
   }

   void endExport()
   {
      if (export_file_)
      {
         lglEndExport();
         fclose(export_file_);
      }

      export_ = "";
   }

   void printFPS(double dt)
   {
      if (startup_ > 0)
      {
         startup_--;
         return;
      }

      dt /= speedup_;

      static const double interval = 3;

      static double t = 0;
      static int c = 0;
      static double f = 0;

      if (dt > 0)
      {
         t += dt;
         f += 1/dt;
         c++;
      }

      if (t > interval)
      {
         if (c > 0)
         {
            double fps = f/c;
            fps = floor(10*fps+0.5)/10;
            qDebug("fps = %g", fps);
         }

         t = 0;
         c = 0;
         f = 0;
      }
   }

};

//! Qt convenience class for multi-pass rendering
class lgl_Scene_GLUI: public lgl_Qt_GLUI, public lgl_Renderer
{
public:

   lgl_Scene_GLUI(QWidget *parent = NULL, bool core = true)
      : lgl_Qt_GLUI(parent, core),
        lgl_Renderer()
   {}

   virtual ~lgl_Scene_GLUI()
   {}

   virtual void initializeOpenGL()
   {
      lgl_Renderer::initializeOpenGL();
   }

   virtual void renderOpenGL(double dt)
   {
      lgl_Renderer::renderOpenGL(dt);
   }

   virtual void updateOpenGL(double dt)
   {
      lgl_Qt_GLUI::updateOpenGL(dt);

      if (lgl_Cam *cam = getScene()->getCam())
         cam->halt(isSpinning());
   }

   virtual void keyPressed(char key)
   {
      lgl_Renderer::keyPressed(key);
   }

   virtual void keyReleased(char key)
   {
      lgl_Renderer::keyReleased(key);
   }

   virtual std::string getUsageInfo()
   {
      bool enabled = false;

      if (lgl_Scene *scene=getScene())
         if (scene->supportsStereo())
            enabled = true;

      if (enabled)
      {
         const char usage[] =
            " --stereo -> enable interlaced stereo rendering\n"
            " --stereo=left -> enable horizontal left stereo interlacing mode\n"
            " --stereo=right -> enable horizontal right stereo interlacing mode\n"
            " --stereo=top -> enable vertical top stereo interlacing mode\n"
            " --stereo=bottom -> enable vertical top stereo interlacing mode\n"
            " --stereobase=... -> specify distance of eye points in stereo mode\n";

         return(lgl_Qt_GLUI::getUsageInfo() + usage);
      }

      return(lgl_Qt_GLUI::getUsageInfo());
   }

   virtual std::string getKeyInfo()
   {
      std::string info1 = lgl_Qt_GLUI::getKeyInfo();
      std::string info2 = lgl_Renderer::getKeyInfo();

      std::string info = info1;
      if (info2 != "") info += "\n" + info2;

      return(info);
   }

   virtual void mouseClick(double x, double y)
   {
      lgl_Renderer::mouseClick(x, y);
      printPickInfo(x, y);
   }

   virtual void doubleClick(double x, double y)
   {
      lgl_Renderer::doubleClick(x, y);
   }

   void printDoubleClickInfo()
   {
      QString info = QString("Available interactions:\n\n") + getDoubleClickInfo().c_str();
      if (info.endsWith("\n")) info.chop(1);

      qDebug("%s", info.toStdString().c_str());
   }

   void printPickInfo(double x=0.5, double y=0.5)
   {
      vec3 o = vec3(0,0,0);
      vec3 d = vec3(0,0,-1);

      if (lgl_Cam *cam = getScene()->getCam())
      {
         d = cam->unprojectViewport(x, y);

         if (lglVBO *vbo = pickScene(o, d))
         {
            std::string name = vbo->getName();

            if (name != "")
               glslmath::print("picked object \"" + name + "\"");
            else
               glslmath::print("picked unnamed object");
         }
      }
   }

   virtual void interaction()
   {
      lgl_Renderer::interaction();
   }

   virtual void config()
   {
      double sbase = 0;
      lgl_interlacing_enum smode = LGL_INTERLACE_VERTICAL_TOP;

      if (hasOption("stereo"))
      {
         sbase = 0.1;

         QString mode = getOptionString("stereo");

         if (mode == "left") smode = LGL_INTERLACE_HORIZONTAL_LEFT;
         else if (mode == "right") smode = LGL_INTERLACE_HORIZONTAL_RIGHT;
         else if (mode == "top") smode = LGL_INTERLACE_VERTICAL_TOP;
         else if (mode == "bottom") smode = LGL_INTERLACE_VERTICAL_BOTTOM;
      }

      if (hasOption("stereobase"))
         sbase = getOptionValue("stereobase");

      if (sbase != 0)
         if (scene_)
            if (lgl_Cam *cam = scene_->getCam())
            {
               setStereoMode(smode);
               cam->setStereoBase(sbase);
               fullscreen(true);
            }
   }

   virtual int getViewportWidth() {return(width());}
   virtual int getViewportHeight() {return(height());}
   virtual void update() {lgl_Qt_GLUI::update();}

protected:

   void initialize()
   {
      printDoubleClickInfo();
      qDebug() << "";

      lgl_Qt_GLUI::initialize();
   }

};

#endif
