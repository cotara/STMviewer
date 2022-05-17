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
    AutoFindWizard *wizard;
    void updateSettingsStructSlot(const QVector<double> &par);
    void filLabels(QVector<double> &par);
    int ldmModel = 20;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void fillFields();
    void fillStruct();
    void slotShortcutF5();

private:
    Ui::SettingsShadowsFindDialog *ui;
    QVector<double> geomParams;
    const QVector<double> ldm20Params = {5320,5320,207400,207400,73400,73400};
    const QVector<double> ldm50Params = {3750,3750,283500,283500,56400,56400};

    QShortcut       *keyF5;    // объект горячей клавиши F11
signals:
    void settingsChanged();
    void sendSettingsToMK();

};



#endif // SETTINGSSHADOWSFINDDIALOG_H
