#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include "qgit2.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QString MacPath = "/Users/MakeHui/Desktop/test3";
    QString WindowsPath = "C:/Users/MakeHui/Desktop/test3";

    QString PublicUrl = "http://github.com/c9/c9.ide.ace.repl.git";
    QString PrivateUrl = "http://git.oschina.net/make/test.git";

    QByteArray UserName = "makehuir@gmail.com";
    QByteArray Passphrase = "oschinaMail003";

    auto repository = new LibQGit2::Repository();

//    try {
//        auto credentials = LibQGit2::Credentials::ssh(QString(), QString(), UserName, Passphrase);
//        repository->setRemoteCredentials("origin", credentials);
//    } catch (const LibQGit2::Exception& e) {
//        qDebug() << e.message();
//    }

    try {
        repository->clone(PublicUrl, MacPath);
    } catch (const LibQGit2::Exception& e) {
        qDebug() << e.message();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString MacPath = "/Users/MakeHui/Desktop/test3";
    QString WindowsPath = "C:/Users/MakeHui/Desktop/test3";

    QString PublicUrl = "http://git.coding.net/sciooga/Keyboard-launch.git";
    QString PrivateUrl = "http://git.oschina.net/make/test.git";
    QString PrivateUrl2 = "http://git.coding.net/MakeHui/a40.git";

    QByteArray UserName = "makehuir@gmail.com";
    QByteArray Passphrase = "oschinaMail003";

    auto repository = new LibQGit2::Repository();

    try {
        auto credentials = LibQGit2::Credentials::ssh(QString(), QString(), UserName, Passphrase);
        repository->setRemoteCredentials("origin", credentials);
    } catch (const LibQGit2::Exception& e) {
        qDebug() << e.message();
    }

    try {
        repository->clone(PrivateUrl, MacPath);
    } catch (const LibQGit2::Exception& e) {
        qDebug() << e.message();
    }
}
