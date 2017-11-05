#include "tagmodellist.h"
#include "globals.h"
#include "tools.h"

TagModel::TagModel(const QString &name, int count)
{
    TagModel::mName = name;
    TagModel::mCount = count;
}

const QString &TagModel::getName() const
{
    return mName;
}

void TagModel::setName(const QString &name)
{
    TagModel::mName = name;
}

int TagModel::getCount() const
{
    return mCount;
}

void TagModel::setCount(int count)
{
    TagModel::mCount = count;
}

/**
 *
 * TagModelList
 *
 */

void TagModelList::init()
{
    if (!Globals::noteModelList.getList().length()) {
        return;
    }

    tagModelList.clear();
    QMap<QString, TagModel *> map;
    for (auto &&noteModel : Globals::noteModelList.getList()) {
        for (auto &&item : noteModel->getTagList()) {
            if (map.contains(item)) {
                TagModel *tagModel = map[item];
                tagModel->setCount(tagModel->getCount() + 1);
            }
            else {
                map[item] = new TagModel(item, 1);
            }
        }
    }

    QStringList stringList = Tools::readerFileToList(Globals::repoTagListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new TagModel(str);
        }
    }

    tagModelList = map.values();
}

TagModel *TagModelList::append(const QString tagName)
{
    if (tagName.isEmpty()) {
        return nullptr;
    }

    for (auto &&tagsModel : tagModelList) {
        if (tagsModel->getName() == tagName) {
            tagsModel->setCount(tagsModel->getCount() + 1);
            toString();
            return tagsModel;
        }
    }

    TagModel *tagModel = new TagModel(tagName, 1);
    tagModelList.append(tagModel);
    return tagModel;
}

int TagModelList::indexOf(const QString &name)
{
    for (int i = 0; i < tagModelList.length(); ++i) {
        if (tagModelList[i]->getName() == name) {
            return i;
        }
    }

    return -1;
}

void TagModelList::rename(const QString oldName, const QString newName)
{
    for (auto &&item : Globals::noteModelList.getList()) {
        QStringList tagList = item->getTagList();
        for (int i = 0; i < tagList.length(); ++i) {
            if (tagList[i] == oldName) {
                tagList[i] = newName;
                item->saveNoteDataToLocal();
                break;
            }
        }
    }
}

void TagModelList::removeOne(const QString &name)
{
    for (auto &&item : tagModelList) {
        if (item->getName() == name) {
            item->setCount(item->getCount() - 1);
        }
    }
}

void TagModelList::removeList(const QStringList &nameList)
{
    for (auto &&name : nameList) {
        removeOne(name);
    }
}

void TagModelList::removeAt(int index)
{
    tagModelList.removeAt(index);
}

const QString TagModelList::toString()
{
    QString str = "";
    for (auto &&item : tagModelList) {
        str += item->getName() + "\n";
    }
    return str.trimmed();
}

void TagModelList::saveToLocal()
{
    Tools::writerFile(Globals::repoTagListPath, toString());
}

const QList<TagModel *> &TagModelList::getList() const
{
    return tagModelList;
}

void TagModelList::setList(const QList<TagModel *> &tagModelList)
{
    TagModelList::tagModelList = tagModelList;
}
