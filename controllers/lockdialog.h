#ifndef LOCKDIALOG_H
#define LOCKDIALOG_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class LockDialog;
}

class LockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LockDialog(QWidget *parent = 0);
    ~LockDialog();

protected:
    void showEvent(QShowEvent *showEvent) override;

signals:
    void hideActivated();

public slots:
    void showWidget();
    void hideWidget();

private slots:
    void on_lineEdit_returnPressed();

    void on_lineEdit_textChanged(const QString &arg1);


private:
    Ui::LockDialog *ui;
    MainWindow *mMainWindow;

};

#endif // LOCKDIALOG_H
