#include "settingdialog.h"
#include "ui_settingdialog.h"

#include <QPropertyAnimation>
#include <QDebug>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::setWindowHeight(int pageIndex)
{
    qDebug() << pageIndex;
    if (ui->stackedWidget->currentIndex() == pageIndex) {
        return;
    }

    if (contentGeometry.isEmpty()) {
        contentGeometry = ui->widget_content->geometry();
    }

    ui->stackedWidget->setCurrentIndex(pageIndex);
    int _height = 0;
    switch (pageIndex) {
        case 0:_height = 100;break;
        case 1:_height = 200;break;
        case 2:_height = 300;break;
        case 3:_height = 400;break;
        case 4:_height = 500;break;
        default:return;
    }

    QPropertyAnimation *animation = new QPropertyAnimation(ui->widget_content, "geometry");
    QRect endRect = QRect(contentGeometry.x(), contentGeometry.y(), contentGeometry.width(), _height);
    animation->setDuration(200);
    animation->setStartValue(contentGeometry);
    animation->setEndValue(endRect);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    contentGeometry = endRect;
}

void SettingDialog::on_pushButton_general_clicked()
{
    setWindowHeight(0);
}

void SettingDialog::on_pushButton_accountRepo_clicked()
{
    setWindowHeight(1);
}

void SettingDialog::on_pushButton_edit_clicked()
{
    setWindowHeight(2);
}

void SettingDialog::on_pushButton_shortcuts_clicked()
{
    setWindowHeight(3);
}

void SettingDialog::on_pushButton_snippets_clicked()
{
    setWindowHeight(4);
}
