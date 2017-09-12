#include "database.h"
#include "tools.h"
#include "sqlquerybuilder.h"
#include "globals.h"

using namespace SqlQueryBuilder;

Database::Database()
{
    this->connect(g_dbName);
}

Database::Database(const QString filename)
{
    this->connect(filename);
}

Database::~Database()
{
    db.close();
}

void Database::initTables()
{
    if (query.exec("select version from config") && query.first()) {
        QString version = query.value(0).toString();
        if (version != g_version) {
            this->createTables();
        }
    }
    else {
        this->createTables();
    }
}

void Database::addNoteText(NoteModel *noteModel) {
    // 插入 notes
    NoteTableModel *noteTableModel = this->selectNotesTableByUuid(noteModel->noteTableModel->getUuid());
    QString uuid = "";
    if (noteTableModel) {
        this->updateNotesTableByUuid(noteModel->noteTableModel);
        uuid = noteTableModel->getUuid();
    }
    else {
        uuid = this->insertNotesTable(noteModel->noteTableModel);
    }

    // 插入 tags
    this->deleteNJTTableByNotesUuid(uuid);
    for (auto &&item : *noteModel->tagTableList) {
        TagTableModel *tagTableModel = this->selectTagsTableByName(item->getName());
        int tagId = tagTableModel ? tagTableModel->getTagsId() : this->insertTagsTable(item->getName());
        this->insertNJTTable(uuid, tagId);
    }

    // 插入 categories
    this->deleteNJCTableByNotesUuid(uuid);
    CategoriesTableModel *categoriseTableModel = this->selectCategoriesTableById(
            noteModel->categoriesTableModel->getName());
    int categoriesId = categoriseTableModel ? categoriseTableModel->getCategoriesId()
                                            : this->insertCategoriesTable(noteModel->categoriesTableModel->getName());
    this->insertNJCTable(uuid, categoriesId);
}

NoteModel* Database::getNoteByUuid(QString uuid)
{
    // 获取 notes 数据
    NoteTableModel *noteTableModel = this->selectNotesTableByUuid(uuid);

    // 获取 tags 数据
    QList<TagTableModel *> *tagTableList = this->selectNJTTableByNotesUuid(uuid);
    for (int i = 0; i < tagTableList->length(); ++i) {
        TagTableModel *tagTableModel = this->selectTagsTableById(tagTableList->at(i)->getTagsId());
        tagTableList->replace(i, *tagTableList->at(i) + *tagTableModel);
    }

    // 获取 categories 数据
    CategoriesTableModel *categoriseTableModel = nullptr;
    QList<CategoriesTableModel *> categoriseTableList = this->selectNJCTableByNotesUuid(uuid);
    if (categoriseTableList.length() > 0) {
        for (int i = 0; i < categoriseTableList.length(); ++i) {
            CategoriesTableModel *ctm = this->selectCategoriesTableById(categoriseTableList[i]->getCategoriesId());
            categoriseTableList[i] = *categoriseTableList[i] + *ctm;
        }
        categoriseTableModel = categoriseTableList[0];
    }

    return new NoteModel(noteTableModel, tagTableList, categoriseTableModel);
}

void Database::deleteNoteByUuid(QString uuid)
{
    this->deleteNotesTableByUuid(uuid);
    this->deleteNJTTableByNotesUuid(uuid);
    this->deleteNJCTableByNotesUuid(uuid);
}

