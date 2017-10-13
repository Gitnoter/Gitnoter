#ifndef LOCKDIALOG_H
#define LOCKDIALOG_H

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

private:
    Ui::LockDialog *ui;
};

#endif // LOCKDIALOG_H
