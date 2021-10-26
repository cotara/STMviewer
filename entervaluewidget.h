#ifndef ENTERVALUEWIDGET_H
#define ENTERVALUEWIDGET_H

#include <QDialog>

namespace Ui {
class EnterValueWidget;
}

class EnterValueWidget : public QDialog
{
    Q_OBJECT

public:
    explicit EnterValueWidget(QWidget *parent = nullptr, int min=0, int max=255);
    ~EnterValueWidget();

private:
    Ui::EnterValueWidget *ui;
    int oldValue=0;

signals:
    void sendValue(int);
    void valueChanged();
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // ENTERVALUEWIDGET_H
