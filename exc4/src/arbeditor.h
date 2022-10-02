// (c) by Stefan Roettger, licensed under MIT license

#ifndef ARBEDITOR_H
#define ARBEDITOR_H

#include "codeeditor.h"
#include "qglarbwindow.h"

#ifdef HAVE_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QRegExp>

#include <locale.h>

class ARBTextEdit: public CodeEditor
{
   Q_OBJECT

public:

   //! default ctor
   ARBTextEdit(QWidget *parent = NULL)
      : CodeEditor(parent)
   {}

   //! dtor
   virtual ~ARBTextEdit()
   {}

   QSize minimumSizeHint() const
   {
      return(QSize(512, 256));
   }

};

class ARBProgramEditorWindow: public QWidget
{
   Q_OBJECT

public:

   //! default ctor
   ARBProgramEditorWindow(QWidget *parent = NULL);

   //! dtor
   virtual ~ARBProgramEditorWindow();

   QSize minimumSizeHint() const
   {
      return(QSize(1024, 512));
   }

protected:

   QGLARBWindow *arbwindow_;

   ARBTextEdit *vtxshader_;
   ARBTextEdit *frgshader_;

   QLabel *vtxstatus_;
   QLabel *frgstatus_;

   QString browse(QString title, QString path="", bool newfile=false);
   QString readText(QString file);

protected slots:

   void gfxCube();
   void gfxTeapot();

   void reloadShaders();

   void vtxstatus(QString);
   void frgstatus(QString);

   void vtxLoad();
   void frgLoad();
};

#endif
