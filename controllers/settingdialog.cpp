#include "ui_settingdialog.h"

#include "settingdialog.h"
#include "globals.h"
#include "tools.h"

SettingDialog::SettingDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    setWindowHeight(0);

    ui->comboBox_autoSynch->setCurrentIndex(getComboBoxIndexByTime(gConfigModel->getAutoSyncRepoTime()));
    ui->comboBox_autoLock->setCurrentIndex(getComboBoxIndexByTime(gConfigModel->getAutoLockTime()));
    ui->lineEdit_unlockPassword->setText(gConfigModel->getUnlockPassword());
    ui->lineEdit_repoUrl->setText(gConfigModel->getRepoUrl());
    ui->lineEdit_username->setText(gConfigModel->getRepoUsername());
    ui->lineEdit_password->setText(gConfigModel->getRepoPassword());
//    ui->fontComboBox->setCurrentText(configModel->getEditorFont());
//    ui->comboBox_fontSize->setCurrentText(QString::number(configModel->getFontPixelSize()));

    // default key sequence
    ui->keySequenceWidget_cutRect->setClearButtonIcon(QIcon(":/images/icon-delete.png"));
    ui->keySequenceWidget_cutRect->setClearButtonStyleSheet("border:none;");
    ui->keySequenceWidget_cutRect->setDefaultKeySequence(gCutRectKeySequence);
    ui->keySequenceWidget_cutRect->setKeySequence(QKeySequence(gConfigModel->getCutRectKeySequence()));
    connect(ui->keySequenceWidget_cutRect, SIGNAL(keySequenceAccepted(const QKeySequence &)),
            this, SLOT(onCutRectKeySequenceAccepted(const QKeySequence &)));
    connect(ui->keySequenceWidget_cutRect, SIGNAL(keySequenceCleared()),
            this, SLOT(onCutRectKeySequenceCleared()));

    ui->keySequenceWidget_cutFull->setClearButtonIcon(QIcon(":/images/icon-delete.png"));
    ui->keySequenceWidget_cutFull->setClearButtonStyleSheet("border:none;");
    ui->keySequenceWidget_cutFull->setDefaultKeySequence(gCutFullKeySequence);
    ui->keySequenceWidget_cutFull->setKeySequence(QKeySequence(gConfigModel->getCutFullKeySequence()));
    connect(ui->keySequenceWidget_cutFull, SIGNAL(keySequenceAccepted(const QKeySequence &)),
            this, SLOT(onCutFullKeySequenceAccepted(const QKeySequence &)));
    connect(ui->keySequenceWidget_cutFull, SIGNAL(keySequenceCleared()),
            this, SLOT(onCutFullKeySequenceCleared()));

    ui->keySequenceWidget_cutWindow->setClearButtonIcon(QIcon(":/images/icon-delete.png"));
    ui->keySequenceWidget_cutWindow->setClearButtonStyleSheet("border:none;");
    ui->keySequenceWidget_cutWindow->setDefaultKeySequence(gCutWindowKeySequence);
    ui->keySequenceWidget_cutWindow->setKeySequence(QKeySequence(gConfigModel->getCutWindowKeySequence()));
    connect(ui->keySequenceWidget_cutWindow, SIGNAL(keySequenceAccepted(const QKeySequence &)),
            this, SLOT(onCutWindowKeySequenceAccepted(const QKeySequence &)));
    connect(ui->keySequenceWidget_cutWindow, SIGNAL(keySequenceCleared()),
            this, SLOT(onCutWindowKeySequenceCleared()));
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::setWindowHeight(int pageIndex)
{
    if (ui->stackedWidget->currentIndex() == pageIndex) {
        return;
    }

    // todo: 切换page时动态修改当前页面的高度, 直接修改 this 会出现闪烁的情况
//    if (contentGeometry.isEmpty()) {
//        contentGeometry = ui->widget_content->geometry();
//    }

    ui->stackedWidget->setCurrentIndex(pageIndex);
    int _height = 0;
    switch (pageIndex) {
        case 0:_height = 260;break;
        case 1:_height = 320;break;
        case 2:_height = 240;break;
        case 3:_height = 280;break;
        case 4:_height = 500;break;
        default:return;
    }

    setFixedHeight(_height);

//    QPropertyAnimation *animation = new QPropertyAnimation(ui->widget_content, "geometry");
//    QRect endRect = QRect(contentGeometry.x(), contentGeometry.y(), contentGeometry.width(), _height);
//    animation->setDuration(200);
//    animation->setStartValue(contentGeometry);
//    animation->setEndValue(endRect);
//    animation->setEasingCurve(QEasingCurve::Linear);
//    animation->start(QAbstractAnimation::DeleteWhenStopped);

//    contentGeometry = endRect;
}

