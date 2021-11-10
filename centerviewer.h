#ifndef CENTERVIEWER_H
#define CENTERVIEWER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPainter>
class centerViewer : public QWidget
{
    Q_OBJECT

    public:
    centerViewer(QWidget *parent=0);
    void setCoord(double x, double y);
    void setRad(double x,double y);
    private:
    QVBoxLayout *layoutV;
    void paintEvent(QPaintEvent *event);
    void addCircle(QPainter *qp);
    void paintPosition(QPainter *qp,double x, double y, double xRad, double yRad);
    double xPos=0,yPos=0;
    double xRad=0,yRad=0;
};

#endif // CENTERVIEWER_H
