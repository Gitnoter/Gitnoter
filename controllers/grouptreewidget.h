#ifndef GROUPTREEWIDGET_H
#define GROUPTREEWIDGET_H

#include "groupmodel.h"

#include <QTreeWidget>

class GroupTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit GroupTreeWidget(QWidget *parent = 0);

    ~GroupTreeWidget();

    void init(QList<NoteModel *> noteModelList);

    QTreeWidgetItem *getTreeWidgetItem(Gitnoter::GroupType type, const QString &text = "");

    QTreeWidgetItem *getTreeWidgetItem(GroupModel &groupModel);

    GroupModel *getGroupModel(Gitnoter::GroupType type, const QString &text = "");

    QList<GroupModel *> getGroupModelList(Gitnoter::GroupType type);

    GroupModel *append(GroupModel *groupModel);

    void removeOne(GroupModel *groupModel);

    void removeOne(Gitnoter::GroupType type, const QString &text);

    void appendAny(GroupModel *groupModel, int num);

    void appendOne(NoteModel *noteModel, int num = 1);

    bool has(const QList<GroupModel *> &groupModelList, Gitnoter::GroupType type, const QString &text);

    QString toString(Gitnoter::GroupType type);

    void setItemSelected(Gitnoter::GroupType type, const QString &text);

public:
    const QList<GroupModel *> &getGroupModelList() const { return mGroupModelList; }

    void setGroupModelList(const QList<GroupModel *> &groupModelList) { mGroupModelList = groupModelList; }

private:
    void appendToGroupModelMap(QMap<QString, GroupModel *> &map, Gitnoter::GroupType type, QString name, int count);

private:
    QList<GroupModel *> mGroupModelList;

};

#endif // GROUPTREEWIDGET_H
