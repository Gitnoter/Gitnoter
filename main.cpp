#include "mainwindow.h"
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
    MainWindow *w = new MainWindow(menuBar);

    if (gConfigModel->getHasLockWindow()) {
        w->lockDialog()->show();
    }
    else {
        w->show();
    }

    return a.exec();
}
