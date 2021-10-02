#include "entervaluewidget.h"
#include "ui_entervaluewidget.h"
#include <QPushButton>

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

void EnterValueWidget::on_buttonBox_accepted()
{
   if(ui->spinBox->value()!=oldValue){
       emit valueChanged();
       oldValue = ui->spinBox->value();
   }
   emit sendValue(ui->spinBox->value());

}
