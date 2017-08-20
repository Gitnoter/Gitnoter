#include "notemodel.h"

#include <QDebug>

NoteModel::NoteModel(QString noteText)
{
    QTextStream in(&noteText);
    uint i = 0;
    while(!in.atEnd()) {
        QString noteLine = in.readLine();

        if (i == 0) {
            this->uuid = noteLine.mid(6);
        }
        else if (i == 1) {
            this->title = noteLine.replace("title: ", "");
        }
        else if (i == 2) {
            this->createDate = this->timestampFromDateTime(noteLine.replace("createDate: ", ""));
        }
        else if (i == 3) {
            this->updateDate = this->timestampFromDateTime(noteLine.replace("updateDate: ", ""));
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

void NoteModel::setId(uint id)
{
    this->id = id;
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

void NoteModel::setFileDir(QString fileDir)
{
    this->fileDir = fileDir;
}

void NoteModel::setFileName(QString fileName)
{
    this->fileName = fileName;
}

uint NoteModel::getId()
{
    return this->id;
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

QString NoteModel::getFileDir()
{
    return this->fileDir;
}

QString NoteModel::getFileName()
{
    return this->fileName;
}

uint NoteModel::timestampFromDateTime(QString dateTime)
{
    return QDateTime::fromString(dateTime, __DATE_FORMAT__).toTime_t();
}

QString NoteModel::timestampToDateTime(uint timestamp)
{
    return QDateTime::fromTime_t(timestamp).toString(__DATE_FORMAT__);
}
