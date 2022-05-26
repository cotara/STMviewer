#include "entervaluewidget.h"
#include "ui_entervaluewidget.h"
#include <QPushButton>
#include <QFile>

EnterValueWidget::EnterValueWidget(QWidget *parent, int min, int max) :
    QDialog(parent),
    ui(new Ui::EnterValueWidget)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Отправить");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");
    setObjectName("entervaluewidget");
    ui->spinBox->setMinimum(min);
    ui->spinBox->setMaximum(max);
    ui->spinBox->setFocus();
}

EnterValueWidget::~EnterValueWidget()
{
    delete ui;
}

void EnterValueWidget::setSpinBox(int n){
    ui->spinBox->setValue(n);
}

void EnterValueWidget::on_buttonBox_accepted()
{
   if(ui->spinBox->value()!=oldValue){
       emit valueChanged();
       emit sendValue(ui->spinBox->value());
       oldValue = ui->spinBox->value();
       //Чтобы этот диалог, являющийся частью  асинхронной кнопки имел обычные кнопки внутри себя
       QFile file(":/qss/styleWhiteButtons.css");
       if(file.open(QFile::ReadOnly))
             this->setStyleSheet(file.readAll());
   }
   ui->spinBox->setFocus();
}

void EnterValueWidget::on_buttonBox_rejected()
{
    setFocus();
}

void EnterValueWidget::setFocus()
{
    ui->spinBox->setFocus();
}

