#include "savelog.h"
#include "ui_savelog.h"
#include <QFileDialog>
#include <QPushButton>
SaveLog::SaveLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveLog)
{
    ui->setupUi(this);
    connect(ui->buttonBox,&QDialogButtonBox::accepted,[=]{emit SaveToFolder(dirname);});
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Сохранить");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");
}

SaveLog::~SaveLog()
{
    delete ui;
}

void SaveLog::on_toolButton_clicked()
{
    dirname = QFileDialog::getExistingDirectory(this, "Сохранить лог в... ");
    ui->SaveLogsLabel->setText(dirname);
}
