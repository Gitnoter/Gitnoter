#include "contentmodel.h"
#include "tools.h"
#include "globals.h"

#include <QDateTime>

ContentModel::ContentModel(QString uuid, QString title, int createDate, int updateDate, QString body, QString filePath)
{
    setUuid(uuid);
    setTitle(title);
    setCreateDate(createDate);
    setUpdateDate(updateDate);
    setFilePath(filePath);
    mBody = body;
}

QString ContentModel::getUuid()
{
    return mUuid;
}

void ContentModel::setTitle(const QString title)
{
    mTitle = title;
}

void ContentModel::setCreateDate(const QString createDate)
{
    mCreateDate = createDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                       : Tools::timestampFromDateTime(createDate);
}

void ContentModel::setCreateDate(int createDate)
{
    mCreateDate = createDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : createDate;
}

void ContentModel::setUpdateDate(const QString updateDate)
{
    mUpdateDate = updateDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                       : Tools::timestampFromDateTime(updateDate);
}

void ContentModel::setUpdateDate(int updateDate)
{
    mUpdateDate = updateDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : updateDate;
}

void ContentModel::setBody(const QString body)
{
    mBody = body;
}

void ContentModel::setUuid(QString uuid)
{
    mUuid = uuid.isEmpty() ? Tools::getUuid() : uuid;
}

QString ContentModel::getTitle()
{
    return mTitle;
}

int ContentModel::getCreateDate()
{
    return mCreateDate;
}

int ContentModel::getUpdateDate()
{
    return mUpdateDate;
}

QString ContentModel::getBody()
{
    return mBody;
}

const QString &ContentModel::getFilePath() const
{
    return mFilePath;
}

void ContentModel::setFilePath(const QString &filePath)
{
    mFilePath = filePath.isEmpty() ? QString("%1/%2-%3.md").arg(Global::repoNotePath, mTitle.isEmpty() ? "无标题" : mTitle, mUuid)
                                   : filePath;
}
