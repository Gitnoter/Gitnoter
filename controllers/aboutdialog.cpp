#include "ui_aboutdialog.h"

#include "aboutdialog.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->name->setText(VER_PRODUCTNAME_STR);
    ui->version->setText(ui->version->text() + VER_PRODUCTVERSION_STR);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
