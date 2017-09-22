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

void Database::inits()
{
    if (mQuery.exec("select version from config") && mQuery.first()) {
        QString version = mQuery.value(0).toString();
        if (version != gVersion) {
            this->creates();
        }
    }
    else {
        this->creates();
    }
}

void Database::addNoteText(NoteModel *noteModel) {
    // 插入 notes
    ContentModel *contentModel = this->selectNotesByUuid(noteModel->contentModel->getUuid());
    QString uuid = "";
    if (contentModel) {
        this->updateNotesByUuid(noteModel->contentModel);
        uuid = contentModel->getUuid();
    }
    else {
        uuid = this->insertNotes(noteModel->contentModel);
    }

    // 插入 tags
    this->deleteNJTByNotesUuid(uuid);
    for (auto &&item : *noteModel->tagsModelList) {
        TagsModel *tagsModel = this->selectTagsByName(item->getName());
        int tagId = tagsModel ? tagsModel->getTagsId() : this->insertTags(item->getName());
        this->insertNJT(uuid, tagId);
    }

    // 插入 categories
    this->deleteNJCByNotesUuid(uuid);
    CategoriesModel *categoriseModel = this->selectCategoriesById(
            noteModel->categoriesModel->getName());
    int categoriesId = categoriseModel ? categoriseModel->getCategoriesId()
                                            : this->insertCategories(noteModel->categoriesModel->getName());
    this->insertNJC(uuid, categoriesId);
}

NoteModel* Database::getNoteByUuid(QString uuid)
{
    // 获取 notes 数据
    ContentModel *contentModel = this->selectNotesByUuid(uuid);

    // 获取 tags 数据
    QList<TagsModel *> *tagList = this->selectNJTByNotesUuid(uuid);
    for (int i = 0; i < tagList->length(); ++i) {
        TagsModel *tagsModel = this->selectTagsById(tagList->at(i)->getTagsId());
        tagList->replace(i, *tagList->at(i) + *tagsModel);
    }

    // 获取 categories 数据
    CategoriesModel *categoriseModel = nullptr;
    QList<CategoriesModel *> categoriseList = this->selectNJCByNotesUuid(uuid);
    if (categoriseList.length() > 0) {
        for (int i = 0; i < categoriseList.length(); ++i) {
            CategoriesModel *ctm = this->selectCategoriesById(categoriseList[i]->getCategoriesId());
            categoriseList[i] = *categoriseList[i] + *ctm;
        }
        categoriseModel = categoriseList[0];
    }

    return new NoteModel(contentModel, tagList, categoriseModel);
}

void Database::deleteNoteByUuid(QString uuid)
{
    this->deleteNotesByUuid(uuid);
    this->deleteNJTByNotesUuid(uuid);
    this->deleteNJCByNotesUuid(uuid);
}

QList<ContentModel *> *Database::getSidebarNotes()
{
    QList<ContentModel *> *result = new QList<ContentModel *>();

    SelectModel selectBuilder;
    selectBuilder.select("uuid", "title", "create_date", "update_date").from("notes");
    QString sql = QString::fromStdString(selectBuilder.str());

    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            QString uuid = mQuery.value(0).toString();
            QString title = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();
            int updateDate = mQuery.value(3).toInt();
            result->append(new ContentModel(uuid, title, createDate, updateDate));
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
//        this->inits();
    }
    else {
        qDebug() << "Error: connection with database fail";
    }
}

void Database::creates()
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

QString Database::insertNotes(ContentModel *contentModel)
{
    InsertModel insertBuilder;
    insertBuilder.insert("uuid", contentModel->getUuid().toStdString())
            ("title", contentModel->getTitle().toStdString())
            ("create_date", contentModel->getCreateDate())
            ("update_date", contentModel->getUpdateDate())
            ("body", contentModel->getBody().toStdString())
            ("file_path", contentModel->getFilePath().toStdString()).into("notes");
    QString sql = QString::fromStdString(insertBuilder.str());

    if (!mQuery.exec(sql)) {
        qDebug() << "void Database::insertNotes(NoteModel noteModel): Failed!";
    }

    return contentModel->getUuid();
}

