// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLSLEDITOR_H
#define GLSLEDITOR_H

#include "codeeditor.h"
#include "qglglslwindow.h"

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <locale.h>

class GLSLTextEdit: public CodeEditor
{
   Q_OBJECT

public:

   //! default ctor
   GLSLTextEdit(QWidget *parent = NULL)
      : CodeEditor(parent)
   {}

   //! dtor
   virtual ~GLSLTextEdit()
   {}

   QSize minimumSizeHint() const
   {
      return(QSize(512, 256));
   }

};

class GLSLProgramEditorWindow: public QWidget
{
   Q_OBJECT

public:

   //! default ctor
   GLSLProgramEditorWindow(QWidget *parent = NULL);

   //! dtor
   virtual ~GLSLProgramEditorWindow();

   QSize minimumSizeHint() const
   {
      return(QSize(1024, 512));
   }

protected:

   QGLGLSLWindow *glslwindow_;

   GLSLTextEdit *vtxshader_;
   GLSLTextEdit *frgshader_;

   QLabel *vtxstatus_;
   QLabel *frgstatus_;
   QLabel *wrnstatus_;
   QLabel *lnkstatus_;

   QString browse(QString title, QString path="", bool newfile=false);
   QString readText(QString file);

protected slots:

   void loadObj();
   void reloadShaders();

   void vtxstatus(QString);
   void frgstatus(QString);
   void lnkstatus(QString);
   void wrnstatus(QString);

   void vtxLoad();
   void frgLoad();

   void background();
   void ztest();
   void wireframe();

   void help();
};

#endif
