#include "maincontrolwidget.h"

#include "centerviewer.h"

MainControlWidget::MainControlWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("maincontrolwidget");
    layout = new QVBoxLayout(this);

    //layout->addStretch(0);

    layout->setContentsMargins(0,0,0,0);

}
