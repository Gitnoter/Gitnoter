#include "notetablemodel.h"
#include "tools.h"

NoteTableModel::NoteTableModel(QString uuid, QString title, int createDate, int updateDate, QString body, QString filePath)
{
    this->uuid = uuid;
    this->title = title;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->body = body;
    this->filePath = filePath;
}

QString NoteTableModel::getUuid()
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

const QString &NoteTableModel::getFilePath() const
{
    return filePath;
}

void NoteTableModel::setFilePath(const QString &filePath)
{
    NoteTableModel::filePath = filePath;
}
