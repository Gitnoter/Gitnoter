#ifndef ENTERLICENSEDIALOG_H
#define ENTERLICENSEDIALOG_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class EnterLicenseDialog;
}

class EnterLicenseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnterLicenseDialog(QWidget *parent = 0);
    ~EnterLicenseDialog();

    void init();

protected:
    void showEvent(QShowEvent *showEvent) override;

private:
    bool checkLicense(QString license, bool save = false);

private slots:
    void on_buttonBox_accepted();

public:
    bool isLicense() { return mLicense; }

private:
    Ui::EnterLicenseDialog *ui;
    bool mLicense;
    MainWindow *mMainWindow;

};

#endif // ENTERLICENSEDIALOG_H
