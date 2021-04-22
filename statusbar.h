#ifndef STATUSBAR_H
#define STATUSBAR_H
#include <QString>
#include <QObject>

class QStatusBar;
class QLabel;

class StatusBar {

public:
    StatusBar(QStatusBar *statusBar);
    void setStatus(bool online);
    void setTime(const QDateTime &time);
    void setManual(bool manual);

private:
    QStatusBar *m_statusBar;
    QLabel *timeLabel;
    QLabel *mcuLabel;
    QLabel *modeLabel;

    const QString lightgreen = "QLabel { background-color : lightgreen; }";
    const QString yellow = "QLabel { background-color : yellow; }";
    const QString red = "QLabel { background-color : red; color : white }";
};

#endif // STATUSBAR_H
