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

    void setGroupName();
    void setOpenNote();
    void setNoteList();
    void setItemSelected();

private slots:
    void on_splitter_splitterMoved(int pos, int index);

    // QTreeWidget

    void on_pushButton_add_clicked();

    void on_pushButton_subtract_clicked();

    void on_action_saveNote_triggered();

    void on_groupTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    // QListWidget

    void on_pushButton_noteAdd_clicked();

    void on_pushButton_noteSubtract_clicked();

    void on_noteListWidget_itemClicked(QListWidgetItem *item);

    void onCategoryAppend(const QString &category);

    void onGroupSubtracted(Gitnoter::GroupType type, const QString &name);

    void onNoteDeleted();

    void onNoteAdded();

private:
    Ui::MainWindow *ui;
    NoteModel *mNoteModel;

private:
    void setupUi();

    void restoreNote();
    void newNote();
    void removeNote();  // remove from disk
    void deleteNote();  // set isDelete: 1

    void modifyTextAllNote(Gitnoter::GroupType type, const QString &oldText, const QString &newText = "");
    void removeTextAllNote(Gitnoter::GroupType type, const QString &oldText);
};

#endif // MAINWINDOW_H
