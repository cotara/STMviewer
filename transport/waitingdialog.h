#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    ~WaitingDialog();
    void waitingStart(int seconds);

private:
    Ui::WaitingDialog *ui;
    QTimer m_timer;

private slots:
    void waitingStop();

};

#endif // WAITINGDIALOG_H
