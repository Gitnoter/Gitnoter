#include "settingdialog.h"
#include "ui_settingdialog.h"

#include <QPropertyAnimation>
#include <QDebug>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    setWindowHeight(0);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::setWindowHeight(int pageIndex)
{
    if (ui->stackedWidget->currentIndex() == pageIndex) {
        return;
    }

    // todo: 切换page时动态修改当前页面的高度, 直接修改 this 会出现闪烁的情况
//    if (contentGeometry.isEmpty()) {
//        contentGeometry = ui->widget_content->geometry();
//    }

    ui->stackedWidget->setCurrentIndex(pageIndex);
    int _height = 0;
    switch (pageIndex) {
        case 0:_height = 260;break;
        case 1:_height = 320;break;
        case 2:_height = 240;break;
        case 3:_height = 280;break;
        case 4:_height = 500;break;
        default:return;
    }

    setFixedHeight(_height);

//    QPropertyAnimation *animation = new QPropertyAnimation(ui->widget_content, "geometry");
//    QRect endRect = QRect(contentGeometry.x(), contentGeometry.y(), contentGeometry.width(), _height);
//    animation->setDuration(200);
//    animation->setStartValue(contentGeometry);
//    animation->setEndValue(endRect);
//    animation->setEasingCurve(QEasingCurve::Linear);
//    animation->start(QAbstractAnimation::DeleteWhenStopped);

//    contentGeometry = endRect;
}

void SettingDialog::on_pushButton_general_clicked()
{
    setWindowHeight(0);
    qDebug() << 0;
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

void SettingDialog::on_lineEdit_repoUrl_editingFinished()
{

}

void SettingDialog::on_lineEdit_repoUrl_returnPressed()
{
    on_lineEdit_repoUrl_editingFinished();
}

void SettingDialog::on_fontComboBox_currentFontChanged(const QFont &f)
{

}
