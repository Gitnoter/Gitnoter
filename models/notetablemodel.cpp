#include "notetablemodel.h"
#include "tools.h"
#include "globals.h"

#include <QDateTime>

NoteTableModel::NoteTableModel(QString uuid, QString title, int createDate, int updateDate, QString body, QString filePath)
{
    this->setUuid(uuid);
    this->setTitle(title);
    this->setCreateDate(createDate);
    this->setUpdateDate(updateDate);
    this->setFilePath(filePath);
    this->body = body;
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
    this->createDate = createDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                            : Tools::timestampFromDateTime(createDate);
}

void NoteTableModel::setCreateDate(int createDate)
{
    this->createDate = createDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : createDate;
}

void NoteTableModel::setUpdateDate(const QString updateDate)
{
    this->updateDate = updateDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                            : Tools::timestampFromDateTime(updateDate);
}

void NoteTableModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : updateDate;
}

void NoteTableModel::setBody(const QString body)
{
    this->body = body;
}

void NoteTableModel::setUuid(QString uuid) {
    this->uuid = uuid.isEmpty() ? Tools::getUuid() : uuid;
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
    NoteTableModel::filePath = filePath.isEmpty() ?
                               QString("%1/%2/%3-%4.md").arg(
                                       g_repoPath, g_noteFolderName,
                                       this->title.isEmpty() ? "无标题" : this->title, this->uuid)
                                                  : filePath;
}
