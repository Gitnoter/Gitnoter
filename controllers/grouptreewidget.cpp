#include "grouptreewidget.h"
#include "globals.h"
#include "tools.h"

#include <QDate>

GroupTreeWidget::GroupTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{

}

GroupTreeWidget::~GroupTreeWidget()
{

}

void GroupTreeWidget::appendToGroupModelMap(QMap<QString, GroupModel *> &map, Gitnoter::GroupType type, QString name,
                                            int count) {
    if (map.contains(name)) {
        GroupModel *model = map[name];
        model->setCount(model->getCount() + count);
    }
    else {
        map[name] = new GroupModel(type, name, count);
    }
}

void GroupTreeWidget::init(QList<NoteModel *> noteModelList)
{
    QMap<QString, GroupModel *> categoryModelMap = {};
    QMap<QString, GroupModel *> tagModelMap = {};
    QMap<QString, GroupModel *> systemModelMap = {};
    QStringList stringList = {};

    appendToGroupModelMap(systemModelMap, Gitnoter::All, topLevelItem(1)->text(0), 0);
    appendToGroupModelMap(systemModelMap, Gitnoter::Recent, topLevelItem(2)->text(0), 0);
    appendToGroupModelMap(systemModelMap, Gitnoter::Unclassified, topLevelItem(3)->text(0), 0);
    appendToGroupModelMap(systemModelMap, Gitnoter::Trash, topLevelItem(4)->text(0), 0);

    for (auto &&noteModel : noteModelList) {
        if (!noteModel->getIsDelete()) {
            appendToGroupModelMap(systemModelMap, Gitnoter::All, topLevelItem(1)->text(0), 1);

            if (noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) {
                appendToGroupModelMap(systemModelMap, Gitnoter::Recent, topLevelItem(2)->text(0), 1);
            }

            if (noteModel->getCategory().isEmpty()) {
                appendToGroupModelMap(systemModelMap, Gitnoter::Unclassified, topLevelItem(3)->text(0), 1);
            }
            else {
                appendToGroupModelMap(categoryModelMap, Gitnoter::Category, noteModel->getCategory(), 1);
            }

            for (auto &&item : noteModel->getTagList()) {
                appendToGroupModelMap(tagModelMap, Gitnoter::Tag, item, 1);
            }
        }
        else {
            appendToGroupModelMap(systemModelMap, Gitnoter::Trash, topLevelItem(4)->text(0), 1);
        }
    }

    stringList = Tools::readerFileToList(Globals::repoCategoryListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty()) {
            appendToGroupModelMap(categoryModelMap, Gitnoter::Category, str, 0);
        }
    }

    stringList = Tools::readerFileToList(Globals::repoTagListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty()) {
            appendToGroupModelMap(tagModelMap, Gitnoter::Tag, str, 0);
        }
    }

    for (auto &&modelMap : categoryModelMap) {
        if (modelMap->getName().isEmpty()) {
            continue;
        }
        append(modelMap);
    }

    for (auto &&modelMap : tagModelMap) {
        if (modelMap->getName().isEmpty()) {
            continue;
        }
        append(modelMap);
    }

    for (auto iterator = systemModelMap.begin(); iterator != systemModelMap.end(); ++iterator) {
        if (iterator.key() == topLevelItem(1)->text(0)) {
            topLevelItem(1)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
        else if (iterator.key() == topLevelItem(2)->text(0)) {
            topLevelItem(2)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
        else if (iterator.key() == topLevelItem(3)->text(0)) {
            topLevelItem(3)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
        else if (iterator.key() == topLevelItem(4)->text(0)) {
            topLevelItem(4)->setData(0, Qt::UserRole, QVariant::fromValue(iterator.value()));
        }
    }
}

GroupModel *GroupTreeWidget::append(GroupModel *groupModel)
{
    QTreeWidgetItem *treeWidgetItem = topLevelItem(groupModel->getType());

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

void GroupTreeWidget::removeOne(GroupModel *groupModel)
{
    removeOne(groupModel->getType(), groupModel->getName());
}

void GroupTreeWidget::removeOne(Gitnoter::GroupType type, const QString &text)
{
    QTreeWidgetItem *treeWidgetItem = getTreeWidgetItem(type, text);
    topLevelItem(type)->removeChild(treeWidgetItem);
}

QTreeWidgetItem *GroupTreeWidget::getTreeWidgetItem(Gitnoter::GroupType type, const QString &text)
{
    if (type <= Gitnoter::Trash) {
        return topLevelItem(type);
    }

    QTreeWidgetItem *treeWidgetItem = topLevelItem((int) type);
    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        QTreeWidgetItem *childItem = treeWidgetItem->child(i);
        if (childItem->text(0) == text) {
            return childItem;
        }
    }

    return nullptr;
}

QTreeWidgetItem *GroupTreeWidget::getTreeWidgetItem(GroupModel &groupModel)
{
    return getTreeWidgetItem(groupModel.getType(), groupModel.getName());
}

GroupModel *GroupTreeWidget::getGroupModel(Gitnoter::GroupType type, const QString &text)
{
    QTreeWidgetItem *treeWidgetItem = getTreeWidgetItem(type, text);

    if (treeWidgetItem) {
        return treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>();
    }

    return nullptr;
}

void GroupTreeWidget::appendAny(GroupModel *groupModel, int num)
{
    groupModel->setCount(groupModel->getCount() + num);
}

void GroupTreeWidget::appendOne(NoteModel *noteModel, int num)
{
    QTreeWidgetItem *treeWidgetItem = nullptr;
    if (!noteModel->getIsDelete()) {
        treeWidgetItem = getTreeWidgetItem(Gitnoter::All, noteModel->getCategory());
        if (treeWidgetItem) {
            appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
        }

        if (noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) {
            treeWidgetItem = getTreeWidgetItem(Gitnoter::Recent, noteModel->getCategory());
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }

        if (noteModel->getCategory().isEmpty()) {
            treeWidgetItem = getTreeWidgetItem(Gitnoter::Unclassified, noteModel->getCategory());
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }
        else {
            treeWidgetItem = getTreeWidgetItem(Gitnoter::Category, noteModel->getCategory());
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }

        for (auto &&tag : noteModel->getTagList()) {
            treeWidgetItem = getTreeWidgetItem(Gitnoter::Tag, tag);
            if (treeWidgetItem) {
                appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
            }
        }
    }
    else {
        treeWidgetItem = getTreeWidgetItem(Gitnoter::Trash, noteModel->getCategory());
        if (treeWidgetItem) {
            appendAny(treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>(), num);
        }
    }
}

QString GroupTreeWidget::toString(Gitnoter::GroupType type)
{
    QString text = "";

    QTreeWidgetItem *treeWidgetItem = topLevelItem(type);
    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        text += treeWidgetItem->child(i)->text(0) + "\n";
    }

    return text.trimmed();
}

void GroupTreeWidget::setItemSelected(Gitnoter::GroupType type, const QString &text)
{
    clearSelection();
    if (type <= Gitnoter::Trash) {
        topLevelItem(type)->setSelected(true);
        return;
    }

    if(type == Gitnoter::Category) {
        QTreeWidgetItem *treeWidgetItem = topLevelItem(6);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            if (childItem->text(0) == text) {
                return childItem->setSelected(true);
            }
        }
    }

    if (type == Gitnoter::Tag) {
        QTreeWidgetItem *treeWidgetItem = topLevelItem(8);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            if (childItem->text(0) == text) {
                return childItem->setSelected(true);
            }
        }
    }
}

QList<GroupModel *> GroupTreeWidget::getGroupModelList(Gitnoter::GroupType type)
{
    QList<GroupModel *> groupModelList = {};
    QTreeWidgetItem *treeWidgetItem = nullptr;
    if(type == Gitnoter::Category) {
        treeWidgetItem = topLevelItem(6);
    }
    else if (type == Gitnoter::Tag) {
        treeWidgetItem = topLevelItem(8);
    }

    if (!treeWidgetItem) {
        return groupModelList;
    }

    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        groupModelList.append(treeWidgetItem->child(i)->data(0, Qt::UserRole).value<GroupModel *>());
    }

    return groupModelList;
}

bool GroupTreeWidget::has(const QList<GroupModel *> &groupModelList, Gitnoter::GroupType type, const QString &text)
{
    for (auto &&item : groupModelList) {
        if (item->getType() == type && item->getName() == text) {
            return true;
        }
    }

    return false;
}
