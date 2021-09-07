#ifndef SETTINGSSHADOWSFINDDIALOG_H
#define SETTINGSSHADOWSFINDDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsShadowsFindDialog;
}

class SettingsShadowsFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsShadowsFindDialog(QWidget *parent = nullptr);
    ~SettingsShadowsFindDialog();

private:
    Ui::SettingsShadowsFindDialog *ui;
};

#endif // SETTINGSSHADOWSFINDDIALOG_H