bool Database::deleteNotesByUuid(QString uuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes").where(column("uuid") == uuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::updateNotesByUuid(QString uuid, QString title, QString body, int updateDate)
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

bool Database::updateNotesByUuid(ContentModel *contentModel)
{
    UpdateModel updateBuilder;
    updateBuilder.update("notes")
            .set("title", contentModel->getTitle().toStdString())
                    ("body", contentModel->getBody().toStdString())
                    ("create_date", contentModel->getCreateDate())
                    ("update_date", contentModel->getUpdateDate())
                    ("file_path", contentModel->getFilePath().toStdString())
            .where(column("uuid") == contentModel->getUuid().toStdString());
    QString sql = QString::fromStdString(updateBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

ContentModel* Database::selectNotesByUuid(QString uuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("title", "create_date", "update_date", "body", "file_path")
            .from("notes")
            .where(column("uuid") == uuid.toStdString())
            .order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    ContentModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        QString title = mQuery.value(0).toString();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();
        QString body = mQuery.value(3).toString();
        QString filePath = mQuery.value(4).toString();
        result = new ContentModel(uuid, title, createDate, updateDate, body, filePath);
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

int Database::insertTags(QString name)
{
    InsertModel insertBuilder;
    insertBuilder.insert("name", name.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch())
            ("update_date", QDateTime::currentSecsSinceEpoch()).into("tags");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertTags(QString name): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteTagsByName(QString name)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("tags").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::updateTagsByName(QString name, QString whereName)
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

QList<TagsModel *> Database::selectTags()
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "name", "create_date", "update_date").from("tags").order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagsModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString name = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();
            int updateDate = mQuery.value(3).toInt();

            TagsModel *tagsModel = new TagsModel(id, name, createDate, updateDate);
            result.append(tagsModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

TagsModel* Database::selectTagsById(int id)
{
    SelectModel selectBuilder;
    selectBuilder.select("name", "create_date", "update_date").from("tags").where(column("id") == id);
    QString sql = QString::fromStdString(selectBuilder.str());

    TagsModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        QString name = mQuery.value(0).toString();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();

        result = new TagsModel(id, name, createDate, updateDate);
    }
    else {
        qDebug() << "TagsModel* Database::selectTagsById(int id): failed!";
    }

    return result;
}

TagsModel *Database::selectTagsByName(QString name)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "create_date", "update_date").from("tags").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(selectBuilder.str());

    TagsModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        int id = mQuery.value(0).toInt();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();

        result = new TagsModel(id, name, createDate, updateDate);
    }
    else {
        qDebug() << "TagsModel *Database::selectTagsByName(QString name): failed!";
    }

    return result;
}

int Database::insertCategories(QString name)
{
    InsertModel insertBuilder;
    insertBuilder.insert("name", name.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch())
            ("update_date", QDateTime::currentSecsSinceEpoch()).into("categories");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertCategories(QString name): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteCategoriesByName(QString name)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("categories").where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::updateCategoriesByName(QString name, QString whereName)
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

QList<CategoriesModel *> Database::selectCategories()
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "name", "create_date", "update_date", "is_default").from("categories")
            .order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString name = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();
            int updateDate = mQuery.value(3).toInt();
            int isDefault = mQuery.value(4).toInt();

            CategoriesModel *categoriesModel = new CategoriesModel(id, name, createDate,
                                                                                  updateDate, isDefault);
            result.append(categoriesModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

CategoriesModel* Database::selectCategoriesById(int id)
{
    SelectModel selectBuilder;
    selectBuilder.select("name", "create_date", "update_date", "is_default").from("categories").where(column("id") == id);
    QString sql = QString::fromStdString(selectBuilder.str());

    CategoriesModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        QString name = mQuery.value(0).toString();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();
        int isDefault = mQuery.value(3).toInt();

        result = new CategoriesModel(id, name, createDate, updateDate, isDefault);
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

CategoriesModel *Database::selectCategoriesById(QString name)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "create_date", "update_date", "is_default").from("categories")
            .where(column("name") == name.toStdString());
    QString sql = QString::fromStdString(selectBuilder.str());

    CategoriesModel *result = nullptr;
    if (mQuery.exec(sql) && mQuery.first()) {
        int id = mQuery.value(0).toInt();
        int createDate = mQuery.value(1).toInt();
        int updateDate = mQuery.value(2).toInt();
        int isDefault = mQuery.value(3).toInt();

        result = new CategoriesModel(id, name, createDate, updateDate, isDefault);
    }
    else {
        qDebug() << "CategoriesModel *Database::selectCategoriesById(QString name): failed!";
    }

    return result;
}

int Database::insertNJT(QString notesUuid, int tagsId)
{
    InsertModel insertBuilder;
    insertBuilder.insert("tags_id", tagsId)
            ("notes_uuid", notesUuid.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch()).into("notes_join_tags");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertNJT(QString notesUuid, int tagsId): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteNJTByNotesUuid(QString notesUuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_tags").where(column("notes_uuid") == notesUuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::deleteNJTById(QString notesUuid, int tagsId)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_tags")
            .where(column("notes_uuid") == notesUuid.toStdString() and column("tags_id") == tagsId);
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

QList<TagsModel *>* Database::selectNJTByNotesUuid(QString notesUuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "tags_id", "create_date").from("notes_join_tags")
            .where(column("notes_uuid") == notesUuid.toStdString()).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagsModel *> *result = new QList<TagsModel *>();
    qDebug() << result->length();
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            int tagsId = mQuery.value(1).toInt();
            int createDate = mQuery.value(2).toInt();

            TagsModel *tagsModel = new TagsModel(id, tagsId, notesUuid, createDate);
            result->append(tagsModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

QList<TagsModel *> Database::selectNJTByTagsId(int tagsId)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "notes_Uuid", "create_date").from("notes_join_tags")
            .where(column("tags_id") == tagsId).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<TagsModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString notesUuid = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();

            TagsModel *tagsModel = new TagsModel(id, tagsId, notesUuid, createDate);
            result.append(tagsModel);
        }
    }
    else {
        qDebug() << "QList<TagsModel> Database::selectNJTByTagsId(int tagsId): failed!";
    }

    return result;
}


