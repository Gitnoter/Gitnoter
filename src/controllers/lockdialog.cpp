#include "ui_lockdialog.h"

#include "lockdialog.h"
#include "globals.h"

LockDialog::LockDialog(QWidget *parent) :
    ui(new Ui::LockDialog),
    mMainWindow((MainWindow *) parent)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);    //去掉窗口边框
//    setAttribute(Qt::WA_TranslucentBackground, true);   // 背景透明
    ui->lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
}

LockDialog::~LockDialog()
{
    delete ui;
}

void LockDialog::on_lineEdit_returnPressed()
{
    if (gConfigModel->getUnlockPassword() == ui->lineEdit->text()) {
        hideWidget();
    }
}

void LockDialog::on_lineEdit_textChanged(const QString &)
{
    on_lineEdit_returnPressed();
}

void LockDialog::showWidget()
{
    show();

    QWidgetList widgetList = mMainWindow->menuBar()->windowMenuWidgetList();
    for (auto &&widget :widgetList) {
        widget->hide();
    }
}

void LockDialog::hideWidget()
{
    hide();

    QWidgetList widgetList = mMainWindow->menuBar()->windowMenuWidgetList();
    for (auto &&widget :widgetList) {
        widget->show();
    }

    gConfigModel->setHasLockWindow(false);

    emit hideActivated();
}

void LockDialog::showEvent(QShowEvent *showEvent)
{
    QDialog::showEvent(showEvent);

    ui->lineEdit->clear();

    ((MainWindow *) parent())->gAnalytics()->sendScreenView(objectName());
}

