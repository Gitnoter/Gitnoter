#ifndef NOTELISTWIDGET_H
#define NOTELISTWIDGET_H

#include "notemodel.h"
#include "mainwindow.h"

#include <QListWidget>

class MainWindow;

class NoteListWidget : public QListWidget
{
Q_OBJECT

public:
    explicit NoteListWidget(QWidget *parent = 0);
    ~NoteListWidget();

    void init(MainWindow *mainWindow);

    QListWidgetItem *append(NoteModel *noteModel);
    NoteModel *append(const QString &category);

    void remove(const QString &uuid);
    void trash(const QString &uuid);
    void restore(const QString &uuid);
    void modify();
    void search();
    void sort();

    void setListWidget();
    void setItemSelected();

public slots:
    void noteModelChanged(NoteModel *noteModel);

private:
    bool has(const QString &uuid);

public:
    QList<NoteModel *> getNoteModelList();

    QListWidgetItem *getItem(const QString &uuid);

    NoteModel *getNoteModel(const QString &uuid);

    NoteModel *getNoteModel(QListWidgetItem *listWidgetItem);

private:
    MainWindow *mMainWindow;
    QList<QListWidgetItem *> mListWidgetItemList;    // display listWidgetItem

};

#endif // NOTELISTWIDGET_H
