#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include "qgit2.h"

using namespace LibQGit2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString Url = "https://git.oschina.net/MakeHuiR/test.git";
    QString Url2 = "http://git.famishare.cn/MakeHui/test2.git";
    QString GitPath = "/Users/MakeHui/Desktop/test3";
    QString UserName = "makehuir@gmail.com";
    QByteArray Passphrase = "oschinaMail003";

    QByteArray UserName2 = "makehuir@gmail.com";

    auto repository = new LibQGit2::Repository();
    try {
        auto credentials = Credentials::ssh(QString(), QString(), "2240544497@qq.com", "yaoHui003");
        repository->setRemoteCredentials("origin", credentials);
    } catch (const LibQGit2::Exception& e) {
        qDebug() << e.message();
    }
    try {
        repository->clone(Url2, GitPath);
    } catch (const LibQGit2::Exception& e) {
        qDebug() << e.message();
    }
    qDebug() << repository->listReferences();
}

MainWindow::~MainWindow()
{
    delete ui;
}
