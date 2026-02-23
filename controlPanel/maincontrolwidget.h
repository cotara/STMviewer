#ifndef MAINCONTROLWIDGET_H
#define MAINCONTROLWIDGET_H

#include <QWidget>

class MainControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainControlWidget(QWidget *parent = nullptr);

signals:
private:

    QVBoxLayout *layout;


};

#endif // MAINCONTROLWIDGET_H
