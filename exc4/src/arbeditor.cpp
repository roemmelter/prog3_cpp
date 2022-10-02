// (c) by Stefan Roettger, licensed under MIT license

#include "gfx/shaderprogram.h"

#include "arbeditor.h"

ARBProgramEditorWindow::ARBProgramEditorWindow(QWidget *parent)
   : QWidget(parent)
{
   QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
   QSplitter *mainSplitter = new QSplitter;
   QGroupBox *editGroup = new QGroupBox();
   QBoxLayout *editLayout = new QBoxLayout(QBoxLayout::TopToBottom);

   arbwindow_ = new QGLARBWindow();

   vtxshader_ = new ARBTextEdit();
   frgshader_ = new ARBTextEdit();

   vtxstatus_ = new QLabel();
   frgstatus_ = new QLabel();

   mainLayout->addWidget(mainSplitter);

   mainSplitter->addWidget(arbwindow_);
   mainSplitter->addWidget(editGroup);
   editGroup->setLayout(editLayout);

   QBoxLayout *gfxLayout = new QBoxLayout(QBoxLayout::LeftToRight);

   QPushButton *gfxCubeButton = new QPushButton("Cube");
   gfxLayout->addWidget(gfxCubeButton);
   connect(gfxCubeButton, SIGNAL(clicked()), this, SLOT(gfxCube()));

   QPushButton *gfxTeapotButton = new QPushButton("Teapot");
   gfxLayout->addWidget(gfxTeapotButton);
   connect(gfxTeapotButton, SIGNAL(clicked()), this, SLOT(gfxTeapot()));

   editLayout->addLayout(gfxLayout);

   QPushButton *vtxLoadButton = new QPushButton("Vertex Shader Program");
   editLayout->addWidget(vtxLoadButton);
   connect(vtxLoadButton, SIGNAL(clicked()), this, SLOT(vtxLoad()));

   editLayout->addWidget(vtxshader_);

   QPushButton *frgLoadButton = new QPushButton("Fragment Shader Program");
   editLayout->addWidget(frgLoadButton);
   connect(frgLoadButton, SIGNAL(clicked()), this, SLOT(frgLoad()));

   editLayout->addWidget(frgshader_);

   mainLayout->addWidget(vtxstatus_);
   mainLayout->addWidget(frgstatus_);

   vtxstatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   vtxstatus_->setWordWrap(true);

   frgstatus_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   frgstatus_->setWordWrap(true);

   setLayout(mainLayout);

   arbwindow_->setARBVertexProgram(default_vtxprg);
   arbwindow_->setARBFragmentProgram(default_frgprg);

   vtxshader_->setText(default_vtxprg);
   frgshader_->setText(default_frgprg);

   connect(vtxshader_, SIGNAL(textChanged()), this, SLOT(reloadShaders()));
   connect(frgshader_, SIGNAL(textChanged()), this, SLOT(reloadShaders()));

   connect(arbwindow_, SIGNAL(vtxstatus(QString)), this, SLOT(vtxstatus(QString)));
   connect(arbwindow_, SIGNAL(frgstatus(QString)), this, SLOT(frgstatus(QString)));
}

ARBProgramEditorWindow::~ARBProgramEditorWindow()
{}

void ARBProgramEditorWindow::gfxCube()
{
   arbwindow_->enableTeapot(false);
}

void ARBProgramEditorWindow::gfxTeapot()
{
   arbwindow_->enableTeapot(true);
}

void ARBProgramEditorWindow::reloadShaders()
{
   QString vtxshader = vtxshader_->document()->toPlainText();
   QString frgshader = frgshader_->document()->toPlainText();

   arbwindow_->setARBVertexProgram(vtxshader.toStdString().c_str());
   arbwindow_->setARBFragmentProgram(frgshader.toStdString().c_str());
}

void ARBProgramEditorWindow::vtxstatus(QString e)
{
   if (e == "") e = "ok";
   vtxstatus_->setText("vertex program status: "+e);
}

void ARBProgramEditorWindow::frgstatus(QString e)
{
   if (e == "") e = "ok";
   frgstatus_->setText("fragment program status: "+e);
}

void ARBProgramEditorWindow::vtxLoad()
{
   QString file = browse("Load Vertex Shader Program");

   if (file != "")
   {
      QString text = readText(file);
      vtxshader_->setText(text);
   }
}

void ARBProgramEditorWindow::frgLoad()
{
   QString file = browse("Load Fragment Shader Program");

   if (file != "")
   {
      QString text = readText(file);
      frgshader_->setText(text);
   }
}

QString ARBProgramEditorWindow::browse(QString title,
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
   fd->setNameFilter(tr("All Files (*);;Shader Program Files (*.vtx,*.frg)"));
#else
   fd->setFilter(tr("All Files (*);;Shader Program Files (*.vtx,*.frg)"));
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

QString ARBProgramEditorWindow::readText(QString file)
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
