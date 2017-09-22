#include "tagsmodel.h"

TagsModel::TagsModel(const QString &name, int count)
{
    TagsModel::name = name;
    TagsModel::count = count;
}

const QString &TagsModel::getName() const
{
    return name;
}

void TagsModel::setName(const QString &name)
{
    TagsModel::name = name;
}

int TagsModel::getCount() const
{
    return count;
}

void TagsModel::setCount(int count)
{
    TagsModel::count = count;
}
