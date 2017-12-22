#include "enterlicensedialog.h"
#include "ui_enterlicensedialog.h"
#include "tools.h"
#include "globals.h"
#include "messagedialog.h"
#include "mainwindow.h"

#include <QCryptographicHash>
#include <QTextStream>

EnterLicenseDialog::EnterLicenseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterLicenseDialog),
    mLicense(false)
{
    ui->setupUi(this);
    ui->label_payInfo->setText(ui->label_payInfo->text().append(gPurchaseLicenseUrl));
}

EnterLicenseDialog::~EnterLicenseDialog()
{
    delete ui;
}

void EnterLicenseDialog::on_buttonBox_accepted()
{
    const QString license = ui->plainTextEdit->toPlainText();

    if (license.isEmpty()) {
        return;
    }

    if (checkLicense(license, true)) {
        (new MessageDialog)->openMessage(tr("如有任何疑问, 请随时联系: %1").arg(gEmailToUser), tr("感谢您的购买和支持 (｡･ω･｡)"));
    }
    else {
        (new MessageDialog)->openMessage(
                tr("请您检查是否输入正确, 包括许可证的开始和结束行哦~\n如确认无误仍然无法激活, 请及时联系: %1").arg(gEmailToUser),
                tr("该许可证似乎无效 ╮(￣▽￣)╭"));
    }
}

bool EnterLicenseDialog::checkLicense(QString license, bool save)
{
    mLicense = false;

    QString uid, email, licenseKeyPart1, licenseKeyPart2 = "";
    QTextStream in(&license);
    QStringList stringList = {};

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }
        stringList.append(line);
    }

    if (stringList.length() != 12) {
        return false;
    }

    for (int i = 0; i < stringList.length(); ++i) {
        if (0 == i) {
            if ("—– BEGIN LICENSE —–" != stringList[i]) {
                return false;
            }
            continue;
        }
        else if (11 == i) {
            if (stringList[i] != "—— END LICENSE ——") {
                return false;
            }
            continue;
        }
        else if (1 == i) {
            email = stringList[i];
        }
        else if (2 == i) {
            if (!stringList[i].startsWith("UID-")) {
                return false;
            }

            uid = stringList[i].mid(4);
        }
        else if (3 <= i && i <= 6) {
            licenseKeyPart1 += stringList[i].replace(" ", "");
        }
        else if (7 <= i && i <= 10) {
            licenseKeyPart2 += stringList[i].replace(" ", "");
        }
    }

    const QString salt = "FsUr*5GDs9u#o0@Zvace5WAyAmVGX09u";
    const QString productType = QSysInfo::productType();
    const QString sha512Part1 = QString(QCryptographicHash::hash(QString(email + salt + uid + salt + productType).toUtf8(), QCryptographicHash::Sha512).toHex()).toUpper();
    const QString sha512Part2 = QString(QCryptographicHash::hash(QString(uid + salt + email + salt + productType).toUtf8(), QCryptographicHash::Sha512).toHex()).toUpper();

    if (sha512Part1 != licenseKeyPart1 || sha512Part2 != licenseKeyPart2) {
        return false;
    }

    if (save) {
        Tools::writerFile(gAppLicensePath, Tools::encrypt(license).toUtf8());
    }

    mLicense = true;

    return true;
}

void EnterLicenseDialog::init()
{
    QString license = Tools::readerFile(gAppLicensePath);
    mLicense = license.isEmpty() ? false : checkLicense(Tools::decrypt(license));
    ((MainWindow *) parentWidget())->menuBar()->initLicenseAction(mLicense);
}
