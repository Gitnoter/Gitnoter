#include "mainwindow.h"
#include "loginwidget.h"
#include "globals.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Global::initConfigModel();

    if (Global::configModel->getIsLocalRepo()) {
        LoginWidget *l = new LoginWidget;
        l->show();
    }
    else {
        MainWindow *w = new MainWindow;
        w->show();
    }

    return a.exec();
}