QList<NoteTableModel *> *Database::getSidebarNotes()
{
    QList<NoteTableModel *> *result = new QList<NoteTableModel *>();

    SelectModel selectBuilder;
    selectBuilder.select("uuid", "title", "create_date", "update_date").from("notes");
    QString sql = QString::fromStdString(selectBuilder.str());

    if (query.exec(sql)) {
        while (query.next()) {
            QString uuid = query.value(0).toString();
            QString title = query.value(1).toString();
            int createDate = query.value(2).toInt();
            int updateDate = query.value(3).toInt();
            result->append(new NoteTableModel(uuid, title, createDate, updateDate));
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

void Database::connect(const QString filename)
{
    QDir dir(g_dbPath);
    if (!dir.exists()) {
        dir.mkpath(g_dbPath);
        qDebug() << "dir is not exists";
    }

    QString path = dir.filePath(filename);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (db.open()) {
        qDebug() << "Database: connection ok";
        query = QSqlQuery(db);
//        this->initTables();
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
    }
}

QString Database::insertNotesTable(NoteTableModel *noteTableModel)
{
    InsertModel insertBuilder;
    insertBuilder.insert("uuid", noteTableModel->getUuid().toStdString())
            ("title", noteTableModel->getTitle().toStdString())
            ("create_date", noteTableModel->getCreateDate())
            ("update_date", noteTableModel->getUpdateDate())
            ("body", noteTableModel->getBody().toStdString())
            ("file_path", noteTableModel->getFilePath().toStdString()).into("notes");
    QString sql = QString::fromStdString(insertBuilder.str());

    if (!query.exec(sql)) {
        qDebug() << "void Database::insertNotes(NoteModel noteModel): Failed!";
    }

    return noteTableModel->getUuid();
}

bool Database::deleteNotesTableByUuid(QString uuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes").where(column("uuid") == uuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

bool Database::updateNotesTableByUuid(QString uuid, QString title, QString body, int updateDate)
{
    UpdateModel updateBuilder;
    updateBuilder.update("notes")
            .set("title", title.toStdString())
                    ("body", body.toStdString())
                    ("update_date", updateDate)
            .where(column("uuid") == uuid.toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

bool Database::updateNotesTableByUuid(NoteTableModel *noteTableModel)
{
    UpdateModel updateBuilder;
    updateBuilder.update("notes")
            .set("title", noteTableModel->getTitle().toStdString())
                    ("body", noteTableModel->getBody().toStdString())
                    ("create_date", noteTableModel->getCreateDate())
                    ("update_date", noteTableModel->getUpdateDate())
                    ("file_path", noteTableModel->getFilePath().toStdString())
            .where(column("uuid") == noteTableModel->getUuid().toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

NoteTableModel* Database::selectNotesTableByUuid(QString uuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("title", "create_date", "update_date", "body", "file_path")
            .from("notes")
            .where(column("uuid") == uuid.toStdString())
            .order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    NoteTableModel *result = nullptr;
    if (query.exec(sql) && query.first()) {
        QString title = query.value(0).toString();
        int createDate = query.value(1).toInt();
        int updateDate = query.value(2).toInt();
        QString body = query.value(3).toString();
        QString filePath = query.value(4).toString();
        result = new NoteTableModel(uuid, title, createDate, updateDate, body, filePath);
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

int Database::insertTagsTable(QString name)
{
    InsertModel insertBuilder;
    insertBuilder.insert("name", name.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch())
            ("update_date", QDateTime::currentSecsSinceEpoch()).into("tags");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!query.exec(sql)) {
        qDebug() << "int Database::insertTagsTable(QString name): Failed!";
    }
    else {
        insertId = query.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteTagsTableByName(QString name)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("tags").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

bool Database::updateTagsTableByName(QString name, QString whereName)
{
    UpdateModel updateBuilder;
    updateBuilder.update("tags")
            .set("name", name.toStdString())
                    ("update_date", QDateTime::currentSecsSinceEpoch())
            .where(column("name") == whereName.toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

QList<TagTableModel *> Database::selectTagsTable()
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "name", "create_date", "update_date").from("tags").order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagTableModel *> result;
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            int createDate = query.value(2).toInt();
            int updateDate = query.value(3).toInt();

            TagTableModel *tagTableModel = new TagTableModel(id, name, createDate, updateDate);
            result.append(tagTableModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

TagTableModel* Database::selectTagsTableById(int id)
{
    SelectModel selectBuilder;
    selectBuilder.select("name", "create_date", "update_date").from("tags").where(column("id") == id);
    QString sql = QString::fromStdString(selectBuilder.str());

    TagTableModel *result = nullptr;
    if (query.exec(sql) && query.first()) {
        QString name = query.value(0).toString();
        int createDate = query.value(1).toInt();
        int updateDate = query.value(2).toInt();

        result = new TagTableModel(id, name, createDate, updateDate);
    }
    else {
        qDebug() << "TagTableModel* Database::selectTagsTableById(int id): failed!";
    }

    return result;
}

TagTableModel *Database::selectTagsTableByName(QString name)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "create_date", "update_date").from("tags").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(selectBuilder.str());

    TagTableModel *result = nullptr;
    if (query.exec(sql) && query.first()) {
        int id = query.value(0).toInt();
        int createDate = query.value(1).toInt();
        int updateDate = query.value(2).toInt();

        result = new TagTableModel(id, name, createDate, updateDate);
    }
    else {
        qDebug() << "TagTableModel *Database::selectTagsTableByName(QString name): failed!";
    }

    return result;
}

int Database::insertCategoriesTable(QString name)
{
    InsertModel insertBuilder;
    insertBuilder.insert("name", name.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch())
            ("update_date", QDateTime::currentSecsSinceEpoch()).into("categories");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!query.exec(sql)) {
        qDebug() << "int Database::insertCategoriesTable(QString name): Failed!";
    }
    else {
        insertId = query.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteCategoriesTableByName(QString name)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("categories").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

bool Database::updateCategoriesTableByName(QString name, QString whereName)
{
    UpdateModel updateBuilder;
    updateBuilder.update("categories")
            .set("name", name.toStdString())
                    ("update_date", QDateTime::currentSecsSinceEpoch())
            .where(column("name") == whereName.toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

QList<CategoriesTableModel *> Database::selectCategoriesTable()
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "name", "create_date", "update_date", "is_default").from("categories")
            .order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesTableModel *> result;
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            int createDate = query.value(2).toInt();
            int updateDate = query.value(3).toInt();
            int isDefault = query.value(4).toInt();

            CategoriesTableModel *categoriesTableModel = new CategoriesTableModel(id, name, createDate,
                                                                                  updateDate, isDefault);
            result.append(categoriesTableModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

CategoriesTableModel* Database::selectCategoriesTableById(int id)
{
    SelectModel selectBuilder;
    selectBuilder.select("name", "create_date", "update_date", "is_default").from("categories").where(column("id") == id);
    QString sql = QString::fromStdString(selectBuilder.str());

    CategoriesTableModel *result = nullptr;
    if (query.exec(sql) && query.first()) {
        QString name = query.value(0).toString();
        int createDate = query.value(1).toInt();
        int updateDate = query.value(2).toInt();
        int isDefault = query.value(3).toInt();

        result = new CategoriesTableModel(id, name, createDate, updateDate, isDefault);
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

CategoriesTableModel *Database::selectCategoriesTableById(QString name)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "create_date", "update_date", "is_default").from("categories")
            .where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(selectBuilder.str());

    CategoriesTableModel *result = nullptr;
    if (query.exec(sql) && query.first()) {
        int id = query.value(0).toInt();
        int createDate = query.value(1).toInt();
        int updateDate = query.value(2).toInt();
        int isDefault = query.value(3).toInt();

        result = new CategoriesTableModel(id, name, createDate, updateDate, isDefault);
    }
    else {
        qDebug() << "CategoriesTableModel *Database::selectCategoriesTableById(QString name): failed!";
    }

    return result;
}

int Database::insertNJTTable(QString notesUuid, int tagsId)
{
    InsertModel insertBuilder;
    insertBuilder.insert("tags_id", tagsId)
            ("notes_uuid", notesUuid.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch()).into("notes_join_tags");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!query.exec(sql)) {
        qDebug() << "int Database::insertNJTTable(QString notesUuid, int tagsId): Failed!";
    }
    else {
        insertId = query.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteNJTTableByNotesUuid(QString notesUuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_tags").where(column("notes_uuid") == notesUuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

bool Database::deleteNJTTableById(QString notesUuid, int tagsId)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_tags")
            .where(column("notes_uuid") == notesUuid.toStdString() and column("tags_id") == tagsId);
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

QList<TagTableModel *>* Database::selectNJTTableByNotesUuid(QString notesUuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "tags_id", "create_date").from("notes_join_tags")
            .where(column("notes_uuid") == notesUuid.toStdString()).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagTableModel *> *result = new QList<TagTableModel *>();
    qDebug() << result->length();
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            int tagsId = query.value(1).toInt();
            int createDate = query.value(2).toInt();

            TagTableModel *tagTableModel = new TagTableModel(id, tagsId, notesUuid, createDate);
            result->append(tagTableModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

QList<TagTableModel *> Database::selectNJTTableByTagsId(int tagsId)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "notes_Uuid", "create_date").from("notes_join_tags")
            .where(column("tags_id") == tagsId).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagTableModel *> result;
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString notesUuid = query.value(1).toString();
            int createDate = query.value(2).toInt();

            TagTableModel *tagTableModel = new TagTableModel(id, tagsId, notesUuid, createDate);
            result.append(tagTableModel);
        }
    }
    else {
        qDebug() << "QList<TagTableModel> Database::selectNJTTableByTagsId(int tagsId): failed!";
    }

    return result;
}


int Database::insertNJCTable(QString notesUuid, int categoriesId)
{
    InsertModel insertBuilder;
    insertBuilder.insert("categories_id", categoriesId)
            ("notes_uuid", notesUuid.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch()).into("notes_join_categories");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!query.exec(sql)) {
        qDebug() << "int Database::insertNJCTable(QString notesUuid, int categoriesId): Failed!";
    }
    else {
        insertId = query.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteNJCTableByNotesUuid(QString notesUuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_categories").where(column("notes_uuid") == notesUuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

bool Database::deleteNJCTableById(QString notesUuid, int categoriesId)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_categories")
            .where(column("notes_uuid") == notesUuid.toStdString() and column("categories_id") == categoriesId);
    QString sql = QString::fromStdString(deleteBuilder.str());
    query.exec(sql);

    return query.numRowsAffected() != -1;
}

QList<CategoriesTableModel *> Database::selectNJCTableByNotesUuid(QString notesUuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "categories_id", "create_date").from("notes_join_categories")
            .where(column("notes_uuid") == notesUuid.toStdString()).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesTableModel *> result;
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            int categoriesId = query.value(1).toInt();
            int createDate = query.value(2).toInt();

            CategoriesTableModel *tagTableModel = new CategoriesTableModel(id, categoriesId, notesUuid, createDate);
            result.append(tagTableModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

QList<CategoriesTableModel *> Database::selectNJCTableByCategoriesId(int categoriesId)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "notes_uuid", "create_date").from("notes_join_categories")
            .where(column("categories_id") == categoriesId).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesTableModel *> result;
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString notesUuid = query.value(1).toString();
            int createDate = query.value(2).toInt();

            CategoriesTableModel *tagTableModel = new CategoriesTableModel(id, categoriesId, notesUuid, createDate);
            result.append(tagTableModel);
        }
    }
    else {
        qDebug() << "QList<TagTableModel> Database::selectNJTTableByTagsId(int tagsId): failed!";
    }

    return result;
}
