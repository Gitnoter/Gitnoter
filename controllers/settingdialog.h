#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QFont>

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

private:
    void setupUi();

    void setWindowHeight(int pageIndex);

    int getTimeByComboBoxIndex(int index);

    int getComboBoxIndexByTime(int time);

signals:

    void autoSyncRepoTimeChanged();

    void autoLockTimeChanged();

    void editorFontChanged(const QFont &font);

    void globalHotKeysChanged();

private slots:

    void on_pushButton_general_clicked();

    void on_pushButton_accountRepo_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_shortcuts_clicked();

    void on_lineEdit_repoUrl_editingFinished();

    void on_comboBox_autoSynch_currentIndexChanged(int index);

    void on_comboBox_autoLock_currentIndexChanged(int index);

    void on_lineEdit_unlockPassword_editingFinished();

    void on_lineEdit_username_editingFinished();

    void on_lineEdit_password_editingFinished();

    void onCutRectKeySequenceAccepted(const QKeySequence &keySequence);
    void onCutWindowKeySequenceAccepted(const QKeySequence &keySequence);
    void onCutFullKeySequenceAccepted(const QKeySequence &keySequence);

    void onCutRectKeySequenceCleared();
    void onCutWindowKeySequenceCleared();
    void onCutFullKeySequenceCleared();

    void on_pushButton_font_clicked();

private:
    Ui::SettingDialog *ui;
    QSize mContentGeometry;

    QList<int> mHeight;

};

#endif // SETTINGDIALOG_H
