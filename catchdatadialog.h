#ifndef CATCHDATADIALOG_H
#define CATCHDATADIALOG_H

#include <QDialog>


namespace Ui {
class catchDataDialog;
}

class catchDataDialog : public QWidget
{
    Q_OBJECT

public:
    explicit catchDataDialog(QWidget *parent = nullptr);
    ~catchDataDialog();

    void setButtonPushed(QVector <double> data, int i);
    void clear();

private:
    Ui::catchDataDialog *ui;
    QVector <bool> buttonStatus{false,false,false,false,false,false,false,false,false};
    bool mainStatus=false;
    bool checkStatus();
signals:
    void buttonClicked(int i);
    void dataCatched();
};

#endif // CATCHDATADIALOG_H
