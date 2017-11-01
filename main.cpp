#include "mainwindow.h"
#include "loginwidget.h"
#include "globals.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Globals::configModel->init();

    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

    if (Globals::configModel->getLocalRepoStatus()) {
        MainWindow *w = new MainWindow;
        w->show();
    }
    else {
        LoginWidget *l = new LoginWidget;
        l->show();
    }

    return a.exec();
}
