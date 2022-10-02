#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glsleditor.h"
#include "arbeditor.h"

#include "painterwidget.h"

#include "qglwindow.h"
#include "qlglwindow.h"

#ifdef HAVE_QT5
#include <QtWidgets/QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

#include <QtGui/QKeyEvent>

class MainWindow: public QMainWindow
{
   Q_OBJECT // Qt Metacall object for signal/slot connections

public:

   enum ModuleType {
      MODULE_PAINTER = 0,
      MODULE_GL = 1,
      MODULE_LGL = 2,
      MODULE_ARBEDITOR = 3,
      MODULE_GLSLEDITOR = 4
   };

   //! default ctor
   MainWindow(QWidget *parent = 0);

   //! dtor
   virtual ~MainWindow()
   {}

protected:

   QWidget *widget_;

   ModuleType module;

   bool animation;
   bool override;
   bool ztest;
   bool wireframe;

   void keyPressEvent(QKeyEvent *event);

private:

   void parseOptions();
   void usage(QString prog);

   QString get_str(QString o)
   {
      return(o.mid(o.indexOf("=")+1));
   }

   double get_opt(QString o)
   {
      return(get_str(o).toDouble());
   }

};

#endif
