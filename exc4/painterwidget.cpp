// (c) by Stefan Roettger, licensed under MIT license

#include "painterwidget.h"

// reimplemented paint event method
void PainterWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    drawMandelBrotSet(Complex(-0.123, 0.765), 100, &painter);
}

bool PainterWidget::is_in_range(double query, double min, double max) {
    if (min <= query && query < max) return true;
    return false;
}

void PainterWidget::drawMandelBrotSet(Complex constant, int max_iteration, QPainter *painter) {

    double h = height() - 200;

    for (int i = 0; i < width(); i++) {
        for (int j = 0; j < height(); j++) {

            // nomalize coordinates
            double x = (double) i / (h - 1);
            double y = (double) j / (h - 1);

            // translate coordinate system
            x = x - 1;
            y = y - 0.66;

            // scale coordinates
            //x *= (width()-1);
            x *= 2;
            //y *= (height()-1);
            y *= 2;

            int counter = 0;
            Complex c(x,y);
            Complex z(x,y);

            while (z.norm() < 4) {
                z = z*z + c;
                if (counter++ > max_iteration) break;
            }

            double red = 0;
            double green = 0;
            double blue = 0;

            if (is_in_range(counter, 0, 500)) {
                red = sqrt((double) (counter/max_iteration)) * 255;
            } else if (is_in_range(counter, 5000, 10000)) {
                green = sqrt((double) (counter/max_iteration)) * 255;
            } else if (is_in_range(counter, 10000, 15000)) {
                blue = sqrt((double) (counter/max_iteration)) * 255;
            }

            painter->setPen(QColor(red, green, blue));
            //if (counter >= max_iteration)
            painter->drawPoint(i,j);
            //painter->drawPoint( QPointF(z.re, z.im));
        }
    }
}

void PainterWidget::drawJuliaSet(Complex constant, int max_iteration, QPainter *painter) {

    for (int i = 0; i < width(); i++) {
        for (int j = 0; j < height(); j++) {
            double x = (double) i / (height() - 1) - 0.5;
            double y = 0.5 - (double) j / (height() - 1);

            x *= 3;
            y *= 3;


            Complex c(constant);
            Complex z(x, y);

            for (int i = 0; i < max_iteration; i++) {
                if (z.norm() >= 4) break;
                z = z * z + c;

                QPointF point(z.re, z.im);
                painter->drawPoint(point);
            }
        }
    }
}
