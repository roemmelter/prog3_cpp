// (c) by Stefan Roettger, licensed under MIT license

#include "glsleditor.h"

#include <QRegExp>

GLSLProgramEditorWindow::GLSLProgramEditorWindow(QWidget *parent)
   : QWidget(parent)
{
   QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
   QSplitter *mainSplitter = new QSplitter;
   QGroupBox *editGroup = new QGroupBox();
   QBoxLayout *editLayout = new QBoxLayout(QBoxLayout::TopToBottom);

   glslwindow_ = new QGLGLSLWindow();

   vtxshader_ = new GLSLTextEdit();
   frgshader_ = new GLSLTextEdit();

   vtxstatus_ = new QLabel();
   frgstatus_ = new QLabel();
   lnkstatus_ = new QLabel();
   wrnstatus_ = new QLabel();

   mainLayout->addWidget(mainSplitter);

   mainSplitter->addWidget(glslwindow_);
   mainSplitter->addWidget(editGroup);
   editGroup->setLayout(editLayout);

   QPushButton *loadObjButton = new QPushButton("Load OBJ");
   editLayout->addWidget(loadObjButton);
   connect(loadObjButton, SIGNAL(clicked()), this, SLOT(loadObj()));

   QPushButton *vtxLoadButton = new QPushButton("Vertex Shader");
   editLayout->addWidget(vtxLoadButton);
   connect(vtxLoadButton, SIGNAL(clicked()), this, SLOT(vtxLoad()));

   editLayout->addWidget(vtxshader_);

   QPushButton *frgLoadButton = new QPushButton("Fragment Shader");
   editLayout->addWidget(frgLoadButton);
   connect(frgLoadButton, SIGNAL(clicked()), this, SLOT(frgLoad()));

   editLayout->addWidget(frgshader_);

   mainLayout->addWidget(vtxstatus_);
   mainLayout->addWidget(frgstatus_);
   mainLayout->addWidget(lnkstatus_);
   mainLayout->addWidget(wrnstatus_);

   vtxstatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   vtxstatus_->setWordWrap(true);

   frgstatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   frgstatus_->setWordWrap(true);

   lnkstatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   lnkstatus_->setWordWrap(true);

   wrnstatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   wrnstatus_->setWordWrap(true);

   QHBoxLayout *footer = new QHBoxLayout;
   QPushButton *backgroundButton = new QPushButton("Toggle background");
   QPushButton *ztestButton = new QPushButton("Toggle z-test");
   QPushButton *wireframeButton = new QPushButton("Toggle wire frame");
   QPushButton *helpButton = new QPushButton("Help");
   QPushButton *quitButton = new QPushButton("Quit");
   footer->addWidget(backgroundButton);
   footer->addWidget(ztestButton);
   footer->addWidget(wireframeButton);
   footer->addStretch();
   footer->addWidget(helpButton);
   footer->addWidget(quitButton);
   mainLayout->addLayout(footer);

   setLayout(mainLayout);

   vtxshader_->setText(lglPlainGLSLVertexShader().c_str());
   frgshader_->setText(lglPlainGLSLFragmentShader().c_str());

   connect(vtxshader_, SIGNAL(textChanged()), this, SLOT(reloadShaders()));
   connect(frgshader_, SIGNAL(textChanged()), this, SLOT(reloadShaders()));

   connect(glslwindow_, SIGNAL(vtxstatus(QString)), this, SLOT(vtxstatus(QString)));
   connect(glslwindow_, SIGNAL(frgstatus(QString)), this, SLOT(frgstatus(QString)));
   connect(glslwindow_, SIGNAL(lnkstatus(QString)), this, SLOT(lnkstatus(QString)));
   connect(glslwindow_, SIGNAL(wrnstatus(QString)), this, SLOT(wrnstatus(QString)));

   connect(backgroundButton, SIGNAL(clicked()), this, SLOT(background()));
   connect(ztestButton, SIGNAL(clicked()), this, SLOT(ztest()));
   connect(wireframeButton, SIGNAL(clicked()), this, SLOT(wireframe()));
   connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
   connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

   reloadShaders();
}

GLSLProgramEditorWindow::~GLSLProgramEditorWindow()
{}

void GLSLProgramEditorWindow::reloadShaders()
{
   QString vtxshader = vtxshader_->document()->toPlainText();
   QString frgshader = frgshader_->document()->toPlainText();

   glslwindow_->setGLSLVertexShader(vtxshader.toStdString().c_str());
   glslwindow_->setGLSLFragmentShader(frgshader.toStdString().c_str());

   wrnstatus_->setText("shader warnings: none");
}

void GLSLProgramEditorWindow::vtxstatus(QString e)
{
   if (e == "") e = "vertex shader status: ok";
   vtxstatus_->setText(e);

   QRegExp rxVS("^.*vertex.shader.errors:");
   if (rxVS.indexIn(e) >= 0)
   {
      QRegExp rxLN(":\\s*\\d+[:(](\\d+)");
      if (rxLN.indexIn(e) >= 0)
      {
         int ln = rxLN.cap(1).toInt();
         vtxshader_->highlightLine(ln);
      }
   }
   else
      vtxshader_->highlightLine();
}

void GLSLProgramEditorWindow::frgstatus(QString e)
{
   if (e == "") e = "fragment shader status: ok";
   frgstatus_->setText(e);

   QRegExp rxFS("^.*fragment.shader.errors:");
   if (rxFS.indexIn(e) >= 0)
   {
      QRegExp rxLN(":\\s*\\d+[:(](\\d+)");
      if (rxLN.indexIn(e) >= 0)
      {
         int ln = rxLN.cap(1).toInt();
         frgshader_->highlightLine(ln);
      }
   }
   else
      frgshader_->highlightLine();
}

