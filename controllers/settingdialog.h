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

private slots:

    void on_pushButton_general_clicked();

    void on_pushButton_accountRepo_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_shortcuts_clicked();

    void on_lineEdit_repoUrl_editingFinished();

    void on_lineEdit_repoUrl_returnPressed();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_comboBox_autoSynch_currentIndexChanged(int index);

    void on_comboBox_autoLock_currentIndexChanged(int index);

    void on_lineEdit_unlockPassword_editingFinished();

private:
    Ui::SettingDialog *ui;
    QRect contentGeometry;

    void setWindowHeight(int pageIndex);

    int getTimeByComboBoxIndex(int index);

    int getComboBoxIndexByTime(int time);

};

#endif // SETTINGDIALOG_H
