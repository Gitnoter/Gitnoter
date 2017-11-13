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