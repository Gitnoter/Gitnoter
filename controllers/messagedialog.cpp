#include "messagedialog.h"
#include "ui_messagedialog.h"

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

MessageDialog *MessageDialog::openMessage(QWidget *parent, const QString &body, const QString &title, const QString &apply, const QString &close)
{
    MessageDialog *messageDialog = new MessageDialog(parent);
    messageDialog->openMessage(body, title, apply, close);

    return messageDialog;
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
    emit closeClicked();
}

void MessageDialog::on_pushButton_apply_clicked()
{
    emit applyClicked();
}
