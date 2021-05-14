#ifndef STATUSBAR_H
#define STATUSBAR_H
#include <QString>
#include <QObject>
#include <QMovie>

class QStatusBar;
class QLabel;
class QProgressBar;

class StatusBar {

public:
    StatusBar(QStatusBar *statusBar);
    void setStatus(bool online);
    void setDataReady(int ready);
    void setInfo(int info);
    void setDownloadBarValue(int value);
    void incDownloadBarValue(int value);
    void setDownloadBarRange(int value);
    int getDownloadBarRange();
    int getDownloadBarValue();
    void incReSent();
    void clearReSent();
    void setMessageBar(QString status);

private:
    QStatusBar *m_statusBar;
    QLabel *dataReadyLabel;
    QLabel *mcuLabel;
    QLabel *infoLabel;
    QProgressBar *downloadBar;
    QLabel *reSent;

    int reSentCount=0;

    const QString lightgreen = "QLabel { background-color : lightgreen; }";
    const QString yellow = "QLabel { background-color : yellow; }";
    const QString red = "QLabel { background-color : red; color : white }";
};

#endif // STATUSBAR_H
