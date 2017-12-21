#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDialog(QWidget *parent = 0);
    ~MessageDialog();

    void openMessage(const QString &body, const QString &title = "",
                     const QString &apply = "", const QString &close = "");

public:
    static MessageDialog *openMessage( QWidget *parent, const QString &body, const QString &title = "",
                            const QString &apply = "", const QString &close = "");

signals:
    void applyClicked();

    void closeClicked();

private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_apply_clicked();

private:
    Ui::MessageDialog *ui;

};

#endif // MESSAGEDIALOG_H
