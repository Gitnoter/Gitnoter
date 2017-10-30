#include "notemodel.h"
#include "tools.h"
#include "globals.h"

#include <json.h>

NoteModel::NoteModel(const QString noteText)
{
    clear();
    setNoteText(noteText);
}

NoteModel::NoteModel(const QString textPath, const QString dataPath)
{
    setNoteText(Tools::readerFile(textPath));
    setNoteData(Tools::readerFile(dataPath));
}

const QString & NoteModel::getFileDir()
{
    if (mUuid.isEmpty()) {
        setUuid();
    }

    return QDir(Global::repoNotePath).filePath(getUuid());
}

QString NoteModel::getDisplayNote() const
{
    return "";
}

QString NoteModel::getTagsString() const
{
    QString tagsString;
    for (auto &&item : tagList) {
        tagsString += item + Global::tagSplit;
    }
    tagsString.chop(Global::tagSplit.length());

    return tagsString;
}

void NoteModel::setNoteData(const QString &noteData)
{
    QtJson::JsonObject result = QtJson::parse(noteData).toMap();

    mUuid = result["uuid"].toString();
    mCreateDate = result["createDate"].toInt();
    mUpdateDate = result["updateDate"].toInt();
    category = result["category"].toString();

    tagList.clear();
    QtJson::JsonArray tagListArray = result["tagList"].toList();
    for (int i = 0; i < tagListArray.length(); ++i) {
        tagList.append(tagListArray[i].toString());
    }
}

QString NoteModel::getNoteData() const
{
    QtJson::JsonObject contributor;

    contributor["uuid"] = mUuid;
    contributor["createDate"] = mCreateDate;
    contributor["updateDate"] = mUpdateDate;
    contributor["category"] = category;

    QtJson::JsonArray tagListArray;
    for (int i = 0; i < tagList.length(); ++i) {
        tagListArray.append(tagList[i]);
    }
    contributor["tagList"] = tagListArray;

    return QString(QtJson::serialize(contributor));
}

void NoteModel::clear()
{
    setUuid();
    setNoteText();
    setCreateDate(0);
    setUpdateDate(0);
    setTagList();
    setCategory();
}

void NoteModel::writerLocal()
{
    QString path = getFileDir();

    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(Global::noteTextFileName), getNoteText());
    Tools::writerFile(QDir(path).filePath(Global::noteDataFileName), getNoteData());
}

QString NoteModel::getUuid()
{
    return mUuid;
}

void NoteModel::setCreateDate(const QString createDate)
{
    mCreateDate = createDate.isEmpty() ? (int) QDateTime::currentMSecsSinceEpoch() / 1000
                                       : Tools::timestampFromDateTime(createDate);
}

void NoteModel::setCreateDate(int createDate)
{
    mCreateDate = createDate == 0 ? (int) QDateTime::currentMSecsSinceEpoch() / 1000 : createDate;
}

void NoteModel::setUpdateDate(const QString updateDate)
{
    mUpdateDate = updateDate.isEmpty() ? (int) QDateTime::currentMSecsSinceEpoch() / 1000
                                       : Tools::timestampFromDateTime(updateDate);
}

void NoteModel::setUpdateDate(int updateDate)
{
    mUpdateDate = updateDate == 0 ? (int) QDateTime::currentMSecsSinceEpoch() / 1000 : updateDate;
}

void NoteModel::setNoteText(QString body)
{
    mBody = body;
}

void NoteModel::setUuid(QString uuid)
{
    mUuid = uuid.isEmpty() ? Tools::getUuid() : uuid;
}

int NoteModel::getCreateDate()
{
    return mCreateDate;
}

int NoteModel::getUpdateDate()
{
    return mUpdateDate;
}

QString NoteModel::getNoteText()
{
    return mBody;
}

const QStringList &NoteModel::getTagList() const
{
    return tagList;
}

void NoteModel::setTagList(const QStringList &tagList)
{
    NoteModel::tagList = tagList;
}

const QString &NoteModel::getCategory() const
{
    return category;
}

void NoteModel::setCategory(const QString &category)
{
    NoteModel::category = category;
}
