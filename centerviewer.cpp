#include "centerviewer.h"

centerViewer::centerViewer(QWidget *parent): QWidget(parent) {
    layoutV = new QVBoxLayout;
    setLayout(layoutV);
}

void centerViewer::setCoord(int x, int y)
{
    xPos=x;
    yPos=y;
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
  paintPosition(&qp,xPos,yPos);
}
void centerViewer::addCircle(QPainter *qp)
{
    qp->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));

    qp->drawEllipse(0, 0, 100, 100);
    qp->drawLine(50,55,60,55);
    qp->drawLine(55,50,55,60);
}
void centerViewer::paintPosition(QPainter *qp,int x, int y)
{
    qp->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    QBrush brush(Qt::red,Qt::SolidPattern);
    qp->setBrush(brush);
    qp->drawEllipse(50+x/10.0, 50+y/10.0, 10, 10);
}
