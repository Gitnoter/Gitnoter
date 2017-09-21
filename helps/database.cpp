#include "database.h"
#include "tools.h"
#include "sqlquerybuilder.h"
#include "globals.h"

using namespace SqlQueryBuilder;

Database::Database()
{
    this->connect(gDbName);
}

Database::Database(const QString filename)
{
    this->connect(filename);
}

Database::~Database()
{
    mDb.close();
}

void Database::initTables()
{
    if (mQuery.exec("select version from config") && mQuery.first()) {
        QString version = mQuery.value(0).toString();
        if (version != gVersion) {
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
        TagsTableModel *tagsTableModel = this->selectTagsTableByName(item->getName());
        int tagId = tagsTableModel ? tagsTableModel->getTagsId() : this->insertTagsTable(item->getName());
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
    QList<TagsTableModel *> *tagTableList = this->selectNJTTableByNotesUuid(uuid);
    for (int i = 0; i < tagTableList->length(); ++i) {
        TagsTableModel *tagsTableModel = this->selectTagsTableById(tagTableList->at(i)->getTagsId());
        tagTableList->replace(i, *tagTableList->at(i) + *tagsTableModel);
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

    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            QString uuid = mQuery.value(0).toString();
            QString title = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();
            int updateDate = mQuery.value(3).toInt();
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
    QDir dir(gDbPath);
    if (!dir.exists()) {
        dir.mkpath(gDbPath);
        qDebug() << "dir is not exists";
    }

    QString path = dir.filePath(filename);
    mDb = QSqlDatabase::addDatabase("QSQLITE");
    mDb.setDatabaseName(path);

    if (mDb.open()) {
        qDebug() << "Database: connection ok";
        mQuery = QSqlQuery(mDb);
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
            mQuery.exec(sqlList[i]);
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

    if (!mQuery.exec(sql)) {
        qDebug() << "void Database::insertNotes(NoteModel noteModel): Failed!";
    }

    return noteTableModel->getUuid();
}

bool Database::deleteNotesTableByUuid(QString uuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes").where(column("uuid") == uuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
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
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
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
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
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
    if (mQuery.exec(sql) && mQuery.first()) {
        QString title = mQuery.value(0).toString();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();
        QString body = mQuery.value(3).toString();
        QString filePath = mQuery.value(4).toString();
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
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertTagsTable(QString name): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteTagsTableByName(QString name)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("tags").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::updateTagsTableByName(QString name, QString whereName)
{
    UpdateModel updateBuilder;
    updateBuilder.update("tags")
            .set("name", name.toStdString())
                    ("update_date", QDateTime::currentSecsSinceEpoch())
            .where(column("name") == whereName.toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

QList<TagsTableModel *> Database::selectTagsTable()
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "name", "create_date", "update_date").from("tags").order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagsTableModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString name = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();
            int updateDate = mQuery.value(3).toInt();

            TagsTableModel *tagsTableModel = new TagsTableModel(id, name, createDate, updateDate);
            result.append(tagsTableModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

TagsTableModel* Database::selectTagsTableById(int id)
{
    SelectModel selectBuilder;
    selectBuilder.select("name", "create_date", "update_date").from("tags").where(column("id") == id);
    QString sql = QString::fromStdString(selectBuilder.str());

    TagsTableModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        QString name = mQuery.value(0).toString();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();

        result = new TagsTableModel(id, name, createDate, updateDate);
    }
    else {
        qDebug() << "TagsTableModel* Database::selectTagsTableById(int id): failed!";
    }

    return result;
}

TagsTableModel *Database::selectTagsTableByName(QString name)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "create_date", "update_date").from("tags").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(selectBuilder.str());

    TagsTableModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        int id = mQuery.value(0).toInt();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();

        result = new TagsTableModel(id, name, createDate, updateDate);
    }
    else {
        qDebug() << "TagsTableModel *Database::selectTagsTableByName(QString name): failed!";
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
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertCategoriesTable(QString name): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteCategoriesTableByName(QString name)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("categories").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::updateCategoriesTableByName(QString name, QString whereName)
{
    UpdateModel updateBuilder;
    updateBuilder.update("categories")
            .set("name", name.toStdString())
                    ("update_date", QDateTime::currentSecsSinceEpoch())
            .where(column("name") == whereName.toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

QList<CategoriesTableModel *> Database::selectCategoriesTable()
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "name", "create_date", "update_date", "is_default").from("categories")
            .order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesTableModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString name = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();
            int updateDate = mQuery.value(3).toInt();
            int isDefault = mQuery.value(4).toInt();

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
    if (mQuery.exec(sql) && mQuery.first()) {
        QString name = mQuery.value(0).toString();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();
        int isDefault = mQuery.value(3).toInt();

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
    if (mQuery.exec(sql) && mQuery.first()) {
        int id = mQuery.value(0).toInt();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();
        int isDefault = mQuery.value(3).toInt();

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
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertNJTTable(QString notesUuid, int tagsId): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteNJTTableByNotesUuid(QString notesUuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_tags").where(column("notes_uuid") == notesUuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::deleteNJTTableById(QString notesUuid, int tagsId)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_tags")
            .where(column("notes_uuid") == notesUuid.toStdString() and column("tags_id") == tagsId);
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

QList<TagsTableModel *>* Database::selectNJTTableByNotesUuid(QString notesUuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "tags_id", "create_date").from("notes_join_tags")
            .where(column("notes_uuid") == notesUuid.toStdString()).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagsTableModel *> *result = new QList<TagsTableModel *>();
    qDebug() << result->length();
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            int tagsId = mQuery.value(1).toInt();
            int createDate = mQuery.value(2).toInt();

            TagsTableModel *tagsTableModel = new TagsTableModel(id, tagsId, notesUuid, createDate);
            result->append(tagsTableModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

QList<TagsTableModel *> Database::selectNJTTableByTagsId(int tagsId)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "notes_Uuid", "create_date").from("notes_join_tags")
            .where(column("tags_id") == tagsId).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagsTableModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString notesUuid = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();

            TagsTableModel *tagsTableModel = new TagsTableModel(id, tagsId, notesUuid, createDate);
            result.append(tagsTableModel);
        }
    }
    else {
        qDebug() << "QList<TagsTableModel> Database::selectNJTTableByTagsId(int tagsId): failed!";
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
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertNJCTable(QString notesUuid, int categoriesId): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteNJCTableByNotesUuid(QString notesUuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_categories").where(column("notes_uuid") == notesUuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::deleteNJCTableById(QString notesUuid, int categoriesId)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_categories")
            .where(column("notes_uuid") == notesUuid.toStdString() and column("categories_id") == categoriesId);
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

QList<CategoriesTableModel *> Database::selectNJCTableByNotesUuid(QString notesUuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "categories_id", "create_date").from("notes_join_categories")
            .where(column("notes_uuid") == notesUuid.toStdString()).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesTableModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            int categoriesId = mQuery.value(1).toInt();
            int createDate = mQuery.value(2).toInt();

            CategoriesTableModel *tagsTableModel = new CategoriesTableModel(id, categoriesId, notesUuid, createDate);
            result.append(tagsTableModel);
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
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString notesUuid = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();

            CategoriesTableModel *tagsTableModel = new CategoriesTableModel(id, categoriesId, notesUuid, createDate);
            result.append(tagsTableModel);
        }
    }
    else {
        qDebug() << "QList<TagsTableModel> Database::selectNJTTableByTagsId(int tagsId): failed!";
    }

    return result;
}
