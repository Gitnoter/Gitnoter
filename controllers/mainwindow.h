#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "document.h"
#include "categorieswidget.h"
#include "notemodel.h"
#include "database.h"
#include "configtablemodel.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>

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

private slots:
    void on_pushButton_clicked();
    void on_tableWidget_list_doubleClicked(const QModelIndex &index);

    void onSaveFile();
    void onNewFile();
    void onRemoveFile();

    void on_pushButton_deleteNote_clicked();
    void on_tableWidget_list_clicked(const QModelIndex &index);

    void on_headerView_sortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

signals:
    void resizeChildWindow(QSize size);

private:
    Ui::MainWindow *ui;
    Document m_content;
    QMap<QString, QPushButton *> menuPushButtons;
    CategoriesWidget *categoriesWidget;
    NoteModel *noteModel;
    ConfigTableModel *configTableModel;
    Database *database;
    QList<NoteTableModel *> *sidebarNoteList;

    void textChanged();
    void updatePreview();
    void menuPushButtonClicked();

    void initNotesToDatabases();
    void setDefaultNote();
    void setSidebarTable();
    void setEditText();

    bool isModified();
    void setModified(bool m);
};

#endif // MAINWINDOW_H
