#include "errormarker.h"
#include "ui_errormarker.h"

ErrorMarker::ErrorMarker(QWidget *parent, const QString &title)
    : QWidget(parent)
    , ui(new Ui::ErrorMarker)
{
    ui->setupUi(this);
    pixDouble.load(":/1/Resources/doubleset.png");
    pixNone.load(":/1/Resources/empty-set.png");
    pixOk.load(":/1/Resources/okSet.png");
    ui->titleLabel->setText(title);

    ui->lineLeft->setStyleSheet("background-color: red;");
    ui->lineRight->setStyleSheet("background-color: red;");
    ui->lineRight->setFrameShadow(QFrame::Sunken);
    ui->lineLeft->setFrameShadow(QFrame::Sunken);
}

ErrorMarker::~ErrorMarker()
{
    delete ui;
}

void ErrorMarker::setError(int err)
{
    pixNone.scaled(ui->label->size(),
                   Qt::KeepAspectRatioByExpanding,
                   Qt::SmoothTransformation);
    pixDouble.scaled(ui->label->size(),
                   Qt::KeepAspectRatioByExpanding,
                   Qt::SmoothTransformation);

    if(err&0b00000001)
        ui->label->setPixmap(pixDouble);
    else if(err&0b00000010)
        ui->label->setPixmap(pixNone);
    else
        ui->label->setPixmap(pixOk);

    if(err&0b00000100)
        ui->lineRight->setStyleSheet("background-color: red;");
    else
        ui->lineRight->setStyleSheet("background-color: gray;");
    if(err&0b00001000)
        ui->lineLeft->setStyleSheet("background-color: red;");
    else
       ui->lineLeft->setStyleSheet("background-color: gray;");

}
