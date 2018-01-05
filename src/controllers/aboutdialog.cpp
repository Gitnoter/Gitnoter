#include "ui_aboutdialog.h"

#include "aboutdialog.h"
#include "version.h"
#include "tools.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->name->setText(VER_PRODUCTNAME_STR);
    ui->version->setText(ui->version->text() + VER_PRODUCTVERSION_STR);
    ui->build->setText(ui->build->text() + QString::number(VER_PRODUCTBUILD_STR));
    ui->copyright->setText(QString(VER_LEGALCOPYRIGHT_STR) + VER_LEGALTRADEMARKS1_STR);
    ui->textBrowser->setHtml(Tools::readerFileString(":/html/appinfo/about.html"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
