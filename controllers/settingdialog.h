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

signals:

    void autoSyncRepoTimeChanged();

    void autoLockTimeChanged();

    void editorFontChanged();

    void globalHotKeysChanged();

private slots:

    void on_pushButton_general_clicked();

    void on_pushButton_accountRepo_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_shortcuts_clicked();

    void on_lineEdit_repoUrl_editingFinished();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_comboBox_autoSynch_currentIndexChanged(int index);

    void on_comboBox_autoLock_currentIndexChanged(int index);

    void on_lineEdit_unlockPassword_editingFinished();

    void on_lineEdit_username_editingFinished();

    void on_lineEdit_password_editingFinished();
    
    void on_comboBox_fontSize_currentIndexChanged(const QString &arg1);

    void onCutRectKeySequenceAccepted(const QKeySequence &keySequence);
    void onCutWindowKeySequenceAccepted(const QKeySequence &keySequence);
    void onCutFullKeySequenceAccepted(const QKeySequence &keySequence);

    void onCutRectKeySequenceCleared();
    void onCutWindowKeySequenceCleared();
    void onCutFullKeySequenceCleared();

private:
    Ui::SettingDialog *ui;
    QRect contentGeometry;

    void setWindowHeight(int pageIndex);

    int getTimeByComboBoxIndex(int index);

    int getComboBoxIndexByTime(int time);

};

#endif // SETTINGDIALOG_H
