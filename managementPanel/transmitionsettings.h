#ifndef TRANSMITIONSETTINGS_H
#define TRANSMITIONSETTINGS_H

#include <QObject>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
class TransmitionSettings : public QGroupBox
{
    Q_OBJECT

public:
    TransmitionSettings(QWidget *parent);

public slots:
    void setChEn(bool en);
    void setGetButton(bool en);
    bool getStatusGetButton();
private:
    QVBoxLayout *layout;
    QHBoxLayout *shift1Layout,*shift2Layout;
    QList<QCheckBox*>chCheckBox;

    QSpinBox *shift1Spinbox, *shift2Spinbox;
    QPushButton *getButton;
    QLabel *shift1Label, *shift2Label;

    int order=0;

signals:
    void chChooseChanged(int);
    void getButtonClicked(bool);
    void shiftChanged(int ch, int val);

};

#endif // TRANSMITIONSETTINGS_H
