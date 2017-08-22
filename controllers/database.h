#ifndef DATABASE_H
#define DATABASE_H

#include "notemodel.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class Database : public QObject
{
    Q_OBJECT
public:
    Database();
    Database(QString filename);
    ~Database();

    void connect(QString filename);
    void createTables();
    void initTables();

    QList<NoteModel *> selectNote();
    QString insertNote(NoteTableModel *noteTableModel);
    void insertTag(TagTableModel *tagTableModel);
    void insertCategorie(CategoriseTableModel *categoriseTableModel);

private:
    QSqlDatabase db;
    QSqlQuery query;
    QString sql;
};

#endif // DATABASE_H
