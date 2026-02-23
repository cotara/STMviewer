#ifndef HISTORYSETTINGS_H
#define HISTORYSETTINGS_H

#include <QObject>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include "collapsiblegroupbox.h"

class HistorySettings : public CollapsibleGroupBox
{
    Q_OBJECT

public:
    HistorySettings(QWidget *parent);

public slots:
    void addShot(int val);
    int curShot();

private:
    QVBoxLayout *layout;
    QHBoxLayout *butLayout;
    QPushButton *saveButton, *loadButton,*clearButton;
    QComboBox *shotsComboBox;

private slots:
    void clear();

signals:
    void saveHistory();
    void loadHistory();
    void clearHistory();
    void shotSelected(int);


};

#endif // HISTORYSETTINGS_H
