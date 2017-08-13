#include "database.h"

#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Database::Database()
{
    this->connent(__DB_NAME__);
}

Database::Database(const QString filename)
{
    this->connent(filename);
}

Database::~Database()
{
    db.close();
}

void Database::connent(const QString filename)
{
    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(appConfigLocation);
    if (!dir.exists()) {
        dir.mkpath(appConfigLocation);
        qDebug() << "dir is not exists";
    }

    QString path = dir.filePath(filename);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (db.open()) {
        qDebug() << "Database: connection ok";
        query = QSqlQuery(db);
        this->initTables();
    }
    else {
        qDebug() << "Error: connection with database fail";
    }
}

void Database::initTables()
{
    if (!query.exec("select * from config")) {
        QFile tablesFile(":/database/gitnoter.sql");
        tablesFile.open(QIODevice::ReadOnly);
        QString tablesSql = QString(tablesFile.readAll());
        QStringList sqlList = tablesSql.split("\n");

        for (int i = 0; i < sqlList.length(); ++i) {
            query.clear();
            if (!sqlList[i].isEmpty()) {
                qDebug() << query.exec(sqlList[i]);
            }
        }
    }
    query.clear();
}
