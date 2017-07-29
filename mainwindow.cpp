#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QDebug>

#include "gitmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    GitManager *gitManager = new GitManager();
    const char *url = "https://git.oschina.net/TerryZ/bTabs.git";
    const char *repo_directory = "/Users/apple/Desktop/test3";

    const char *purl = "https://git.oschina.net/make/libgit2-test.git";
    const char *prepo_directory = "/Users/apple/Desktop/test2";

    char *username = "makehuir@gmail.com";
    char *password = "oschinaMail003";

    QString docFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    docFolder = QDir(docFolder).filePath("test3");

    QByteArray array = docFolder.toLocal8Bit();
    char* buffer = array.data();

    qDebug() << buffer;
    GitManager *gitManager = new GitManager(username, password);
    gitManager->clone(purl, buffer);



    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
