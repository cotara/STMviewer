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
    void setDownloadGif(bool downloading);

private:
    QStatusBar *m_statusBar;
    QLabel *dataReadyLabel;
    QLabel *mcuLabel;
    QLabel *infoLabel;
    QMovie *downloadGif;
    QLabel *downloadLabel;

    const QString lightgreen = "QLabel { background-color : lightgreen; }";
    const QString yellow = "QLabel { background-color : yellow; }";
    const QString red = "QLabel { background-color : red; color : white }";
};

#endif // STATUSBAR_H