void GLSLProgramEditorWindow::lnkstatus(QString e)
{
   if (e == "") e = "shader linker status: ok";
   lnkstatus_->setText(e);
}

void GLSLProgramEditorWindow::wrnstatus(QString e)
{
   if (e != "")
      wrnstatus_->setText("shader warnings: "+e);
}

void GLSLProgramEditorWindow::loadObj()
{
   QString file = browse("Load Object File");

   if (file != "")
   {
      glslwindow_->loadObj(file.toStdString().c_str());
   }
}

void GLSLProgramEditorWindow::vtxLoad()
{
   QString file = browse("Load Vertex Shader");

   if (file != "")
   {
      if (file.endsWith(".glsl"))
      {
         QString text = readText(file);

         std::string vertex_shader;
         std::string fragment_shader;

         if (lglSplitGLSLProgram(text.toStdString(), vertex_shader, fragment_shader))
         {
            vtxshader_->setText(vertex_shader.c_str());
            frgshader_->setText(fragment_shader.c_str());
         }
      }
      else
      {
         QString text = readText(file);
         vtxshader_->setText(text);
      }
   }
}

void GLSLProgramEditorWindow::frgLoad()
{
   QString file = browse("Load Fragment Shader");

   if (file != "")
   {
      if (file.endsWith(".glsl"))
      {
         QString text = readText(file);

         std::string vertex_shader;
         std::string fragment_shader;

         if (lglSplitGLSLProgram(text.toStdString(), vertex_shader, fragment_shader))
         {
            vtxshader_->setText(vertex_shader.c_str());
            frgshader_->setText(fragment_shader.c_str());
         }
      }
      else
      {
         QString text = readText(file);
         frgshader_->setText(text);
      }
   }
}

QString GLSLProgramEditorWindow::browse(QString title,
                                        QString path,
                                        bool newfile)
{
   QString file;

   QFileDialog* fd = new QFileDialog(this, title);

   if (!newfile) fd->setFileMode(QFileDialog::ExistingFile);
   else fd->setFileMode(QFileDialog::AnyFile);
   fd->setViewMode(QFileDialog::List);
   if (newfile) fd->setAcceptMode(QFileDialog::AcceptSave);
#ifdef HAVE_QT5
   fd->setFilter(QDir::Dirs|QDir::Files);
   fd->setNameFilter(tr("All Files (*);;Shader Files (*.vtx;*.frg;*.glsl);;Object Files (*.obj)"));
#else
   fd->setFilter(tr("All Files (*);;Shader Files (*.vtx;*.frg;*.glsl);;Object files (*.obj)"));
#endif

   if (path!="") fd->setDirectory(path);

   if (fd->exec() == QDialog::Accepted)
   {
      QStringList selected = fd->selectedFiles();

      if (selected.size() > 0)
         file = selected.at(0);
   }

   delete fd;

   setlocale(LC_NUMERIC, "C");

   return(file);
}

QString GLSLProgramEditorWindow::readText(QString file)
{
   QString text;

   QFile f(file);

   if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
      return("");

   QTextStream in(&f);
   while (!in.atEnd()) {
      QString line = in.readLine();
      text += line + "\n";
   }

   return(text);
}

void GLSLProgramEditorWindow::background()
{
   glslwindow_->toggle_background();
}

void GLSLProgramEditorWindow::ztest()
{
   glslwindow_->toggle_ztest();
}

void GLSLProgramEditorWindow::wireframe()
{
   glslwindow_->toggle_wireframe();
}

void GLSLProgramEditorWindow::help()
{
   QMessageBox msgBox;

   msgBox.setText("The programmable computer graphics pipeline features two shaders: "
                  "a programmable vertex and a programmable fragment shader.\n"
                  "\n"
                  "The vertex shader processes incoming triangles consisting of a sequence of "
                  "3D vertices by mapping them to the 2D image plane. Next, the triangles are "
                  "rasterized, which means that the triangles are divided into fragments. "
                  "For each of those fragments the fragment shader computes the final color, "
                  "which is displayed as a single pixel on screen.\n"
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
                  "The fragment shader computes a color for each incoming fragment and writes "
                  "the resulting color vector into the \"gl_FragColor\" output register.\n"
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
                  "\n"
                  "For texturing purposes the following texture samplers are pre-defined:\n"
                  " - tex2D: 2D checker-board texture -> use texture2D(tex2D, vec2(...))\n"
                  " - tex3D: 3D brick texture -> use texture3D(tex3D, vec3(...))\n"
                  "\n"
                  "As a first quick usage example of the shader editor, it is recommended to "
                  "modify the fragment shader to achieve a custom colorization effect:\n"
                  "\n"
                  "The editor's default fragment shader just writes a uniform color into "
                  "the color output register, so that all faces appear flat white. We can "
                  "elaborate on that by replacing one or more components of the output color "
                  "with \"sin(0.1f*gl_FragCoord.y)\". You will see a vertical striping effect. "
                  "Next, we use a varying to pass the color attributes to the fragment shader "
                  "and modulate those with the above color (by component-wise multiplication). "
                  "Now each face also shows its distinctive color.\n"
                  "\n"
                  "To see more shader effects, please load one of the shader examples in the "
                  "\"shaders\" directory.\n"
                  "\n"
                  "Enjoy!\n");

   QSpacerItem *horizontalSpacer = new QSpacerItem(1000, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
   QGridLayout *layout = (QGridLayout*)msgBox.layout();
   layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

   msgBox.exec();
}
