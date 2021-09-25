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
    explicit EnterValueWidget(QWidget *parent = nullptr);
    ~EnterValueWidget();

private:
    Ui::EnterValueWidget *ui;

signals:
    void sendValue(int);
private slots:
    void on_buttonBox_accepted();
};

#endif // ENTERVALUEWIDGET_H
