#include "mainwindow.h"
#include "globals.h"
#include "tools.h"
#include "menubar.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    qApp->setStyleSheet(Tools::readerFile(":/theme/windows-default.qss"));
#endif

#if defined(Q_OS_MACOS)
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    qApp->setStyleSheet(qApp->styleSheet() + Tools::readerFileString(":/theme/default.qss"));
    QDir::setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    gConfigModel->init();

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
