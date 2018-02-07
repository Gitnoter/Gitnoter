#include "mainwindow.h"
#include "globals.h"
#include "tools.h"
#include "version.h"

#include <QBreakpadHandler.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName(VER_COMPANYNAME_STR);
    QApplication::setOrganizationDomain(VER_ORGANIZATIONDOMAIN_STR);
    QApplication::setApplicationName(VER_PRODUCTNAME_STR);
    QApplication::setApplicationVersion(VER_PRODUCTVERSION_STR);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    qApp->setStyleSheet(Tools::readerFile(":/theme/windows-default.qss"));
#endif

#if defined(Q_OS_MACOS)
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    qApp->setStyleSheet(qApp->styleSheet() + Tools::readerFileString(":/theme/default.qss"));
    QDir::setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

#if defined(QT_NO_DEBUG)
    QBreakpadInstance.setDumpPath(__CRASHES_PATH__);
#endif

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
