// (c) by Stefan Roettger, licensed under MIT license

#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#ifdef HAVE_QT5
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#else
#include <QtGui/QWidget>
#include <QPainter>
#include <QPalette>
#endif

#include <math.h>
#include <iostream>
#include "Complex"

class PainterWidget: public QWidget
{
public:

   //! default ctor
   PainterWidget(QWidget *parent = NULL)
      : QWidget(parent)
   {
       QPalette pal;
       pal.setColor(QPalette::Window, Qt::black);
       this->setPalette(pal);
       this->show();
   }

   //! dtor
   virtual ~PainterWidget()
   {}

   //! return preferred minimum window size
   QSize minimumSizeHint() const
   {
      return(QSize(800, 800));
   }

protected:

   // reimplemented paint event method
   void paintEvent(QPaintEvent *);
   bool is_in_range(double query, double min, double max);
   void drawMandelBrotSet(Complex constant, int max_iteration, QPainter *painter);
   void drawJuliaSet(Complex constant, int max_iteration, QPainter *painter);
};

#endif
