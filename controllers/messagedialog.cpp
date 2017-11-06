#include "messagedialog.h"
#include "ui_messagedialog.h"

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton_apply, SIGNAL(clicked()), parent, SLOT(onNoteDeleted()));
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::openMessage(QWidget *parent, const QString &body, const QString &title,
                                const QString &apply, const QString &close)
{
    connect(ui->pushButton_apply, SIGNAL(clicked()), parent, SLOT(onNoteDeleted()));
    setParent(parent);
    openMessage(body, title, apply, close);
}

void MessageDialog::openMessage(const QString &body, const QString &title, const QString &apply, const QString &close)
{
    ui->label_body->setText(body);

    if (!title.isEmpty()) {
        ui->label_title->setText(title);
    }

    if (!apply.isEmpty()) {
        ui->pushButton_apply->setText(apply);
    }

    if (!close.isEmpty()) {
        ui->pushButton_close->setText(close);
    }

    open();
}

void MessageDialog::on_pushButton_close_clicked()
{

}

void MessageDialog::on_pushButton_apply_clicked()
{

}
