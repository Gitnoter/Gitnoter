#include "tagmodel.h"

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
