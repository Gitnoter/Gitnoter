#ifndef ENTERLICENSEDIALOG_H
#define ENTERLICENSEDIALOG_H

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

private:
    bool checkLicense(QString license, bool save = false);

private slots:
    void on_buttonBox_accepted();

public:
    bool isLicense() { return mLicense; }

private:
    Ui::EnterLicenseDialog *ui;
    bool mLicense;

};

#endif // ENTERLICENSEDIALOG_H