int Database::insertNJC(QString notesUuid, int categoriesId)
{
    InsertModel insertBuilder;
    insertBuilder.insert("categories_id", categoriesId)
            ("notes_uuid", notesUuid.toStdString())
            ("create_date", QDateTime::currentSecsSinceEpoch()).into("notes_join_categories");
    QString sql = QString::fromStdString(insertBuilder.str());

    int insertId = 0;
    if (!mQuery.exec(sql)) {
        qDebug() << "int Database::insertNJC(QString notesUuid, int categoriesId): Failed!";
    }
    else {
        insertId = mQuery.lastInsertId().toInt();
    }

    return insertId;
}

bool Database::deleteNJCByNotesUuid(QString notesUuid)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_categories").where(column("notes_uuid") == notesUuid.toStdString());
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

bool Database::deleteNJCById(QString notesUuid, int categoriesId)
{
    DeleteModel deleteBuilder;
    deleteBuilder._delete().from("notes_join_categories")
            .where(column("notes_uuid") == notesUuid.toStdString() and column("categories_id") == categoriesId);
    QString sql = QString::fromStdString(deleteBuilder.str());
    mQuery.exec(sql);

    return mQuery.numRowsAffected() != -1;
}

QList<CategoriesModel *> Database::selectNJCByNotesUuid(QString notesUuid)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "categories_id", "create_date").from("notes_join_categories")
            .where(column("notes_uuid") == notesUuid.toStdString()).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            int categoriesId = mQuery.value(1).toInt();
            int createDate = mQuery.value(2).toInt();

            CategoriesModel *tagsModel = new CategoriesModel(id, categoriesId, notesUuid, createDate);
            result.append(tagsModel);
        }
    }
    else {
        qDebug() << "QList<NoteModel *> Database::selectNoteList(): failed!";
    }

    return result;
}

QList<CategoriesModel *> Database::selectNJCByCategoriesId(int categoriesId)
{
    SelectModel selectBuilder;
    selectBuilder.select("id", "notes_uuid", "create_date").from("notes_join_categories")
            .where(column("categories_id") == categoriesId).order_by("create_date desc");
    QString sql = QString::fromStdString(selectBuilder.str());

    QList<CategoriesModel *> result;
    if (mQuery.exec(sql)) {
        while (mQuery.next()) {
            int id = mQuery.value(0).toInt();
            QString notesUuid = mQuery.value(1).toString();
            int createDate = mQuery.value(2).toInt();

            CategoriesModel *tagsModel = new CategoriesModel(id, categoriesId, notesUuid, createDate);
            result.append(tagsModel);
        }
    }
    else {
        qDebug() << "QList<TagsModel> Database::selectNJTByTagsId(int tagsId): failed!";
    }

    return result;
}
