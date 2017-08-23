#include "notetablemodel.h"
#include "tools.h"

NoteTableModel::NoteTableModel(QString uuid, QString title, int createDate)
{
    this->uuid = uuid;
    this->title = title;
    this->createDate = createDate;
}

NoteTableModel::NoteTableModel(QString uuid, QString title, int createDate, int updateDate, QString body)
{
    this->uuid = uuid;
    this->title = title;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->body = body;
}

QString NoteTableModel::setUuid()
{
    return this->uuid;
}

void NoteTableModel::setTitle(const QString title)
{
    this->title = title;
}

void NoteTableModel::setCreateDate(const QString createDate)
{
    this->createDate = Tools::timestampFromDateTime(createDate);
}

void NoteTableModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

void NoteTableModel::setUpdateDate(const QString updateDate)
{
    this->updateDate = Tools::timestampFromDateTime(updateDate);
}

void NoteTableModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

void NoteTableModel::setBody(const QString body)
{
    this->body = body;
}

void NoteTableModel::setUuid(QString uuid) {
    this->uuid = uuid;
}

QString NoteTableModel::getTitle()
{
    return this->title;
}

int NoteTableModel::getCreateDate()
{
    return this->createDate;
}

int NoteTableModel::getUpdateDate()
{
    return this->updateDate;
}

QString NoteTableModel::getBody()
{
    return this->body;
}
