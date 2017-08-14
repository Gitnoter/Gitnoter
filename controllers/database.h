#ifndef DATABASE_H
#define DATABASE_H

#include "notemodel.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

#define __DB_NAME__ "gitnoter.db"

class Database : public QObject
{
    Q_OBJECT
public:
    Database();
    Database(const QString filename);
    ~Database();

    void connent(const QString filename);
    void initTables();

    QList<NoteModel *> selectNote();

    void insertNote(NoteModel *noteModel);
    void insertTag(NoteModel *noteModel);
    void insertCategorie(NoteModel *noteModel);
    void insertNoteJoinCategorie(NoteModel *noteModel);
    void insertNoteJoinTag(NoteModel *noteModel);

private:
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // DATABASE_H
