#include "mainwindow.h"
#include "loginwidget.h"
#include "globals.h"
#include "tools.h"
#include "menubar.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gConfigModel->init();

    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
    qApp->setStyleSheet(Tools::readerFile(":/theme/default.qss"));

    QDir::setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    MenuBar *menuBar = new MenuBar();

    if (gConfigModel->getLocalRepoStatus() > Gitnoter::NoneRepo) {
        MainWindow *w = new MainWindow(menuBar);
        w->show();
    }
    else {
        LoginWidget *l = new LoginWidget(menuBar);
        l->show();
    }

    return a.exec();
}
