#include "database.h"
#include "tools.h"
#include "sqlquerybuilder.h"
#include "appconfig.h"

#include <QStandardPaths>
#include <QDir>
#include <QDebug>

using namespace SqlQueryBuilder;

Database::Database()
{
    this->connect(AppConfig::dbName);
}

Database::Database(const QString filename)
{
    this->connect(filename);
}

Database::~Database()
{
    db.close();
}

void Database::connect(const QString filename)
{
    QDir dir(AppConfig::dbPath);
    if (!dir.exists()) {
        dir.mkpath(AppConfig::dbPath);
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

QList<NoteModel *> Database::selectNote()
{
    QList<NoteModel *> result;
//    selectBuilder.select("uuid", "title", "create_date").from("notes");
//    QString sql = QString::fromStdString(selectBuilder.str());

//    if (query.exec(sql)) {
//        while (query.next()) {
//            QString uuid = query.value(0).toString();
//            QString title = query.value(1).toString();
//            int insertDate = query.value(2).toInt();

////            NoteTableModel *noteTableModel = new NoteTableModel(uuid, title, insertDate);
////            result.append(noteTableModel);
//        }
//    }
//    else {
//        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
//    }
//    this->clear();

    return result;
}

QString Database::insertNote(NoteTableModel *noteTableModel)
{
//    InsertModel insertBuilder;
//    insertBuilder.insert("uuid", Tools::getUuid())
//            ("title", noteTableModel->getTitle())
//            ("create_date", noteTableModel->getCreateDate())
//            ("update_date", noteTableModel->getUpdateDate())
//            ("body", noteTableModel->getBody()).into("notes");
//    sql = QString::fromStdString(insertBuilder.str());
//
//    QString insertId = "";
//    if (!query.exec(sql)) {
//        qDebug() << "void Database::insertNotes(NoteModel noteModel): Failed!";
//    }
//    else {
//        insertId = query.lastInsertId().toString();
//    }
//    query.clear();
//
//    return insertId;
    return "";
}
