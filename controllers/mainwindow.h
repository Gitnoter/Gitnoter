#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "categorieswidget.h"
#include "notemodel.h"
#include "configmodel.h"
#include "tagswidget.h"
#include "settingdialog.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

public slots:

    void onChangeCategories();

    void onChangeTags();

private slots:

    // 系统菜单栏
    void on_action_saveNote_triggered();

    void on_action_newNote_triggered();

    void on_action_deleteNote_triggered();

    void openAboutDialog();

    void openSettingDialog();

    // 左侧菜单栏
    void on_pushButton_notes_clicked();

    void on_pushButton_folder_clicked();

    void on_pushButton_tags_clicked();

    void on_pushButton_trash_clicked();

    void on_pushButton_sync_clicked();

    // 菜单分页第一页
    void on_tableWidget_list_doubleClicked(const QModelIndex &index);

    void on_tableWidget_list_itemClicked(QTableWidgetItem *item);

    void onHeaderViewSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    void on_pushButton_deleteNote_clicked();

    void on_pushButton_categories_clicked();

    void on_pushButton_changeTags_clicked();

    // 菜单分页第二页
    void on_listWidget_categories_itemClicked(QListWidgetItem *item);

    void on_listWidget_categories_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_categories_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_addCategories_clicked();

    void on_pushButton_removeCategories_clicked();

    void onActionRenameCategoriesTriggered();

    void onActionNameSortTriggered();

    void onActionCountSortTriggered();

    void onActionTimeSortTriggered();

    void onLineEditNameCategoriesEditingFinished();

    void on_lineEdit_searchCategories_textChanged(const QString &arg1);

    // 菜单分页第三页
    void on_listWidget_tags_itemClicked(QListWidgetItem *item);

    void on_listWidget_tags_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_tags_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_addTags_clicked();

    void on_pushButton_removeTags_clicked();

    void onActionRenameTagsTriggered();
//    void onActionNameSortTagsTriggered();
//    void onActionCountSortTagsTriggered();
//    void onActionTimeSortTagsTriggered();

    void onLineEditNameTagsEditingFinished();

    void on_lineEdit_searchTags_textChanged(const QString &arg1);

    void on_lineEdit_searchNoteList_textChanged(const QString &arg1);

    // 菜单分页第四页

    // 菜单分页第五页

signals:

    void resizeChildWindow(QSize size);

protected:
    virtual void resizeEvent(QResizeEvent *size) override;

    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;
    CategoriesWidget *categoriesWidget;
    TagsWidget *tagWidget;
    SettingDialog *settingDialog;

    Document m_content;

    void textChanged();

    void mSetOpenedNoteModel(bool initEditor = false);

    void mSetTableWidgetList();

    void mFiltrateTableWidgetList();

    bool isEditorModified();

    void mSetEditorModified(bool m);

    void mSetCategoriesListWidgetList();

    void mFiltrateCategoriesListWidgetList();

    void mSetTagsListWidgetList();

    void mFiltrateTagsListWidgetList();

    QMenu *createListWidgetCategoriesMenu();

    QMenu *createListWidgetTagsMenu();

    void setCategoriesListEnabled(bool b);

    void setTagsListEnabled(bool b);

    void mSelectedSidebarButtonByName(int i);

    void mInitAppMenu();
};

#endif // MAINWINDOW_H
