#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QShowEvent>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

protected:
    void showEvent(QShowEvent *showEvent) override;

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
