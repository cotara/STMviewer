#ifndef SIGNALERRWIDGET_H
#define SIGNALERRWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <QGroupBox>

class SignalErrWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit SignalErrWidget(QWidget *parent = nullptr);
    void setVal(char val, int ch);
signals:

private:
    QHBoxLayout *err1Layout, *err2Layout;
    QVBoxLayout *errorsLayout;
    QLCDNumber *err11,*err12,*err13,*err14,*err21,*err22,*err23,*err24;

};

#endif // SIGNALERRWIDGET_H
