#include "centerviewer.h"

centerViewer::centerViewer(QWidget *parent): QWidget(parent) {
    setObjectName("centerviewer");
    layoutV = new QVBoxLayout;
    setLayout(layoutV);

}

void centerViewer::setCoord(double x, double y)
{
    xPos=x;
    yPos=y;
    repaint();
}

void centerViewer::setRad(double x,double y)
{
    xRad=x;
    yRad=y;
}


void centerViewer::paintEvent(QPaintEvent *e) {

  Q_UNUSED(e);

  QPainter qp(this);
  addCircle(&qp);
  paintPosition(&qp,xPos,yPos,xRad,yRad);
}
void centerViewer::addCircle(QPainter *qp)
{

    qp->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));

    qp->drawEllipse(0, 0, 100, 100);
    qp->drawLine(45,50,55,50);
    qp->drawLine(50,45,50,55);
}
void centerViewer::paintPosition(QPainter *qp,double x, double y,double xRad, double yRad)
{
    QPointF m_point(5*x + 50, 5*y + 50);
    qp->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    QBrush brush(Qt::red,Qt::SolidPattern);
    qp->setBrush(brush);
    qp->drawEllipse(m_point, 5*xRad, 5*yRad);
}
