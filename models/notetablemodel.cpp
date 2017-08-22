#include "notetablemodel.h"
#include "tools.h"

NoteTableModel::NoteTableModel(QString uuid, QString title, int insertDate)
{
    this->uuid = uuid;
    this->title = title;
    this->insertDate = insertDate;
}

NoteTableModel::NoteTableModel(QString uuid, QString title, int insertDate, int updateDate, QString body,
                               QString fileDir, QString fileName)
{
    this->uuid = uuid;
    this->title = title;
    this->insertDate = insertDate;
    this->updateDate = updateDate;
    this->body = body;
    this->fileDir = fileDir;
    this->fileName = fileName;
}

NoteTableModel::NoteTableModel(QString uuid, QString title, int insertDate, int updateDate, QString body)
{
    this->uuid = uuid;
    this->title = title;
    this->insertDate = insertDate;
    this->updateDate = updateDate;
    this->body = body;
}

QString NoteTableModel::setUuid() {
    return this->uuid;
}

void NoteTableModel::setTitle(const QString title)
{
    this->title = title;
}

void NoteTableModel::setCreateDate(const QString createDate)
{
    this->insertDate = Tools::timestampFromDateTime(createDate);
}

void NoteTableModel::setCreateDate(int createDate)
{
    this->insertDate = createDate;
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

void NoteTableModel::setFileDir(const QString fileDir)
{
    this->fileDir = fileDir;
}

void NoteTableModel::setFileName(const QString fileName)
{
    this->fileName = fileName;
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
    return this->insertDate;
}

int NoteTableModel::getUpdateDate()
{
    return this->updateDate;
}

QString NoteTableModel::getBody()
{
    return this->body;
}

QString NoteTableModel::getFileDir()
{
    return this->fileDir;
}

QString NoteTableModel::getFileName()
{
    return this->fileName;
}
