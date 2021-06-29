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
    void setCoord(int x, int y);
    void setDiam(int x, int y);
    private:
    QVBoxLayout *layoutV;
    void paintEvent(QPaintEvent *event);
    void addCircle(QPainter *qp);
    void paintPosition(QPainter *qp,int x, int y);
    int xPos=0,yPos=0;
    int xDiam=0,yDiam=0;
};

#endif // CENTERVIEWER_H
