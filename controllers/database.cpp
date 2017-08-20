#include "database.h"
#include "tools.h"

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
    const QFileInfoList fileInfoList = Tools::getFilesPath(path);
    for (auto &&fileInfo : fileInfoList) {

    }
}

QList<NoteModel *> Database::selectNote()
{
    QList<NoteModel *> result;
    selectBuilder.select("uuid", "title", "create_date").from("notes");
    QString sql = QString::fromStdString(selectBuilder.str());

    if (query.exec(sql)) {
        while (query.next()) {
            QString uuid = query.value(0).toString();
            QString title = query.value(1).toString();
            int createDate = query.value(2).toInt();

            NoteModel *noteModel = new NoteModel(uuid, title, createDate);
            result.append(noteModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }
    this->clear();

    return result;
}

QString Database::insertNote(NoteModel *noteModel)
{
    insertBuilder.insert("title", noteModel->getTitle())
            ("create_date", noteModel->getCreateDate())
            ("update_date", noteModel->getUpdateDate())
            ("body", noteModel->getBody()).into("notes");
    sql = QString::fromStdString(insertBuilder.str());

    QString insertId = "";
    if (!query.exec(sql)) {
        qDebug() << "void Database::insertNotes(NoteModel noteModel): Failed!";
    }
    else {
        insertId = query.lastInsertId().toString();
    }
    this->clear();

    return insertId;
}

void Database::clear() {
    query.clear();
    selectBuilder.reset();
    insertBuilder.reset();
    updateBuilder.reset();
    deleteBuilder.reset();
    sql = "";
}
