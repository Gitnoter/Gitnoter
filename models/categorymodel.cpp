#include "categorymodel.h"

CategoryModel::CategoryModel(const QString &name, int count)
{
    setName(name);
    setCount(count);
}

CategoryModel *CategoryModel::operator+(CategoryModel &categoriseModel)
{
    QString name = getName().isEmpty() ? categoriseModel.getName() : getName();
    int count = getCount() != 0 ? getCount() : categoriseModel.getCount();

    return new CategoryModel(name, count);
}

QString CategoryModel::getName() const
{
    return mName;
}

void CategoryModel::setName(QString name)
{
    mName = name;
}

int CategoryModel::getCount() const
{
    return mCount;
}

void CategoryModel::setCount(int count)
{
    CategoryModel::mCount = count;
}
