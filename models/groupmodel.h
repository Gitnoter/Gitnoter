#ifndef GROUPMODELLIST_H
#define GROUPMODELLIST_H

#include "gitnoter.h"
#include "notemodel.h"

#include <QObject>
#include <QTreeWidget>

class NoteModel;

class GroupModel : public QObject
{
    Q_OBJECT
public:
    GroupModel(Gitnoter::GroupType type, const QString &name = "", int count = 0);

    const QString &getName() const;

    void setName(const QString &name);

    int getCount() const;

    void setCount(int count);

    Gitnoter::GroupType getType() const;

    void setType(Gitnoter::GroupType type);

public:
    static void init(QTreeWidget *treeWidget, QList<NoteModel *> noteModelList);

    static QTreeWidgetItem *getTreeWidgetItem(QTreeWidget *treeWidget, Gitnoter::GroupType type,
                                              const QString &text = "");

    static QTreeWidgetItem *getTreeWidgetItem(QTreeWidget *treeWidget, GroupModel &groupModel);

    static GroupModel *getGroupModel(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text = "");

    static QList<GroupModel *> getGroupModelList(QTreeWidget *treeWidget, Gitnoter::GroupType type);

    static GroupModel *append(QTreeWidget *treeWidget, GroupModel *groupModel);

    static void removeOne(QTreeWidget *treeWidget, GroupModel *groupModel);

    static void removeOne(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text);

    static void appendAny(GroupModel *groupModel, int num);

    static void appendOne(QTreeWidget *treeWidget, NoteModel *noteModel, int num = 1);

    static bool has(const QList<GroupModel *> &groupModelList, Gitnoter::GroupType type, const QString &text);

    static QString toString(QTreeWidget *treeWidget, Gitnoter::GroupType type);

    static void setItemSelected(QTreeWidget *treeWidget, Gitnoter::GroupType type, const QString &text);

private:
    QString mName;
    int mCount;
    Gitnoter::GroupType mType;

};

#endif // GROUPMODELLIST_H
