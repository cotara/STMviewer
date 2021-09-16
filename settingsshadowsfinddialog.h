#ifndef SETTINGSSHADOWSFINDDIALOG_H
#define SETTINGSSHADOWSFINDDIALOG_H

#include <QDialog>
#include <QFile>
#include <QDebug>
namespace Ui {
class SettingsShadowsFindDialog;
}

class SettingsShadowsFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsShadowsFindDialog(QWidget *parent = nullptr);
    ~SettingsShadowsFindDialog();
    QList<double>& getShadowFindSettings();
    void updateSettingsStruct();
    void fillFileads();
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SettingsShadowsFindDialog *ui;
    QFile *file;
    QString filename = "ShadowSettings.txt";
    QList<double> paramsDouble{0};
    void defaultToFile();
    void writeToFile();

signals:
    void settingsChanged();

};



#endif // SETTINGSSHADOWSFINDDIALOG_H
