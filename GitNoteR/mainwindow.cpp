#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include "qgit2.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString PrivateUrl = "http://git.famishare.cn/MakeHui/test2.git";
    QString PublicUrl = "http://git.famishare.cn/MakeHui/test.git";

    QString MacPath = "/Users/MakeHui/Desktop/test3";
    QString WindowsPath = "C:/Users/MakeHui/Desktop/test3";

    QString UserName = "2240544497@qq.com";
    QString Passphrase = "yaoHui003";

    auto repository = new LibQGit2::Repository();

//    try {
//        auto credentials = Credentials::ssh(QString(), QString(), UserName, Passphrase);
//        repository->setRemoteCredentials("origin", credentials);
//    } catch (const LibQGit2::Exception& e) {
//        qDebug() << e.message();
//    }

    try {
        repository->clone(PublicUrl, MacPath);
    } catch (const LibQGit2::Exception& e) {
        qDebug() << e.message();
    }
    qDebug() << repository->listReferences();
}

MainWindow::~MainWindow()
{
    delete ui;
}
