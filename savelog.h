#ifndef SAVELOG_H
#define SAVELOG_H

#include <QDialog>

namespace Ui {
class SaveLog;
}

class SaveLog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveLog(QWidget *parent = nullptr);
    ~SaveLog();

private slots:
    void on_toolButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::SaveLog *ui;
    QString dirname;

signals:
    void SaveToFolder(QString &dirname);
};

#endif // SAVELOG_H
