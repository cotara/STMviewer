#ifndef ASYNCHRONBUTTON_H
#define ASYNCHRONBUTTON_H

#include <QPushButton>
#include <QWidget>
#include "entervaluewidget.h"

class AsynchronButton : public QPushButton
{
    Q_OBJECT
public:
    AsynchronButton(QWidget *parent = nullptr, int min=0, int max=255);

private:
    EnterValueWidget *enterDialog;

signals:
    void sendValue(int val);
};

#endif // ASYNCHRONBUTTON_H
