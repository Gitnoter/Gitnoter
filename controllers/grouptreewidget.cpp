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

void GroupTreeWidget::appendToGroupModelMap(
        QMap<QString, GroupModel *> &map, Gitnoter::GroupType type, QString name, int count) {
    if (map.contains(name)) {
        GroupModel *model = map[name];
        model->setCount(model->getCount() + count);
    }
    else {
        map[name] = new GroupModel(type, name, count);
    }
}

void GroupTreeWidget::init(QList<NoteModel *> noteModelList, MainWindow *mainWindow)
{
    mMainWindow = mainWindow;

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

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onItemChanged(QTreeWidgetItem *, int)));
    setItemSelected();
}

bool GroupTreeWidget::append(Gitnoter::GroupType type, const QString &text, int num)
{
    if (!has(type, text)) {
        append(new GroupModel(type, text, num));
        saveDataToLocal(type);
        return true;
    }

    return false;
}

void GroupTreeWidget::subtract(Gitnoter::GroupType type, const QString &text, int num)
{
    if (has(type, text)) {
        appendAny(getGroupModel(type, text), -num);
    }
}

void GroupTreeWidget::remove(Gitnoter::GroupType type, const QString &name)
{
    QTreeWidgetItem *treeWidgetItem = getTreeWidgetItem(type, name);
    topLevelItem(type)->removeChild(treeWidgetItem);
    saveDataToLocal(type);

    QList<NoteModel *> noteModelList = mMainWindow->noteListWidget()->getNoteModelList(type, name);
    if (Gitnoter::Category == type) {
        for (auto &&item : noteModelList) {
            mMainWindow->noteListWidget()->trash(item);
            item->setCategory("");
            mMainWindow->noteListWidget()->noteModelChanged(item);
            subtract(item, true);
        }
    }
    else if (Gitnoter::Tag == type) {
        for (auto &&item : noteModelList) {
            QStringList tagList = item->getTagList();
            tagList.removeOne(name);
            item->setTagList(tagList);
            item->saveNoteDataToLocal();
            subtract(type, name);
            mMainWindow->noteListWidget()->noteModelChanged(item);
        }
    }
}

void GroupTreeWidget::add(Gitnoter::GroupType type, const QString &text, int num)
{
    if (has(type, text)) {
        appendAny(getGroupModel(type, text), num);
    }
}

void GroupTreeWidget::modify(GroupModel *groupModel, QString oldText, const QString &newText)
{
    QList<NoteModel *> noteModelList = mMainWindow->noteListWidget()->getNoteModelList(groupModel);
    groupModel->setName(newText);

    if (Gitnoter::Category == groupModel->getType()) {
        for (auto &&item : noteModelList) {
            item->setCategory(newText);
            item->saveNoteDataToLocal();
            mMainWindow->noteListWidget()->noteModelChanged(item);
        }
        saveDataToLocal(Gitnoter::Category);
    }
    else if (Gitnoter::Tag == groupModel->getType()) {
        for (auto &&item : noteModelList) {
            QStringList tagList = item->getTagList();
            tagList.removeOne(oldText);
            tagList.append(newText);
            item->setTagList(tagList);
            item->saveNoteDataToLocal();
            mMainWindow->noteListWidget()->noteModelChanged(item);
        }
        saveDataToLocal(Gitnoter::Tag);
    }
    Globals::configModel->setSideSelected(groupModel);
    mMainWindow->updateView(Gitnoter::NoteListTitle | Gitnoter::MarkdownEditorWidget);
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
    childItem->setFlags(childItem->flags() | Qt::ItemIsEditable);
    treeWidgetItem->addChild(childItem);
    treeWidgetItem->sortChildren(0, Qt::AscendingOrder);

    return groupModel;
}

void GroupTreeWidget::setItemSelected()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString name = Globals::configModel->getSideSelectedName();

    clearSelection();
    if (type <= Gitnoter::Trash) {
        topLevelItem(type)->setSelected(true);
        return;
    }

    if(type == Gitnoter::Category) {
        QTreeWidgetItem *treeWidgetItem = topLevelItem(6);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            if (childItem->text(0) == name) {
                return childItem->setSelected(true);
            }
        }
    }

    if (type == Gitnoter::Tag) {
        QTreeWidgetItem *treeWidgetItem = topLevelItem(8);
        for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
            QTreeWidgetItem *childItem = treeWidgetItem->child(i);
            if (childItem->text(0) == name) {
                return childItem->setSelected(true);
            }
        }
    }
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

