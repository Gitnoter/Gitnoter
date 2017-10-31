#include "ui_loginwidget.h"

#include "loginwidget.h"
#include "mainwindow.h"
#include "tools.h"
#include "globals.h"

LoginWidget::LoginWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);    //去掉窗口边框
    setAttribute(Qt::WA_TranslucentBackground, true);   // 背景透明
    ui->lineEdit_username->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_password->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->lineEdit_repoUrl->setAttribute(Qt::WA_MacShowFocusRect, 0);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_pushButton_close_clicked()
{
    close();
}

void LoginWidget::on_lineEdit_repoUrl_textChanged(const QString &)
{
    changeEnterButtonStatus();
}

void LoginWidget::on_lineEdit_username_textChanged(const QString &)
{
    changeEnterButtonStatus();
}

void LoginWidget::on_lineEdit_password_textChanged(const QString &)
{
    changeEnterButtonStatus();
}

void LoginWidget::on_pushButton_enter_clicked()
{
    if (changeEnterButtonStatus()) {
        Global::configModel->setRepoUrl(ui->lineEdit_repoUrl->displayText());
        Global::configModel->setRepoUsername(ui->lineEdit_username->displayText());
        Global::configModel->setRepoPassword(ui->lineEdit_password->text());

        int result = initGitManager();
        if (!result) {
            Global::configModel->setLocalRepoStatus(1);
            openMainWindow();
        }
        else {
            qDebug() << "void LoginWidget::on_pushButton_enter_clicked(): " << result;
        }
    }
}

bool LoginWidget::changeEnterButtonStatus()
{
    if (!ui->lineEdit_password->displayText().isEmpty()
        && !ui->lineEdit_repoUrl->displayText().isEmpty()
        && !ui->lineEdit_username->displayText().isEmpty()) {
        ui->pushButton_enter->setStyleSheet(
                "QPushButton#pushButton_enter{border-image: url(:/images/icon-enter-up.png);}");
        return true;
    }
    else {
        ui->pushButton_enter->setStyleSheet(
                "QPushButton#pushButton_enter{border-image: url(:/images/icon-enter-down.png);}");
        return false;
    }
}

void LoginWidget::openMainWindow()
{
    Tools::createMkDir(Global::repoNoteTextPath);
    Tools::createMkDir(Global::repoNoteDataPath);

    close();
    (new MainWindow)->show();
}

void LoginWidget::on_pushButton_initLocal_clicked()
{
    int result = initGitManager();
    if (!result) {
        Global::configModel->setLocalRepoStatus(2);
        openMainWindow();
    }
    else {
        qDebug() << "void LoginWidget::on_pushButton_initLocal_clicked(): " << result;
    }
}

int LoginWidget::initGitManager()
{
    if (QDir(Global::repoPath).exists()) {
        return Global::gitManager->open(Global::repoPath.toUtf8().constData());
    }

    if (!Global::configModel->getRepoUsername().isEmpty()
        && !Global::configModel->getRepoPassword().isEmpty()
        && !Global::configModel->getRepoUrl().isEmpty()) {
        Global::gitManager->setUserPass(Global::configModel->getRepoUsername().toUtf8().constData(),
                                        Global::configModel->getRepoPassword().toUtf8().constData());
        return Global::gitManager->clone(Global::configModel->getRepoUrl().toUtf8().constData(), Global::repoPath.toUtf8().constData());
    }

    return Global::gitManager->initLocalRepo(Global::repoPath.toUtf8().constData(), true);
}
