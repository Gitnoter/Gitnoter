#ifndef DATABASE_H
#define DATABASE_H

#include "notemodel.h"
#include "sqlquerybuilder.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

#define __DB_NAME__ "gitnoter.db"

using namespace SqlQueryBuilder;

class Database : public QObject
{
    Q_OBJECT
public:
    Database();
    Database(QString filename);
    ~Database();

    void connent(QString filename);
    void createTables();
    void initNoteData(QString path);
    void initTables();

    void clear();

    QList<NoteModel *> selectNote();

    QString insertNote(NoteModel *noteModel);
    void insertTag(NoteModel *noteModel);
    void insertCategorie(NoteModel *noteModel);
    void insertNoteJoinCategorie(NoteModel *noteModel);
    void insertNoteJoinTag(NoteModel *noteModel);

private:
    QSqlDatabase db;
    QSqlQuery query;
    SelectModel selectBuilder;
    InsertModel insertBuilder;
    UpdateModel updateBuilder;
    DeleteModel deleteBuilder;
    QString sql;
};

#endif // DATABASE_H
