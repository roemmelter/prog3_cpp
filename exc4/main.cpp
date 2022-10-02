#include <locale.h>

#include "mainwindow.h"

#ifdef HAVE_QT5
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   setlocale(LC_NUMERIC, "C");

   MainWindow main;
   main.show();

   return(app.exec());
}