QList<GroupModel *> GroupTreeWidget::getGroupModelList(Gitnoter::GroupType type)
{
    QList<GroupModel *> groupModelList = {};
    QTreeWidgetItem *treeWidgetItem = nullptr;
    treeWidgetItem = topLevelItem(type);

    if (!treeWidgetItem) {
        return groupModelList;
    }

    if (Gitnoter::Trash >= type) {
        groupModelList.append(getGroupModel(treeWidgetItem));
    }

    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        groupModelList.append(getGroupModel(treeWidgetItem->child(i)));
    }

    return groupModelList;
}

bool GroupTreeWidget::has(Gitnoter::GroupType type, const QString &text)
{
    QList<GroupModel *> groupModelList = getGroupModelList(type);
    for (auto &&item : groupModelList) {
        if (Gitnoter::Category > type && item->getType() == type) {
            return true;
        }

        if (item->getType() == type && item->getName() == text) {
            return true;
        }
    }

    return false;
}

void GroupTreeWidget::saveDataToLocal(Gitnoter::GroupType type)
{
    if (type == Gitnoter::Category) {
        Tools::writerFile(Globals::repoCategoryListPath, toString(type).toUtf8());
    }
    else if (type == Gitnoter::Tag) {
        Tools::writerFile(Globals::repoTagListPath, toString(type).toUtf8());
    }
}

QList<GroupModel *> GroupTreeWidget::getGroupModelList()
{
    QList<GroupModel *> groupModelList = {};

    for (int j = 1; j < 5; ++j) {
        groupModelList.append(topLevelItem(j)->data(0, Qt::UserRole).value<GroupModel *>());
    }

    QTreeWidgetItem *treeWidgetItem = topLevelItem(6);
    for (int k = 0; k < treeWidgetItem->childCount(); ++k) {
        groupModelList.append(treeWidgetItem->child(k)->data(0, Qt::UserRole).value<GroupModel *>());
    }

    treeWidgetItem = topLevelItem(8);
    for (int i = 0; i < treeWidgetItem->childCount(); ++i) {
        groupModelList.append(treeWidgetItem->child(i)->data(0, Qt::UserRole).value<GroupModel *>());
    }

    return groupModelList;
}

GroupModel *GroupTreeWidget::selectedGroupModel()
{
    QList<QTreeWidgetItem *> treeWidgetItemList = selectedItems();
    if (treeWidgetItemList.length()) {
        return treeWidgetItemList[0]->data(0, Qt::UserRole).value<GroupModel *>();
    }
    return nullptr;
}

GroupModel *GroupTreeWidget::getGroupModel(QTreeWidgetItem *treeWidgetItem)
{
    return treeWidgetItem->data(0, Qt::UserRole).value<GroupModel *>();
}

void GroupTreeWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
    GroupModel *groupModel = getGroupModel(item);
    const QString text = item->text(column);
    if (groupModel->getType() >= Gitnoter::Category) {
        if (has(groupModel->getType(), text)) {
            item->setText(column, groupModel->getName());
        }
        else {
            modify(groupModel, groupModel->getName(), text);
        }
    }
}

void GroupTreeWidget::subtract(NoteModel *noteModel, bool remove)
{
    if (noteModel->getCategory().isEmpty()) {
        mMainWindow->groupTreeWidget()->subtract(Gitnoter::Unclassified);
    }
    else {
        mMainWindow->groupTreeWidget()->subtract(Gitnoter::Category, noteModel->getCategory());
    }
    if (noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) {
        mMainWindow->groupTreeWidget()->subtract(Gitnoter::Recent);
    }
    mMainWindow->groupTreeWidget()->subtract(Gitnoter::All);

    for (auto &&tagName : noteModel->getTagList()) {
        mMainWindow->groupTreeWidget()->subtract(Gitnoter::Tag, tagName);
    }

    if (!remove) {
        mMainWindow->groupTreeWidget()->add(Gitnoter::Trash);
    }
}

void GroupTreeWidget::add(NoteModel *noteModel)
{
    if (noteModel->getCategory().isEmpty()) {
        mMainWindow->groupTreeWidget()->add(Gitnoter::Unclassified);
    }
    else {
        mMainWindow->groupTreeWidget()->add(Gitnoter::Category, noteModel->getCategory());
    }
    if (noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) {
        mMainWindow->groupTreeWidget()->add(Gitnoter::Recent);
    }
    mMainWindow->groupTreeWidget()->add(Gitnoter::All);

    for (auto &&tagName : noteModel->getTagList()) {
        mMainWindow->groupTreeWidget()->add(Gitnoter::Tag, tagName);
    }

    mMainWindow->groupTreeWidget()->subtract(Gitnoter::Trash);
}

