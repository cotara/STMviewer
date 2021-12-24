#ifndef SETTINGSSHADOWSFINDDIALOG_H
#define SETTINGSSHADOWSFINDDIALOG_H

#include <QDialog>
#include <QFile>
#include <QDebug>
#include "autofindwizard.h"
#include <QShortcut>

namespace Ui {
class SettingsShadowsFindDialog;
}

class SettingsShadowsFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsShadowsFindDialog(QWidget *parent = nullptr);
    ~SettingsShadowsFindDialog();
    QVector<double>& getShadowFindSettings();
    void updateSettingsStruct();
    void fillFileads();
    AutoFindWizard *wizard;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void updateSettingsStructSlot(QVector<double> &par);
    void slotShortcutF5();

private:
    Ui::SettingsShadowsFindDialog *ui;
    QFile *file;
    QString filename = "ShadowSettings.txt";
    QVector<double> paramsDouble;
    void defaultToFile();
    void writeToFile();
    QByteArray readParamsFromFile();
    QVector<double> defaultSettings{0.905,5320,5320,207400,207400,73400,73400};
    QShortcut       *keyF5;    // объект горячей клавиши F11
signals:
    void settingsChanged();
    void sendSettingsToMK();

};



#endif // SETTINGSSHADOWSFINDDIALOG_H
