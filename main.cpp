#include "mainwindow.h"
#include "loginwidget.h"
#include "globals.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Global::initConfigModel();

    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

    if (Global::configModel->getLocalRepoStatus()) {
        MainWindow *w = new MainWindow;
        w->show();
    }
    else {
        LoginWidget *l = new LoginWidget;
        l->show();
    }

    return a.exec();
}
