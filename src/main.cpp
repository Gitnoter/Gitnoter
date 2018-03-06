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

#if defined(Q_OS_WIN)
//    QApplication::setFont(QFont("Courier New"));
#endif

    QDir::setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    QDir dir;
    dir.mkdir(__AppDataLocation__);
    dir.mkdir(__AppDataPath__);
    dir.mkdir(__TempPath__);
    dir.mkdir(__CRASHES_PATH__);
    dir.mkdir(__CRASHES_BAK_PATH__);

#if defined(Q_OS_MACOS)
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    qApp->setStyleSheet(Tools::readerFile(":/theme/windows-default.qss"));
#endif

    qApp->setStyleSheet(qApp->styleSheet() + Tools::readerFileString(":/theme/default.qss"));

#if defined(QT_NO_DEBUG)
    QBreakpadInstance.setDumpPath(__CRASHES_PATH__);

    QStringList crashDumpList = QBreakpadInstance.dumpFileList();
    QString crashBase64String = "";

    for (int i = 0; i < crashDumpList.length(); ++i) {
        const QString path = __CRASHES_PATH__ + "/" + crashDumpList[i];
        crashBase64String += QString(
                "------------------------------------------------------------------------------------------ crash: %1 "
                "------------------------------------------------------------------------------------------\n %2\n\n"
        ).arg(QString::number(i), Tools::fileToBase64(path));
        QDir().rename(path, __CRASHES_BAK_PATH__ + "/" + crashDumpList[i]);
    }

    if (!crashBase64String.isEmpty()) {
        QDesktopServices::openUrl(QUrl(QString("mailto:?to=%1&subject=Gitnoter for %2: Crashes&body=%3").arg(
                __EmailToUser__, QSysInfo::prettyProductName(), crashBase64String), QUrl::TolerantMode));
    }
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
