#include "notemodel.h"
#include "tools.h"

#include <QDebug>

NoteModel::NoteModel(QString noteText)
{
    QTextStream in(&noteText);
    int i = 0;
    while(!in.atEnd()) {
        QString noteLine = in.readLine();

        if (i == 0) {
            this->uuid = noteLine.mid(6);
        }
        else if (i == 1) {
            this->title = noteLine.replace("title: ", "");
        }
        else if (i == 2) {
            this->createDate = Tools::timestampFromDateTime(noteLine.replace("createDate: ", ""));
        }
        else if (i == 3) {
            this->updateDate = Tools::timestampFromDateTime(noteLine.replace("updateDate: ", ""));
        }
        else if (i == 4) {
            this->tags = noteLine.replace("title: ", "").split(__TAGS_SPLITE__);
        }
        else if (i == 5) {
            this->categories = noteLine.replace("categories: ", "");
        }
        else if (i == 6 || i == 7 || i == 8) {

        }
        else {
            this->body += noteLine += "\n";
        }
        i += 1;
    }
    this->body.chop(2);
}

NoteModel::NoteModel(QString uuid, const QString title, int updateDate)
{
    this->uuid = uuid;
    this->title = title;
    this->updateDate = updateDate;
}

NoteModel::NoteModel(QString uuid, const QString title, const QString categories, const QStringList tags,
          int createDate, int updateDate, const QString body)
{
    this->uuid = uuid;
    this->title = title;
    this->categories = categories;
    this->tags = tags;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->body = body;
}

QString NoteModel::setUuid() {
    return this->uuid;
}

void NoteModel::setTitle(const QString title)
{
    this->title = title;
}

void NoteModel::setCategories(const QString categories)
{
    this->categories = categories;
}

void NoteModel::setTags(const QStringList tags)
{
    this->tags = tags;
}

void NoteModel::setTags(const QString tags)
{
    this->tags = tags.split(__TAGS_SPLITE__);
}

void NoteModel::setCreateDate(const QString createDate)
{
    this->createDate = Tools::timestampFromDateTime(createDate);
}

void NoteModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

void NoteModel::setUpdateDate(const QString updateDate)
{
    this->updateDate = Tools::timestampFromDateTime(updateDate);
}

void NoteModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

void NoteModel::setBody(const QString body)
{
    this->body = body;
}

void NoteModel::setFileDir(const QString fileDir)
{
    this->fileDir = fileDir;
}

void NoteModel::setFileName(const QString fileName)
{
    this->fileName = fileName;
}

void NoteModel::setUuid(QString uuid) {
    this->uuid = uuid;
}

QString NoteModel::getTitle()
{
    return this->title;
}

int NoteModel::getCreateDate()
{
    return this->createDate;
}

int NoteModel::getUpdateDate()
{
    return this->updateDate;
}

QString NoteModel::getCategories()
{
    return this->categories;
}

QStringList NoteModel::getTags()
{
    return this->tags;
}

//const QString NoteModel::getTags()
//{
//    const QString tags = "";
//    for (int i = 0; i < this->tags.length(); ++i) {
//        tags += tags[i];
//        if (i != tags.length() - 1) {
//            tags += ", ";
//        }
//    }
//    return tags;
//}

QString NoteModel::getBody()
{
    return this->body;
}

QString NoteModel::getNote()
{
    QString note;
    note += "title: " + this->title + "\n";
    note += "createDate: " + Tools::timestampToDateTime(this->createDate) + "\n";
    note += "updateDate" + Tools::timestampToDateTime(this->updateDate) + "\n";
    note += "categories: " + this->categories + "\n";

    note += "notes: ";
    for (auto &&tag : this->tags) {
        note += tag + "\n";
    }
    note.chop(2);

    note += "\n\n---\n\n" + this->body;

    return note;
}

QString NoteModel::getFileDir()
{
    return this->fileDir;
}

QString NoteModel::getFileName()
{
    return this->fileName;
}
