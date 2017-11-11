#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "categorylistwidget.h"
#include "configmodel.h"
#include "settingdialog.h"
#include "aboutdialog.h"

#include "notemodel.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QTimer>
#include <QTreeWidgetItem>

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

private slots:
    void on_splitter_splitterMoved(int pos, int index);

    // QTreeWidget

    void on_pushButton_add_clicked();

    void on_pushButton_subtract_clicked();

    void on_action_saveNote_triggered();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    // QListWidget

    void on_pushButton_noteAdd_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_noteSubtract_clicked();

    void onNoteModelChanged(NoteModel *noteModel);

    void onCategoryAppend(const QString &category);

    void onGroupSubtracted(Gitnoter::GroupType type, const QString &name);

    void onGroupRemoved(Gitnoter::GroupType type, const QString &name);

    void onTagAppend(const QString &tag);

    void onTagDeleted(const QString &name = "", bool remove = false);

    void onNoteDeleted();

    void onNoteAdded();

private:
    Ui::MainWindow *ui;
    NoteModel *mNoteModel;

private:
    void setupUi();
    void setNoteList();
    void setOpenNote();
    void setItemSelected();
    void setGroupName();

    void restoreNote();
    void newNote();
    void removeNote();  // remove from disk
    void deleteNote();  // set isDelete: 1

};

#endif // MAINWINDOW_H
