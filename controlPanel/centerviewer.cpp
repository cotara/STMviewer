#include "centerviewer.h"

centerViewer::centerViewer(QWidget *parent, int scale): QWidget(parent) {
    setObjectName("centerviewer");
    layoutV = new QVBoxLayout;
    setLayout(layoutV);
    m_scale = scale;
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
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

void centerViewer::setScale(int scale)
{
    m_scale = scale;
}


void centerViewer::paintEvent(QPaintEvent *e) {

  Q_UNUSED(e);

  widgetCenter.setX(this->size().width()/2);
  widgetCenter.setY(this->size().height()/2);
  QPainter qp(this);
  addCircle(&qp);
  paintPosition(&qp);
}
void centerViewer::addCircle(QPainter *qp)
{

    qp->setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
    int minDimention = qMin(widgetCenter.x(),widgetCenter.y());
    qp->drawEllipse(widgetCenter, minDimention-3, minDimention-3);
    //Рисуем перекрестие
    QPoint d1(widgetCenter.x(),widgetCenter.y()+0.1*widgetCenter.y()),
           d2(widgetCenter.x(),widgetCenter.y()-0.1*widgetCenter.y()),
           d3(widgetCenter.x()-0.1*widgetCenter.y(),widgetCenter.y()),
           d4(widgetCenter.x()+0.1*widgetCenter.y(),widgetCenter.y());

    qp->drawLine(d1,d2);
    qp->drawLine(d3,d4);
}
void centerViewer::paintPosition(QPainter *qp)
{//Размер круга зависист от m_scale
    qp->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    QBrush brush(Qt::red,Qt::SolidPattern);
    qp->setBrush(brush);
    double mmToPixScale = widgetCenter.y()*2.0/m_scale;
    QPointF rot (m_angle*mmToPixScale*(yPos-xPos),-m_angle*mmToPixScale*(-xPos-yPos));
    QPointF center = static_cast<QPointF>(widgetCenter) + rot;
    //QPointF center = static_cast<QPointF>(widgetCenter) + QPointF(xPos*mmToPixScale,yPos*mmToPixScale);
    if(xRad > 0 && yRad>0){
        qp->drawEllipse(center, xRad*mmToPixScale, yRad*mmToPixScale);
    }
}
