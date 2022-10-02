// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! Qt convenience functions
#ifndef GLVERTEX_QT_H
#define GLVERTEX_QT_H

#include <qglobal.h>
#include <qdebug.h>

#ifdef Q_OS_ANDROID
#   define LGL_USE_GLES
#endif

#include "glvertex.h"

#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QImage>

inline QString lglBrowseQtFile(QString title = "Browse file",
                               QString path = "",
                               bool newfile = false)
{
   QString file;

   QFileDialog* fd = new QFileDialog(NULL, title);

   if (!newfile) fd->setFileMode(QFileDialog::ExistingFile);
   else fd->setFileMode(QFileDialog::AnyFile);
   fd->setViewMode(QFileDialog::List);
   if (newfile) fd->setAcceptMode(QFileDialog::AcceptSave);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
   fd->setFilter(QDir::Dirs|QDir::Files);
   fd->setNameFilter("All Text Files (*.txt);;All Files (*)");
#else
   fd->setFilter("All Text Files (*.txt);;All Files (*)");
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

//! load OBJ into vbo from qt resource path
inline lglVBO *lglLoadObj(QString qrc, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
{
   lglVBO *vbo = NULL;

   QFile file(qrc);
   if (file.open(QIODevice::ReadOnly))
   {
      QByteArray a = file.readAll();
      vbo = lglLoadObjData(QString(a).toStdString().c_str(), texgen);
   }
   else
   {
      if (!silent) lglError("cannot open obj file");
   }

   return(vbo);
}

//! load qt image into texture object from qt resource path
inline GLuint lglLoadQtTextureInto(QString qrc, int *iwidth, int *iheight, int *twidth, int *theight, bool mipmapping = true)
{
   GLuint texid = 0;

   QImage image(qrc);

   if (!image.isNull())
   {
      *iwidth = *twidth = image.width();
      *iheight = *theight = image.height();

      image = image.convertToFormat(QImage::Format_ARGB32);

      unsigned int size = 4*(*twidth)*(*theight);

      unsigned char *argb = image.bits();
      unsigned char *rgba = new unsigned char[size];

      for (unsigned int i=0; i<size; i+=4)
      {
         rgba[i] = argb[i+2];
         rgba[i+1] = argb[i+1];
         rgba[i+2] = argb[i];
         rgba[i+3] = argb[i+3];
      }

      if (mipmapping)
         texid = lglCreateMipmap2D(twidth, theight, LGL_RGBA, rgba);
      else
         texid = lglCreateTexmap2D(twidth, theight, LGL_RGBA, rgba);

      delete[] rgba;
   }

   return(texid);
}

//! load qt image into texture object from qt resource path (and from default installation paths)
inline GLuint lglLoadQtTexture(QString qrc, int *iwidth, int *iheight, int *twidth, int *theight, bool mipmapping = true)
{
   GLuint texid = lglLoadQtTextureInto(qrc, iwidth, iheight, twidth, theight, mipmapping);

   if (texid == 0) texid = lglLoadQtTextureInto(QString("../") + qrc, iwidth, iheight, twidth, theight, mipmapping);
   if (texid == 0) texid = lglLoadQtTextureInto(QString("data/") + qrc, iwidth, iheight, twidth, theight, mipmapping);
   if (texid == 0) texid = lglLoadQtTextureInto(QString("../data/") + qrc, iwidth, iheight, twidth, theight, mipmapping);
#ifndef _WIN32
   if (texid == 0) texid = lglLoadQtTextureInto(QString("/usr/local/share/") + qrc, iwidth, iheight, twidth, theight, mipmapping);
#else
   if (texid == 0) texid = lglLoadQtTextureInto(QString("C:\\glvertex\\") + qrc, iwidth, iheight, twidth, theight, mipmapping);
#endif

   return(texid);
}

//! load qt image into texture object from qt resource path (and from default installation paths)
inline GLuint lglLoadQtTexture(QString qrc, bool mipmapping = true)
{
   int iwidth, iheight, twidth, theight;
   return(lglLoadQtTexture(qrc, &iwidth, &iheight, &twidth, &theight, mipmapping));
}

//! load qt image into texture object from qt resource path (and from default installation paths)
inline GLuint lglLoadQtTexture(QString qrc, int *iwidth, int *iheight, bool mipmapping = true)
{
   int twidth, theight;
   return(lglLoadQtTexture(qrc, iwidth, iheight, &twidth, &theight, mipmapping));
}

#endif
