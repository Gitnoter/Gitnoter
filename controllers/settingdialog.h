#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
    class SettingDialog;
}

class SettingDialog : public QDialog
{
Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);

    ~SettingDialog();

private slots:

    void on_pushButton_general_clicked();

    void on_pushButton_accountRepo_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_shortcuts_clicked();

    void on_pushButton_snippets_clicked();


private:
    Ui::SettingDialog *ui;
    QRect contentGeometry;

    void setWindowHeight(int pageIndex);
};

#endif // SETTINGDIALOG_H
