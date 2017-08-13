#ifndef DATABASE_H
#define DATABASE_H

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

private:
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // DATABASE_H
