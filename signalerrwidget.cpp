#include "signalerrwidget.h"

SignalErrWidget::SignalErrWidget(QWidget *parent) : QGroupBox(parent)
{ 
    setTitle("Ошибки");
    //Ошибки
    errorsLayout = new QVBoxLayout(this);       //Т.к. родителем назначен текущий виджет, то setLayout не нужен
    err1Layout = new QHBoxLayout();
    err2Layout = new QHBoxLayout();
    errorsLayout->addLayout(err1Layout);
    errorsLayout->addLayout(err2Layout);


    err11 = new QLCDNumber(1,this);
    err12 = new QLCDNumber(1,this);
    err13 = new QLCDNumber(1,this);
    err14 = new QLCDNumber(1,this);
    err21 = new QLCDNumber(1,this);
    err22 = new QLCDNumber(1,this);
    err23 = new QLCDNumber(1,this);
    err24 = new QLCDNumber(1,this);

    err1Layout->addWidget(err11);
    err1Layout->addWidget(err12);
    err1Layout->addWidget(err13);
    err1Layout->addWidget(err14);
    err2Layout->addWidget(err21);
    err2Layout->addWidget(err22);
    err2Layout->addWidget(err23);
    err2Layout->addWidget(err24);

    err11->display("2");
    err12->display("0");
    err13->display("R");
    err14->display("L");
    err21->display("2");
    err22->display("0");
    err23->display("R");
    err24->display("L");
}

void SignalErrWidget::setVal(char val, int ch)
{
    if(ch==1){
        if(val&0b00000001)
            err11->setPalette(QPalette(Qt::red));
        else
           err11->setPalette(QPalette(Qt::black));

        if(val&0b00000010)
            err12->setPalette(QPalette(Qt::red));
        else
           err12->setPalette(QPalette(Qt::black));

        if(val&0b00000100)
            err13->setPalette(QPalette(Qt::red));
        else
           err13->setPalette(QPalette(Qt::black));
        if(val&0b00001000)
            err14->setPalette(QPalette(Qt::red));
        else
           err14->setPalette(QPalette(Qt::black));
    }
    else if(ch==2){
        if(val&0b00000001)
            err21->setPalette(QPalette(Qt::red));
        else
           err21->setPalette(QPalette(Qt::black));

        if(val&0b00000010)
            err22->setPalette(QPalette(Qt::red));
        else
           err22->setPalette(QPalette(Qt::black));

        if(val&0b00000100)
            err23->setPalette(QPalette(Qt::red));
        else
           err23->setPalette(QPalette(Qt::black));
        if(val&0b00001000)
            err24->setPalette(QPalette(Qt::red));
        else
           err24->setPalette(QPalette(Qt::black));
    }

}
