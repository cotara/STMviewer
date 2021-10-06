#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.showMaximized();

    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/Nunito/Nunito-Light.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/GranaPadano.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/SF/SFUIText-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/SF/SFUIText-Light.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/SF/SFUIText-Regular.ttf");


    QFile file(":/qss/style.css");
    if(file.open(QFile::ReadOnly)){
          QByteArray style = file.readAll();
          a.setStyleSheet(style);
          file.close();
    }



    return a.exec();
}
