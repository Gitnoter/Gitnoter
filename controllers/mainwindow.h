#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "categorieswidget.h"
#include "notemodel.h"
#include "configtablemodel.h"

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

protected:
    void resizeEvent(QResizeEvent *size);

public slots:
    void onChangeCategories();

private slots:
    void on_action_saveFile_triggered();
    void on_action_newFile_triggered();
    void on_action_removeFile_triggered();

    // sidebar 0
    void on_pushButton_notes_clicked();

    void on_tableWidget_list_doubleClicked(const QModelIndex &index);
    void on_tableWidget_list_itemClicked(QTableWidgetItem *item);
    void on_headerView_sortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    void on_pushButton_deleteNote_clicked();
    void on_pushButton_categories_clicked();

    // sidebar 1
    void on_pushButton_folder_clicked();

    void on_listWidget_categories_itemClicked(QListWidgetItem *item);
    void on_listWidget_categories_doubleClicked(const QModelIndex &index);
    void on_listWidget_categories_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_addCategories_clicked();
    void on_pushButton_removeCategories_clicked();
    void on_action_renameCategories_triggered();

    void on_action_nameSort_triggered();
    void on_action_countSort_triggered();
    void on_action_timeSort_triggered();

    void on_lineEdit_name_editingFinished();

    void on_lineEdit_searchCategories_textChanged(const QString &arg1);

signals:
    void resizeChildWindow(QSize size);

private:
    Ui::MainWindow *ui;
    Document m_content;
    QMap<QString, QPushButton *> menuPushButtons;
    CategoriesWidget *categoriesWidget;
    QList<NoteTableModel *> *m_sidebarNoteList;
    QList<NoteTableModel *> *m_sidebarNoteListByCategoriesId;
    QList<CategoriesTableModel *> m_categoriesList;
    QList<CategoriesTableModel *> m_categoriesSearchList;

    void textChanged();
    void updatePreview();
    void menuPushButtonClicked();

    void initNotesToDatabases();
    void setDefaultNote();
    void setSidebarTable();
    void setEditText();

    bool isModified();
    void setModified(bool m);

    void setTagsData();
    void setMainWindowData();
    void resetListWidgetCategoriesBorder();

    void setCategoriesList(bool reread = true, const QString &string = "");

    QMenu* createListWidgetCategoriesMenu();;

};

#endif // MAINWINDOW_H
