#include "mainwindow.h"
#include "appconfig.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    AppConfig::init(appConfigLocation);

    MainWindow w;
    w.show();

    return a.exec();
}