void SettingDialog::on_pushButton_general_clicked()
{
    setWindowHeight(0);
}

void SettingDialog::on_pushButton_accountRepo_clicked()
{
    setWindowHeight(1);
}

void SettingDialog::on_pushButton_edit_clicked()
{
    setWindowHeight(2);
}

void SettingDialog::on_pushButton_shortcuts_clicked()
{
    setWindowHeight(3);
}

void SettingDialog::on_comboBox_autoSynch_currentIndexChanged(int index)
{
    int time = getTimeByComboBoxIndex(index);
    if (time >= 0) {
        gConfigModel->setAutoSyncRepoTime(time);
        emit autoSyncRepoTimeChanged();
    }
}

void SettingDialog::on_comboBox_autoLock_currentIndexChanged(int index)
{
    int time = getTimeByComboBoxIndex(index);
    if (time >= 0) {
        gConfigModel->setAutoLockTime(time);
        emit autoLockTimeChanged();
    }
}

int SettingDialog::getTimeByComboBoxIndex(int index)
{
    int time = -1;
    switch (index) {
        case 0: time = 0;break;
        case 1: time = 5;break;
        case 2: time = 15;break;
        case 3: time = 30;break;
        case 4: time = 60;break;
        default:break;
    }
    return time * 60 * 1000;
}

int SettingDialog::getComboBoxIndexByTime(int time)
{
    time = time / 60 / 1000;
    int index = 0;
    switch (time) {
        case 0: index = 0;break;
        case 5: index = 1;break;
        case 15: index = 2;break;
        case 30: index = 3;break;
        case 60: index = 4;break;
        default:break;
    }
    return index;
}


void SettingDialog::on_lineEdit_unlockPassword_editingFinished()
{
    gConfigModel->setUnlockPassword(ui->lineEdit_unlockPassword->text());
}

void SettingDialog::on_lineEdit_repoUrl_editingFinished()
{
    QRegExp regExp("((http|https)://|(www)\\.)(\\w+)(\\.?[\\.a-z0-9/:?%&=\\-_+#;]*).git", Qt::CaseInsensitive);
    if (regExp.exactMatch(ui->lineEdit_repoUrl->text())) {
        gConfigModel->setRepoUrl(ui->lineEdit_repoUrl->text());
        gGitManager->clearRemoteList();
        gGitManager->addRemote("origin", ui->lineEdit_repoUrl->text().toUtf8().constData());
    }
    else {
        ui->lineEdit_repoUrl->setText(gConfigModel->getRepoUrl());
    }
}

void SettingDialog::on_lineEdit_username_editingFinished()
{
    if (!ui->lineEdit_username->text().isEmpty()) {
        gConfigModel->setRepoUsername(ui->lineEdit_username->text());
    }
}

void SettingDialog::on_lineEdit_password_editingFinished()
{
    if (!ui->lineEdit_password->text().isEmpty()) {
        gConfigModel->setRepoPassword(ui->lineEdit_password->text());
    }
}

void SettingDialog::on_fontComboBox_currentFontChanged(const QFont &f)
{
    gConfigModel->setEditorFont(f.family());
    emit editorFontChanged();
}

void SettingDialog::on_comboBox_fontSize_currentIndexChanged(const QString &arg1)
{
//    configModel->setFontPixelSize(arg1.toInt());
    emit editorFontChanged();
}

void SettingDialog::onCutRectKeySequenceAccepted(const QKeySequence &keySequence)
{
    gConfigModel->setCutRectKeySequence(keySequence.toString());
}

void SettingDialog::onCutWindowKeySequenceAccepted(const QKeySequence &keySequence)
{
    gConfigModel->setCutWindowKeySequence(keySequence.toString());
}

void SettingDialog::onCutFullKeySequenceAccepted(const QKeySequence &keySequence)
{
    gConfigModel->setCutFullKeySequence(keySequence.toString());
}

void SettingDialog::onCutRectKeySequenceCleared()
{
    gConfigModel->setCutRectKeySequence("");
}

void SettingDialog::onCutWindowKeySequenceCleared()
{
    gConfigModel->setCutWindowKeySequence("");
}

void SettingDialog::onCutFullKeySequenceCleared()
{
    gConfigModel->setCutFullKeySequence("");
}
