// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_QT_SHADEREDITOR_H
#define GLVERTEX_QT_SHADEREDITOR_H

#include <QApplication>

#include "glvertex_qt_glwidget.h"
#include "glvertex_qt_textedit.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegExp>

#define FONT_ZOOM 1.25f

#define create_lgl_Qt_ShaderEditor(name, id)\
{\
   lgl_Qt_ShaderEditor *editor = new lgl_Qt_ShaderEditor(name, __FILE__, id, this, Qt::Window);\
   editor->resize(640, 512);\
   editor->show();\
}

class lgl_Qt_ShaderEditor: public QWidget
{
   Q_OBJECT

public:

   //! default ctor
   lgl_Qt_ShaderEditor(QString name, QString filename, GLuint *id = NULL,
                       QWidget *parent = NULL, Qt::WindowFlags f = Qt::Widget)
      : QWidget(parent, f),
        id_(id)
   {
      setWindowTitle(name);

      edit_ = new lgl_Qt_TextEdit(name , "glsl", "txt");

      edit_->setFontZoom(FONT_ZOOM);

      QVBoxLayout *layout = new QVBoxLayout();
      QHBoxLayout *buttonLayout = new QHBoxLayout();

      errorLabel_ = new QLabel();
      errorLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      errorLabel_->setWordWrap(true);

      QPushButton *helpButton = new QPushButton("Help");
      connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));

      QPushButton *loadButton = new QPushButton("Load");
      connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));

      QPushButton *saveButton = new QPushButton("Commit");
      connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));

      QPushButton *revertButton = new QPushButton("Revert");
      connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));

      QPushButton *quitButton = new QPushButton("Save && Quit");
      connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));

      buttonLayout->addWidget(helpButton);
      buttonLayout->addWidget(loadButton);
      buttonLayout->addWidget(saveButton);
      buttonLayout->addWidget(revertButton);
      buttonLayout->addWidget(quitButton);

      layout->addWidget(edit_);
      layout->addWidget(errorLabel_);
      layout->addLayout(buttonLayout);

      setLayout(layout);

      edit_->load(filename);

      connect(edit_, SIGNAL(textChanged()), this, SLOT(textChanged()));

      textChanged();
   }

   //! dtor
   virtual ~lgl_Qt_ShaderEditor()
   {
      edit_->save();
      edit_->write();
   }

protected:

   lgl_Qt_TextEdit *edit_;
   QLabel *errorLabel_;

   GLuint *id_;

   void limitString(QString &string, int lines)
   {
      int index = 0;
      int count = 0;

      while ((index = string.indexOf('\n', index)) >= 0)
      {
         count++;
         index++;

         if (count >= lines)
         {
            string.truncate(index);
            string.append("...");
            break;
         }
      }
   }

protected slots:

   void quit()
   {
      edit_->save(); // encoded extra file
      edit_->write(); // plain text file
      qApp->quit();
   }

   void load()
   {
      QString file = lglBrowseQtFile();

      if (file != "")
         edit_->read(file); // load from file
   }

   void save()
   {
      edit_->save(false); // embed into code
   }

   void revert()
   {
      edit_->revert(); // copy from code
   }

   void textChanged()
   {
      QString glsl = edit_->toPlainText();

      if (glsl == "")
      {
         glsl = lglPlainGLSLProgram().c_str();
         edit_->setPlainText(glsl);
      }
      else
      {
         if (id_)
         {
            GLuint id = lglCompileGLSLProgram(glsl.toStdString());

            if (id != 0)
            {
               if (*id_ != 0)
                  lglDeleteGLSLProgram(*id_);

               *id_ = id;

               lgl_Qt_GLWidget *w = dynamic_cast<lgl_Qt_GLWidget*>(parent());
               if (w) w->update();

               errorLabel_->clear();
               edit_->highlightLine();
            }
            else
            {
               QString error = lglGetError().c_str();
               limitString(error, 5);
               errorLabel_->setText(error);

               QRegExp rxVS("^.*vertex.shader.errors:");
               if (rxVS.indexIn(error) >= 0)
               {
                  QRegExp rxLN(":\\s*\\d+[:(](\\d+)");
                  if (rxLN.indexIn(error) >= 0)
                  {
                     int ln = rxLN.cap(1).toInt();
                     edit_->highlightLine(ln);
                  }
               }

               QRegExp rxFS("^.*fragment.shader.errors:");
               if (rxFS.indexIn(error) >= 0)
               {
                  QRegExp rxLN(":\\s*\\d+[:(](\\d+)");
                  if (rxLN.indexIn(error) >= 0)
                  {
                     int ln = rxLN.cap(1).toInt();
                     ln += edit_->document()->find("---").blockNumber()+1;
                     edit_->highlightLine(ln);
                  }
               }
            }
         }
      }
   }

   void help()
   {
      QMessageBox msgBox;

      msgBox.setText("The programmable computer graphics pipeline features two shaders: "
                     "a programmable vertex and a programmable fragment shader. "
                     "In the shader editor the two shaders are separated by ---.\n"
                     "\n"
                     "The vertex shader transforms the incoming vertices with the model-view-projection "
                     "matrix and writes the resulting vector into the \"gl_Position\" output register. "
                     "The model-view-projection matrix is available as uniform matrix \"mvp\" and "
                     "the vertices are available in the attribute vector \"vertex_position\".\n"
                     "\n"
                     "The following uniforms and attributes are available in the vertex shader:\n"
                     " - uniform vec4 color: actual color\n"
                     " - vec4 vertex_position: incoming vertices\n"
                     " - vec4 vertex_color: incoming color attribute of each vertex\n"
                     " - vec3 vertex_normal: incoming normal vectors of each vertex\n"
                     " - vec4 vertex_texcoord: incoming texture coordinates of each vertex\n"
                     " - uniform mat4 mv: model-view matrix\n"
                     " - uniform mat4 mvp: combined model-view-projection matrix\n"
                     " - uniform mat4 mvit: inverse transpose of the model-view matrix\n"
                     "\n"
                     "The fragment shader computes a color for each incoming fragment with "
                     "screen space position \"gl_FragCoord\" and writes the resulting color "
                     "vector into the \"gl_FragColor\" output register.\n"
                     "\n"
                     "In the fragment shader there are no attributes available, but arbitrary "
                     "vector data can be passed from the vertex to the fragment shader by "
                     "specifying a varying register in both shaders. The varying register acts "
                     "as a transmission channel between the two shaders, where the vector data to "
                     "be passed is written into the varying register on the vertex shader side "
                     "and the per-pixel interpolated values can be read from the varying register "
                     "on the fragment shader side.\n"
                     "\n"
                     "A common use case for a varying register is to pass the per-vertex color "
                     "attribute \"vertex_color\" from the vertex shader to the fragment shader "
                     "and write the interpolated colors into the shader's color output register.\n"
                     "\n");

      QSpacerItem *horizontalSpacer = new QSpacerItem(1000, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
      QGridLayout *layout = (QGridLayout*)msgBox.layout();
      layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

      msgBox.exec();
   }

};

#endif
