#ifndef NOTELISTWIDGET_H
#define NOTELISTWIDGET_H

#include "notemodel.h"

#include <QListWidget>

class NoteListWidget : public QListWidget
{
Q_OBJECT

public:
    explicit NoteListWidget(QWidget *parent = 0);
    ~NoteListWidget();

    void init();

    void appendNew(NoteModel *noteModel);

    NoteModel *appendNew(const QString &category);

    void removeOne(NoteModel *noteModel);

    void deleteOne(const QString &uuid);

    void appendOld(const QString &uuid);

    QListWidgetItem *getItemByUuid(const QString &uuid);

    NoteModel *getNoteModelByUuid(const QString &uuid);

    void showListItems();

    const QString setItemSelected(const QString &uuid);

public slots:
    void onNoteListWidgetChanged(NoteModel *noteModel);

public:
    void setNoteModelList(const QList<NoteModel *> &noteModelList) { mNoteModelList = noteModelList; }

    const QList<NoteModel *> &getNoteModelList() const { return mNoteModelList; }

private:
    QList<NoteModel *> mNoteModelList;

};

#endif // NOTELISTWIDGET_H
