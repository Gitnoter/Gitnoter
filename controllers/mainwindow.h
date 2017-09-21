#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "categorieswidget.h"
#include "notemodel.h"
#include "configtablemodel.h"
#include "tagswidget.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>

namespace Ui {
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

    // 左侧菜单栏
    void on_pushButton_notes_clicked();
    void on_pushButton_folder_clicked();
    void on_pushButton_tags_clicked();

    // 菜单分页第一页
    void on_tableWidget_list_doubleClicked(const QModelIndex &index);
    void on_tableWidget_list_itemClicked(QTableWidgetItem *item);
    void onHeaderViewSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    void on_pushButton_deleteNote_clicked();
    void on_pushButton_categories_clicked();
    void on_pushButton_changeTags_clicked();

    // 菜单分页第二页
    void on_listWidget_categories_itemClicked(QListWidgetItem *item);
    void on_listWidget_categories_doubleClicked(const QModelIndex &index);
    void on_listWidget_categories_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_addCategories_clicked();
    void on_pushButton_removeCategories_clicked();

    void onActionRenameCategoriesTriggered();
    void onActionNameSortTriggered();
    void onActionCountSortTriggered();
    void onActionTimeSortTriggered();

    void onLineEditNameEditingFinished();

    void on_lineEdit_searchCategories_textChanged(const QString &arg1);

    // 菜单分页第二页
    void on_listWidget_tags_itemClicked(QListWidgetItem *item);
    void on_listWidget_tags_doubleClicked(const QModelIndex &index);
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

signals:
    void resizeChildWindow(QSize size);

protected:
    void resizeEvent(QResizeEvent *size) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    Ui::MainWindow *ui;
    Document m_content;
    QMap<QString, QPushButton *> menuPushButtons;
    CategoriesWidget *categoriesWidget;
    TagsWidget *tagWidget;
    QList<NoteTableModel *> *m_sidebarNoteList;
    QList<CategoriesTableModel *> m_categoriesModelList;
    QList<TagsTableModel *> mTagsTableModelList;

    void textChanged();
    void updatePreview();
    void menuPushButtonClicked();

    void initNotesToDatabases();
    void setDefaultNote();
    void setSidebarTable(QString text = "");
    void setEditText();

    bool isModified();
    void setModified(bool m);

    void setTagsData();
    void setMainWindowData();

    void setCategoriesList(bool reread = true, const QString &string = "");
    void setTagsList(bool reread = true, const QString &string = "");

    QMenu* createListWidgetCategoriesMenu();
    QMenu* createListWidgetTagsMenu();

    void setCategoriesListEnabled(bool b);
    void setTagsListEnabled(bool b);
};

#endif // MAINWINDOW_H
