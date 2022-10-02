#include "mainwindow.h"

// default ctor
MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   QAction *quitAction = new QAction(tr("Q&uit"), this);
   connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

   QMenu *fileMenu_ = menuBar()->addMenu(tr("&File"));
   fileMenu_->addAction(quitAction);

   module = MODULE_PAINTER;

   animation = true;
   override = false;
   ztest = true;
   wireframe = false;

#ifdef OPENGL
   module = MODULE_GL;
#endif

#ifdef LGL
   module = MODULE_LGL;
#endif

   parseOptions();

   if (module == MODULE_PAINTER)
   {
      widget_ = new PainterWidget();
      setWindowTitle("PainterWidget");
   }
   else if (module == MODULE_GL)
   {
      widget_ = new QGLWindow();
      setWindowTitle("OpenGL");
   }
   else if (module == MODULE_LGL)
   {
      widget_ = new QLGLWindow();
      setWindowTitle("OpenGL core profile (via glVertex library)");
   }
   else if (module == MODULE_ARBEDITOR)
   {
      widget_ = new ARBProgramEditorWindow();
      setWindowTitle("Shader Program Editor");
   }
   else if (module == MODULE_GLSLEDITOR)
   {
      widget_ = new GLSLProgramEditorWindow();
      setWindowTitle("GLSL Shader Editor");
   }

   setCentralWidget(widget_);
   widget_->setFocus();

   QOGLWidget *gl_widget = dynamic_cast<QOGLWidget *>(centralWidget());

   if (gl_widget)
   {
      gl_widget->animation(animation);

      if (override)
      {
         gl_widget->ztest(ztest);
         gl_widget->wireframe(wireframe);
      }
   }

   lgl_Qt_GLWidget *lgl_widget = dynamic_cast<lgl_Qt_GLWidget *>(centralWidget());

   if (lgl_widget)
   {
      lgl_widget->animation(animation);

      if (override)
      {
         lgl_widget->ztest(ztest);
         lgl_widget->wireframe(wireframe);
      }
   }
}

void MainWindow::parseOptions()
{
   // get argument list
   QStringList args = qApp->arguments();

   // scan for arguments and options
   QStringList arg,opt;
   for (int i=1; i<args.size(); i++)
      if (args[i].startsWith("--")) opt.push_back(args[i].mid(2));
      else if (args[i].startsWith("-")) opt.push_back(args[i].mid(1));
      else arg.push_back(args[i]);

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i].startsWith("paint")) {module = MODULE_PAINTER;}
      else if (opt[i]=="opengl" || opt[i]=="gl") {module = MODULE_GL;}
      else if (opt[i]=="glvertex" || opt[i]=="lgl") {module = MODULE_LGL;}
      else if (opt[i].startsWith("arbed")) {module = MODULE_ARBEDITOR;}
      else if (opt[i].startsWith("glsled")) {module = MODULE_GLSLEDITOR;}
      else if (opt[i].startsWith("module="))
      {
         QString name = get_str(opt[i]);
         if (name.startsWith("paint")) module = MODULE_PAINTER;
         else if (name=="opengl" || name=="gl") module = MODULE_GL;
         else if (name=="glvertex" || name=="lgl") module = MODULE_LGL;
         else if (name.startsWith("arbed")) module = MODULE_ARBEDITOR;
         else if (name.startsWith("glsled")) module = MODULE_GLSLEDITOR;
      }
      else if (opt[i].startsWith("animation=")) animation=(int)(get_opt(opt[i])+0.5);
      else if (opt[i].startsWith("ztest=")) {ztest=(int)(get_opt(opt[i])+0.5); override=true;}
      else if (opt[i].startsWith("wireframe=")) {wireframe=(int)(get_opt(opt[i])+0.5); override=true;}
      else if (opt[i]=="help") usage(args[0]);
}

void MainWindow::usage(QString prog)
{
   QString app(prog.mid(prog.lastIndexOf("/")+1));

   QString txt;
   txt += "usage:\n";
   txt += " ./" + app + " {options}\n";
   txt += "where options are:\n";
   txt += " --paint: show qpainter window\n";
   txt += " --gl: show opengl window\n";
   txt += " --lgl: show lgl window\n";
   txt += " --arbed: show ARB program editor window\n";
   txt += " --glsled: show GLSL shader editor window\n";
   txt += " --help: this help text\n";

   std::cout << txt.toStdString();

   exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Q)
      emit close();

   QMainWindow::keyPressEvent(event);
}
