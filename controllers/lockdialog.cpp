#include "lockdialog.h"
#include "ui_lockdialog.h"

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
