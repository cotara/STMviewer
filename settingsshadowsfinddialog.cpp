#include "settingsshadowsfinddialog.h"
#include "ui_settingsshadowsfinddialog.h"

SettingsShadowsFindDialog::SettingsShadowsFindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsShadowsFindDialog)
{
    ui->setupUi(this);
}

SettingsShadowsFindDialog::~SettingsShadowsFindDialog()
{
    delete ui;
}
