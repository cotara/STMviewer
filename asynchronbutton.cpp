#include "asynchronbutton.h"
#include <QFile>

AsynchronButton::AsynchronButton(QWidget *parent, int min, int max):QPushButton(parent)
{
     enterDialog = new EnterValueWidget (this,min,max);

     connect(this,&QPushButton::clicked, [=]{enterDialog->show();});

     connect(enterDialog,&EnterValueWidget::sendValue, [=](int i)
         {emit sendValue(i);});

     connect(enterDialog,&EnterValueWidget::valueChanged,[=]{
         QFile file(":/qss/styleYelowButtons.css");
           if(file.open(QFile::ReadOnly))
               setStyleSheet(file.readAll());
         });
}
