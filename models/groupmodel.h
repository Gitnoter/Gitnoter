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

private:
    QString mName;
    int mCount;
    Gitnoter::GroupType mType;

};

#endif // GROUPMODELLIST_H
