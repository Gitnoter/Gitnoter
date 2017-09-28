#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "mainwindow.h"

#include "globals.h"

#include <QToolButton>
#include <QDebug>

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

        Global::gitManager->setUserPass(Global::configModel->getRepoUsername().toLocal8Bit().data(),
                                        Global::configModel->getRepoPassword().toLocal8Bit().data());
        int result = Global::gitManager->clone(Global::configModel->getRepoUrl().toLocal8Bit().data(),
                                               Global::repoPath.toLocal8Bit().data());
        if (!result) {
            Global::configModel->setIsLocalRepo(0);
            MainWindow *m = new MainWindow();
            m->show();
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
        ui->pushButton_enter->setStyleSheet("QPushButton#pushButton_enter{border-image: url(:/images/icon-enter-up.png);}");
        return true;
    }
    else {
        ui->pushButton_enter->setStyleSheet("QPushButton#pushButton_enter{border-image: url(:/images/icon-enter-down.png);}");
        return false;
    }
}
