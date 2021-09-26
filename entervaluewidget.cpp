#include "entervaluewidget.h"
#include "ui_entervaluewidget.h"
#include <QPushButton>

EnterValueWidget::EnterValueWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterValueWidget)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Отправить");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");
    setObjectName("entervaluewidget");

}

EnterValueWidget::~EnterValueWidget()
{
    delete ui;
}

void EnterValueWidget::on_buttonBox_accepted()
{
   emit sendValue(ui->spinBox->value());
}
