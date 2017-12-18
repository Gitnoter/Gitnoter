#include "ui_settingdialog.h"

#include "settingdialog.h"
#include "globals.h"
#include "tools.h"

#include <QPropertyAnimation>
#include <QFontDialog>
#include <QDebug>

SettingDialog::SettingDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingDialog),
        mHeight({220, 300, 180, 240})
{
    ui->setupUi(this);
    setupUi();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::setupUi()
{
    setWindowHeight(0);
    qApp->setAttribute(Qt::AA_NativeWindows, true);

    ui->comboBox_autoSynch->setCurrentIndex(getComboBoxIndexByTime(gConfigModel->getAutoSyncRepoTime()));
    ui->comboBox_autoLock->setCurrentIndex(getComboBoxIndexByTime(gConfigModel->getAutoLockTime()));
    ui->lineEdit_unlockPassword->setText(gConfigModel->getUnlockPassword());
    ui->lineEdit_repoUrl->setText(gConfigModel->getRepoUrl());
    ui->lineEdit_username->setText(gConfigModel->getRepoUsername());
    ui->lineEdit_password->setText(gConfigModel->getRepoPassword());
    ui->label_font->setFont(gConfigModel->getEditorFont());
    ui->label_font->setText(gConfigModel->getEditorFont().toString());

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

void SettingDialog::setWindowHeight(int pageIndex)
{
    if (ui->stackedWidget->currentIndex() == pageIndex) {
        return;
    }

    int height = mHeight[pageIndex];
    ui->stackedWidget->setCurrentIndex(pageIndex);
    ui->widget_content->hide();
    ui->widget_content->setFixedHeight(height - ui->widget_buttons->height());

    QPropertyAnimation *animation = new QPropertyAnimation(this, "size");
    animation->setDuration(abs(size().height() - height));
    animation->setStartValue(size());
    animation->setEndValue(QSize(size().width(), height));
    animation->setEasingCurve(QEasingCurve::Linear);

    connect(animation, &QPropertyAnimation::finished, [=]() {
        ui->widget_content->show();
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped);
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

void SettingDialog::onCutRectKeySequenceAccepted(const QKeySequence &keySequence)
{
    gConfigModel->setCutRectKeySequence(keySequence.toString());
    emit globalHotKeysChanged();
}

void SettingDialog::onCutWindowKeySequenceAccepted(const QKeySequence &keySequence)
{
    gConfigModel->setCutWindowKeySequence(keySequence.toString());
    emit globalHotKeysChanged();
}

void SettingDialog::onCutFullKeySequenceAccepted(const QKeySequence &keySequence)
{
    gConfigModel->setCutFullKeySequence(keySequence.toString());
    emit globalHotKeysChanged();
}

void SettingDialog::onCutRectKeySequenceCleared()
{
    gConfigModel->setCutRectKeySequence("");
    emit globalHotKeysChanged();
}

void SettingDialog::onCutWindowKeySequenceCleared()
{
    gConfigModel->setCutWindowKeySequence("");
    emit globalHotKeysChanged();
}

void SettingDialog::onCutFullKeySequenceCleared()
{
    gConfigModel->setCutFullKeySequence("");
    emit globalHotKeysChanged();
}

void SettingDialog::on_pushButton_font_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->label_font->font(), this);

    if (!ok) {
        return;
    }

    ui->label_font->setFont(font);
    ui->label_font->setText(font.toString());
    gConfigModel->setEditorFont(font);

    emit editorFontChanged(font);
}
