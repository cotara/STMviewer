#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include <QObject>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

class HistorySettings : public QGroupBox
{
    Q_OBJECT

public:
    HistorySettings(QWidget *parent);

    QVBoxLayout *layout;
    QCheckBox *autoSaveShotCheckBox;
    QComboBox *shotsComboBox;
    QPushButton *clearButton;

signals:
    void autoSaveShotCheked(int);
    void shotSelected(int);
    void clearButtonClicked(bool);

};

#endif // HISTORYSETTINGS_H
