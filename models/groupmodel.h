#ifndef GROUPMODELLIST_H
#define GROUPMODELLIST_H

#include "notemodellist.h"

#include <QObject>
#include <QTreeWidget>

class GroupModel : public QObject
{
    Q_OBJECT
public:
    enum GroupType {
        All = 0x000001,
        Recent = 0x000002,
        Unclassified = 0x000003,
        Trash = 0x000004,
        Category = 0x000006,
        Tag = 0x000008
    };

    GroupModel(GroupType type, const QString &name = "", int count = 0);

    const QString &getName() const;

    void setName(const QString &name);

    int getCount() const;

    void setCount(int count);

    GroupType getType() const;

    void setType(GroupType type);


    static void init(QTreeWidget *treeWidget, QList<NoteModel *> noteModelList);

    static QTreeWidgetItem *find(QTreeWidget *treeWidget, GroupModel::GroupType type, const QString &text);

    static QTreeWidgetItem *find(QTreeWidget *treeWidget, GroupModel &groupModel);

    static GroupModel *append(QTreeWidget *treeWidget, GroupModel *groupModel);

    static void removeOne(QTreeWidget *treeWidget, GroupModel *groupModel);

    static void removeOne(QTreeWidget *treeWidget, GroupModel::GroupType type, const QString &text);

    static void subtractOne(GroupModel *groupModel);

    static void subtractOne(QTreeWidget *treeWidget, GroupModel::GroupType type, const QString &text);

    static QString toString(QTreeWidget *treeWidget, GroupModel::GroupType type);

    static void setItemSelected(QTreeWidget *treeWidget, GroupModel::GroupType type, const QString &text);

private:
    QString mName;
    int mCount;
    GroupType mType;

};

#endif // GROUPMODELLIST_H
