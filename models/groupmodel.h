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
        None = 0x000000,
        System = 0x000001,
        Category = 0x000002,
        Tag = 0x000003
    };

    GroupModel(GroupType type, const QString &name = "", int count = 0);

    const QString &getName() const;

    void setName(const QString &name);

    int getCount() const;

    void setCount(int count);

    GroupType getType() const;

    void setType(GroupType type);


    static void init(QTreeWidget *treeWidget, QList<NoteModel *> noteModelList);

    static GroupModel *append(QTreeWidget *treeWidget, GroupModel *groupModel);

private:
    QString mName;
    int mCount;
    GroupType mType;

};

#endif // GROUPMODELLIST_H