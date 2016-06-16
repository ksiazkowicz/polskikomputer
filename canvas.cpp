#include "canvas.h"
#include <QPainter>
#include <QDebug>

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    currentIndex = 0;
}
void Canvas::paintEvent(QPaintEvent *)
{
     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing);

     QRect r = this->rect();
     int side = std::min(r.width(), r.height());
     painter.translate(0.5*r.width(), 0.5*height());
     painter.scale(side/200.0, side/200.0);

     painter.setBrush(QBrush(QColor("black")));
     painter.setPen(Qt::NoPen);

     painter.drawEllipse(QPoint(0,0), 6, 6);

     painter.setBrush(QBrush(QColor("yellow")));
     const int N = 8;
     for (int i = 0; i < N; i++)
     {
         if (currentIndex != i)
            painter.drawEllipse(QPoint(0,70), 20, 20);
         painter.rotate(360.0/N);
     }
}

void Canvas::wez_przeskocz() {
    currentIndex++;
    if (currentIndex == 8)
       currentIndex = 0;
    this->update();
}
