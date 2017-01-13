#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include "qgit2.h"

using namespace LibQGit2;

/**
 * Mac 上 libgit2 可以clone仓库
 * windows 上的问题需要单独处理
 * 基本可以确定是 windows 和 libgit2 兼容的问题
 * @brief do_git_clone
 * @param repo_directory
 * @param url
 */
void do_git_clone(const char* repo_directory, const char* url) {
  git_repository* repo;

  git_clone_options opts = GIT_CLONE_OPTIONS_INIT;

  int rtn = git_clone(&repo, url, repo_directory, &opts);
  if (rtn == 0) {
    qDebug() <<  ("clone success\n");
    git_repository_free(repo);
  } else {
    const git_error* err = giterr_last();
    qDebug() << err->klass;
    qDebug() << err->message;
  }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    do_git_clone("/Users/MakeHui/Desktop/test3", "http://git.famishare.cn/MakeHui/test.git");

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
