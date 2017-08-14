#include "notemodel.h"

#include <QDebug>

NoteModel::NoteModel(QString note)
{
    QStringList noteLines = note.replace("\r\n", "\n").split("\n");
    for (int i = 0; i < noteLines.length(); ++i) {
        if (i == 0) {
            this->title = QString(noteLines[i]).replace("title: ", "");
        }
        else if (i == 1) {
            this->createDate = this->timestampFromDateTime(QString(noteLines[i]).replace("createDate: ", ""));
        }
        else if (i == 2) {
            this->updateDate = this->timestampFromDateTime(QString(noteLines[i]).replace("updateDate: ", ""));
        }
        else if (i == 3) {
            this->tags = QString(noteLines[i]).replace("title: ", "").split(__TAGS_SPLITE__);
        }
        else if (i == 4) {
            this->categories = QString(noteLines[i]).replace("categories: ", "");
        }
        else if (i == 5 || i == 6 || i == 7) {
            continue;
        }
        else {
            this->body += noteLines[i];
            if (i != noteLines.length() - 1) {
                this->body += "\n";
            }
        }
    }
}

NoteModel::NoteModel(uint id, QString title, uint updateDate)
{
    this->id = id;
    this->title = title;
    this->updateDate = updateDate;
}

NoteModel::NoteModel(uint id, QString title, QString categories, QStringList tags,
          uint createDate, uint updateDate, QString body)
{
    this->id = id;
    this->title = title;
    this->categories = categories;
    this->tags = tags;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->body = body;
}

void NoteModel::setTitle(QString title)
{
    this->title = title;
}

void NoteModel::setCategories(QString categories)
{
    this->categories = categories;
}

void NoteModel::setTags(QList<QString> tags)
{
    this->tags = tags;
}

void NoteModel::setTags(QString tags)
{
    this->tags = QString(tags).split(__TAGS_SPLITE__);
}

void NoteModel::setCreateDate(QString createDate)
{
    this->createDate = this->timestampFromDateTime(createDate);
}

void NoteModel::setCreateDate(uint createDate)
{
    this->createDate = createDate;
}

void NoteModel::setUpdateDate(QString updateDate)
{
    this->updateDate = this->timestampFromDateTime(updateDate);
}

void NoteModel::setBody(QString body)
{
    this->body = body;
}

QString NoteModel::getTitle()
{
    return this->title;
}

uint NoteModel::getCreateDate()
{
    return this->createDate;
}

uint NoteModel::getUpdateDate()
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

//QString NoteModel::getTags()
//{
//    QString tags = "";
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
    note += "createDate: " + this->timestampToDateTime(this->createDate) + "\n";
    note += "updateDate" + this->timestampToDateTime(this->updateDate) + "\n";
    note += "categories: " + this->categories + "\n";

    note += "notes: ";
    for (int i = 0; i < this->tags.length(); ++i) {
        note += this->tags[i];
        if (i != this->tags.length() - 1) {
            note += "\n";
        }
    }
    note += "\n\n---\n\n" + this->body;

    return note;
}

uint NoteModel::timestampFromDateTime(QString dateTime)
{
    return QDateTime::fromString(dateTime, __DATE_FORMAT__).toTime_t();
}

QString NoteModel::timestampToDateTime(uint timestamp)
{
    return QDateTime::fromTime_t(timestamp).toString(__DATE_FORMAT__);
}
