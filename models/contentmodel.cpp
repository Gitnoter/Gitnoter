#include "contentmodel.h"
#include "tools.h"
#include "globals.h"

#include <QDateTime>

ContentModel::ContentModel(QString uuid, QString title, int createDate, int updateDate, QString body, QString filePath)
{
    this->setUuid(uuid);
    this->setTitle(title);
    this->setCreateDate(createDate);
    this->setUpdateDate(updateDate);
    this->setFilePath(filePath);
    this->body = body;
}

QString ContentModel::getUuid()
{
    return this->uuid;
}

void ContentModel::setTitle(const QString title)
{
    this->title = title;
}

void ContentModel::setCreateDate(const QString createDate)
{
    this->createDate = createDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                            : Tools::timestampFromDateTime(createDate);
}

void ContentModel::setCreateDate(int createDate)
{
    this->createDate = createDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : createDate;
}

void ContentModel::setUpdateDate(const QString updateDate)
{
    this->updateDate = updateDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                            : Tools::timestampFromDateTime(updateDate);
}

void ContentModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : updateDate;
}

void ContentModel::setBody(const QString body)
{
    this->body = body;
}

void ContentModel::setUuid(QString uuid) {
    this->uuid = uuid.isEmpty() ? Tools::getUuid() : uuid;
}

QString ContentModel::getTitle()
{
    return this->title;
}

int ContentModel::getCreateDate()
{
    return this->createDate;
}

int ContentModel::getUpdateDate()
{
    return this->updateDate;
}

QString ContentModel::getBody()
{
    return this->body;
}

const QString &ContentModel::getFilePath() const
{
    return filePath;
}

void ContentModel::setFilePath(const QString &filePath)
{
    ContentModel::filePath = filePath.isEmpty() ?
                               QString("%1/%2/%3-%4.md").arg(
                                       gRepoPath, gNoteFolderName,
                                       this->title.isEmpty() ? "无标题" : this->title, this->uuid)
                                                  : filePath;
}
