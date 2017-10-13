#include "lockdialog.h"
#include "ui_lockdialog.h"
#include "mainwindow.h"
#include "globals.h"

LockDialog::LockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LockDialog)
{
    ui->setupUi(this);
}

LockDialog::~LockDialog()
{
    delete ui;
}

void LockDialog::on_lineEdit_returnPressed()
{
    if (Global::configModel->getUnlockPassword() == ui->lineEdit->text()) {
        close();
        (new MainWindow)->show();
    }
}
