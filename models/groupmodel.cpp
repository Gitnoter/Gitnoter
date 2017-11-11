#include "groupmodel.h"
#include "tools.h"
#include "globals.h"

#include <QDateTime>
#include <QDebug>

GroupModel::GroupModel(Gitnoter::GroupType type, const QString &name, int count)
{
    mType = type;
    mName = name;
    mCount = count;
}

const QString &GroupModel::getName() const
{
    return mName;
}

void GroupModel::setName(const QString &name)
{
    mName = name;
}

int GroupModel::getCount() const
{
    return mCount;
}

void GroupModel::setCount(int count)
{
    mCount = count;
}

Gitnoter::GroupType GroupModel::getType() const
{
    return mType;
}

void GroupModel::setType(Gitnoter::GroupType type)
{
    mType = type;
}


void _appendToGroupModelMap(QMap<QString, GroupModel *> &map, Gitnoter::GroupType type, QString name, int count) {
    if (map.contains(name)) {
        GroupModel *model = map[name];
        model->setCount(model->getCount() + count);
    }
    else {
        map[name] = new GroupModel(type, name, count);
    }
}
void GroupModel::init(QTreeWidget *treeWidget, QList<NoteModel *> noteModelList)
{
    QMap<QString, GroupModel *> categoryModelMap = {};
    QMap<QString, GroupModel *> tagModelMap = {};
    QMap<QString, GroupModel *> systemModelMap = {};
    QStringList stringList = {};

    _appendToGroupModelMap(systemModelMap, Gitnoter::All, treeWidget->topLevelItem(1)->text(0), 0);
    _appendToGroupModelMap(systemModelMap, Gitnoter::Recent, treeWidget->topLevelItem(2)->text(0), 0);
    _appendToGroupModelMap(systemModelMap, Gitnoter::Unclassified, treeWidget->topLevelItem(3)->text(0), 0);
    _appendToGroupModelMap(systemModelMap, Gitnoter::Trash, treeWidget->topLevelItem(4)->text(0), 0);

    for (auto &&noteModel : noteModelList) {
        if (!noteModel->getIsDelete()) {
            _appendToGroupModelMap(systemModelMap, Gitnoter::All, treeWidget->topLevelItem(1)->text(0), 1);

            if (noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) {
                _appendToGroupModelMap(systemModelMap, Gitnoter::Recent, treeWidget->topLevelItem(2)->text(0), 1);
            }

            if (noteModel->getCategory().isEmpty()) {
                _appendToGroupModelMap(systemModelMap, Gitnoter::Unclassified, treeWidget->topLevelItem(3)->text(0), 1);
            }
            else {
                _appendToGroupModelMap(categoryModelMap, Gitnoter::Category, noteModel->getCategory(), 1);
            }

            for (auto &&item : noteModel->getTagList()) {
                _appendToGroupModelMap(tagModelMap, Gitnoter::Tag, item, 1);
            }
        }
        else {
            _appendToGroupModelMap(systemModelMap, Gitnoter::Trash, treeWidget->topLevelItem(4)->text(0), 1);
        }
    }

    stringList = Tools::readerFileToList(Globals::repoCategoryListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty()) {
            _appendToGroupModelMap(categoryModelMap, Gitnoter::Category, str, 0);
        }
    }

    stringList = Tools::readerFileToList(Globals::repoTagListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty()) {
            _appendToGroupModelMap(tagModelMap, Gitnoter::Tag, str, 0);
        }
    }

    for (auto &&modelMap : categoryModelMap) {
        if (modelMap->getName().isEmpty()) {
            continue;
        }
        append(treeWidget, modelMap);
    }

    for (auto &&modelMap : tagModelMap) {
        if (modelMap->getName().isEmpty()) {
            continue;
        }
        append(treeWidget, modelMap);
    }

    for (auto iterator = systemModelMap.begin(); iterator != systemModelMap.end(); ++iterator) {
        if (iterator.key() == treeWidget->topLevelItem(1)->text(0)) {
            treeWidget->topLevelItem(1)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
        else if (iterator.key() == treeWidget->topLevelItem(2)->text(0)) {
            treeWidget->topLevelItem(2)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
        else if (iterator.key() == treeWidget->topLevelItem(3)->text(0)) {
            treeWidget->topLevelItem(3)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
        else if (iterator.key() == treeWidget->topLevelItem(4)->text(0)) {
            treeWidget->topLevelItem(4)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
    }
}

GroupModel *GroupModel::append(QTreeWidget *treeWidget, GroupModel *groupModel)
{
    QTreeWidgetItem *treeWidgetItem = treeWidget->topLevelItem(groupModel->getType());

    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        GroupModel *childModel = treeWidgetItem->child(i)->data(0, Qt::UserRole).value<GroupModel *>();
        if (childModel->getName() == groupModel->getName()) {
            childModel->setCount(childModel->getCount() + 1);
            return childModel;
        }
    }

    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(0, groupModel->getName());
    childItem->setData(0, Qt::UserRole, QVariant::fromValue(groupModel));
    treeWidgetItem->addChild(childItem);
    treeWidgetItem->sortChildren(0, Qt::AscendingOrder);

    return groupModel;
}

void GroupModel::removeOne(QTreeWidget *treeWidget, GroupModel *groupModel)
{
    removeOne(treeWidget, groupModel->getType(), groupModel->getName());
}

void GroupModel::removeOne(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text)
{
    QTreeWidgetItem *treeWidgetItem = find(treeWidget, type, text);
    treeWidget->topLevelItem(type)->removeChild(treeWidgetItem);
}

QTreeWidgetItem *GroupModel::find(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text)
{
    if (type <= Gitnoter::Trash) {
        return treeWidget->topLevelItem(type);
    }

    QTreeWidgetItem *topLevelItem = treeWidget->topLevelItem(type);
    for (int i = 0; i < topLevelItem->childCount(); ++i) {
        QTreeWidgetItem *childItem = topLevelItem->child(i);
        if (childItem->text(0) == text) {
            return childItem;
        }
    }

    return nullptr;
}

QTreeWidgetItem *GroupModel::find(QTreeWidget *treeWidget, GroupModel &groupModel)
{
    return find(treeWidget, groupModel.getType(), groupModel.getName());
}

GroupModel *GroupModel::getGroupModel(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text)
{
    QTreeWidgetItem *treeWidgetItem = find(treeWidget, type, text);

    if (treeWidgetItem) {
        return treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>();
    }

    return nullptr;
}

void GroupModel::appendAny(GroupModel *groupModel, int num)
{
    groupModel->setCount(groupModel->getCount() + num);
}

void GroupModel::appendOne(QTreeWidget *treeWidget, NoteModel *noteModel, int num)
{
    QTreeWidgetItem *treeWidgetItem = nullptr;
    if (!noteModel->getIsDelete()) {
        treeWidgetItem = find(treeWidget, Gitnoter::All, noteModel->getCategory());
        if (treeWidgetItem) {
            appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
        }

        if (noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) {
            treeWidgetItem = find(treeWidget, Gitnoter::Recent, noteModel->getCategory());
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }

        if (noteModel->getCategory().isEmpty()) {
            treeWidgetItem = find(treeWidget, Gitnoter::Unclassified, noteModel->getCategory());
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }
        else {
            treeWidgetItem = find(treeWidget, Gitnoter::Category, noteModel->getCategory());
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }

        for (auto &&tag : noteModel->getTagList()) {
            treeWidgetItem = find(treeWidget, Gitnoter::Tag, tag);
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }
    }
    else {
        treeWidgetItem = find(treeWidget, Gitnoter::Trash, noteModel->getCategory());
        if (treeWidgetItem) {
            appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
        }
    }
}

QString GroupModel::toString(QTreeWidget *treeWidget, Gitnoter::GroupType type)
{
    QString text = "";

    QTreeWidgetItem *topLevelItem = treeWidget->topLevelItem(type);
    for (int i = 0; i < topLevelItem->childCount(); ++i) {
        text += topLevelItem->child(i)->text(0) + "\n";
    }

    return text.trimmed();
}

void GroupModel::setItemSelected(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text)
{
    treeWidget->clearSelection();
    if (type <= Gitnoter::Trash) {
        treeWidget->topLevelItem(type)->setSelected(true);
        return;
    }

    if(type == Gitnoter::Category) {
        QTreeWidgetItem *treeWidgetItem = treeWidget->topLevelItem(6);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            if (childItem->text(0) == text) {
                return childItem->setSelected(true);
            }
        }
    }

    if (type == Gitnoter::Tag) {
        QTreeWidgetItem *treeWidgetItem = treeWidget->topLevelItem(8);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            if (childItem->text(0) == text) {
                return childItem->setSelected(true);
            }
        }
    }
}

QList<GroupModel *> &GroupModel::getGroupModelList(QTreeWidget *treeWidget, Gitnoter::GroupType type)
{
    QList<GroupModel *> groupModelList = {};
    QTreeWidgetItem *treeWidgetItem = nullptr;
    if(type == Gitnoter::Category) {
        treeWidgetItem = treeWidget->topLevelItem(6);
    }
    else if (type == Gitnoter::Tag) {
        treeWidgetItem = treeWidget->topLevelItem(8);
    }

    if (!treeWidgetItem) {
        return groupModelList;
    }

    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        groupModelList.append(treeWidgetItem->child(i)->data(0, Qt::UserRole).value<GroupModel *>());
    }

    return groupModelList;
}
