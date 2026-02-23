#ifndef ERRORMARKER_H
#define ERRORMARKER_H

#include <QWidget>

namespace Ui {
class ErrorMarker;
}

class ErrorMarker : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorMarker(QWidget *parent = nullptr, const QString& title="");
    ~ErrorMarker();

public slots:
    void setError(int err);

private:
    Ui::ErrorMarker *ui;
    QPixmap pixDouble,pixNone,pixOk;

};

#endif // ERRORMARKER_H
