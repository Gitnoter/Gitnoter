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

void Database::createTables()
{
    QFile tablesFile(":/database/gitnoter.sql");
    tablesFile.open(QIODevice::ReadOnly);
    QString tablesSql = QString(tablesFile.readAll());
    QStringList sqlList = tablesSql.split("\n");

    for (int i = 0; i < sqlList.length(); ++i) {
        if (!sqlList[i].isEmpty()) {
            query.exec(sqlList[i]);
        }
        query.clear();
    }
}

void Database::initTables()
{
    if (!query.exec("select * from config")) {
        query.clear();
        this->createTables();
    }
}

void Database::initNoteData(const QString path)
{
    this->createTables();

    QDir dir(path);

    foreach(QFileInfo mfi ,dir.entryInfoList()) {
        if(mfi.isFile()) {
            qDebug()<< "File :" << path << "/" << mfi.fileName();
        }
        else {
            if(mfi.fileName() == "." || mfi.fileName() == "..") {
                continue;
            }
            this->initNoteData(mfi.absoluteFilePath());
        }
    }
}


int Database::insertNote(NoteModel *noteModel)
{
    query.prepare("INSERT INTO notes (title, create_date, update_date, body) VALUES (?, ?, ?, ?)");
    query.bindValue(0, noteModel->getTitle());
    query.bindValue(1, noteModel->getCreateDate());
    query.bindValue(2, noteModel->getUpdateDate());
    query.bindValue(3, noteModel->getBody());

    int insertId = 0;
    if (!query.exec()) {
        qDebug() << "void Database::insertNotes(NoteModel noteModel): Failed!";
    }
    else {
        insertId = query.lastInsertId().toInt();
    }
    query.clear();

    return insertId;
}

QList<NoteModel *> Database::selectNote()
{
    QList<NoteModel *> result;
    if (query.exec("select id, title, create_date from notes")) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString title = query.value(1).toString();
            int createDate = query.value(2).toInt();

            NoteModel *noteModel = new NoteModel(id, title, createDate);
            result.append(noteModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }
    query.clear();

    return result;
}
