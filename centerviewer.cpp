#include "centerviewer.h"

centerViewer::centerViewer(QWidget *parent): QWidget(parent) {
    layoutV = new QVBoxLayout;
    setLayout(layoutV);

}

void centerViewer::setCoord(int x, int y)
{
    xPos=x;
    yPos=y;
    repaint();
}

void centerViewer::setDiam(int x, int y)
{
    xDiam=x;
    yDiam=y;
}


void centerViewer::paintEvent(QPaintEvent *e) {

  Q_UNUSED(e);

  QPainter qp(this);
  addCircle(&qp);
  paintPosition(&qp,static_cast<int>(xPos),static_cast<int>(yPos));
}
void centerViewer::addCircle(QPainter *qp)
{
    qp->translate(width() / 2, height() / 2);
    qp->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));

    qp->drawEllipse(0, 0, 100, 100);
    qp->drawLine(45,50,55,50);
    qp->drawLine(50,45,50,55);
}
void centerViewer::paintPosition(QPainter *qp,int x, int y)
{
    qp->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    QBrush brush(Qt::red,Qt::SolidPattern);
    qp->setBrush(brush);
    qp->drawEllipse(50+x*10, 50+y*10, 20, 20);
}
