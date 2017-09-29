#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = 0);
    ~LoginWidget();

private slots:
    void on_pushButton_close_clicked();

    void on_lineEdit_password_textChanged(const QString &arg1);

    void on_lineEdit_repoUrl_textChanged(const QString &arg1);

    void on_lineEdit_username_textChanged(const QString &arg1);

    void on_pushButton_enter_clicked();

    void on_pushButton_initLocal_clicked();

private:
    Ui::LoginWidget *ui;

    bool changeEnterButtonStatus();

    void openMainWindow();
};

#endif // LOGINWIDGET_H
