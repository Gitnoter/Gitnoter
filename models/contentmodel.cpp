#include "contentmodel.h"
#include "tools.h"
#include "globals.h"

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
    QString aTitle = title.isEmpty() ? tr("无标题") : title;
    if (!mTitle.isEmpty() && mTitle != aTitle) {
        mTitle = aTitle;
        setFilePath();
    }
    else {
        mTitle = aTitle;
    }
}

void ContentModel::setCreateDate(const QString createDate)
{
    mCreateDate = createDate.isEmpty() ? (int) QDateTime::currentMSecsSinceEpoch() / 1000
                                       : Tools::timestampFromDateTime(createDate);
}

void ContentModel::setCreateDate(int createDate)
{
    mCreateDate = createDate == 0 ? (int) QDateTime::currentMSecsSinceEpoch() / 1000 : createDate;
}

void ContentModel::setUpdateDate(const QString updateDate)
{
    mUpdateDate = updateDate.isEmpty() ? (int) QDateTime::currentMSecsSinceEpoch() / 1000
                                       : Tools::timestampFromDateTime(updateDate);
}

void ContentModel::setUpdateDate(int updateDate)
{
    mUpdateDate = updateDate == 0 ? (int) QDateTime::currentMSecsSinceEpoch() / 1000 : updateDate;
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
    QString aFilePath = filePath.isEmpty() ? QString("%1/%2-%3.md").arg(Global::repoNotePath, mTitle, mUuid) : filePath;
    if (!mFilePath.isEmpty() && aFilePath != mFilePath) {
        QFile(mFilePath).rename(aFilePath);
    }
    mFilePath = aFilePath;
}
