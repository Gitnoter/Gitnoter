#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "categorylistwidget.h"
#include "configmodel.h"
#include "settingdialog.h"
#include "aboutdialog.h"
#include "grouptreewidget.h"
#include "notelistwidget.h"
#include "markdowneditorwidget.h"
#include "menubar.h"
#include "notemodel.h"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include <QSplitter>

namespace Ui
{
    class MainWindow;
}

class GroupTreeWidget;
class MarkdownEditorWidget;
class NoteListWidget;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void init();
    void updateView(Gitnoter::UpdateViewFlags flags = Gitnoter::GroupTreeWidget);

    void appendGroup();

    void appendNote();
    void trashNote();         // set isDelete: 1
    void searchNote(const QString &text);
    void sortNote();
    void setNoteListTitle();

public slots:
    void removeGroup();

    void removeNote();    // remove from disk
    void restoreNote();

private:
    void setupUi();

    void changeEvent(QEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

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

    void on_lineEdit_noteSearch_textChanged(const QString &arg1);

public:
    GroupTreeWidget *groupTreeWidget();
    NoteListWidget *noteListWidget();
    MarkdownEditorWidget *markdownEditorWidget();
    QStackedWidget *stackedWidget();
    QSplitter *splitter();

private:
    Ui::MainWindow *ui;
    MenuBar *mMenuBar;


};

#endif // MAINWINDOW_H
