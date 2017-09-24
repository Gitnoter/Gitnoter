#include "categoriesmodel.h"

CategoriesModel::CategoriesModel(const QString &name, int count, int isDefault)
{
    mName = name;
    mCount = count;
    mIsDefault = isDefault;
}

CategoriesModel *CategoriesModel::operator+(CategoriesModel &categoriseModel)
{
    QString name = getName().isEmpty() ? categoriseModel.getName() : getName();
    int count = getCount() != 0 ? getCount() : categoriseModel.getCount();
    int isDefault = getIsDefault() != 0 ? getIsDefault() : categoriseModel.getIsDefault();

    return new CategoriesModel(name, count, isDefault);
}

QString CategoriesModel::getName() const
{
    return mName;
}

int CategoriesModel::getIsDefault()
{
    return mIsDefault;
}

void CategoriesModel::setName(QString name)
{
    mName = name;
}

void CategoriesModel::setIsDefault(int isDefault)
{
    mIsDefault = isDefault;
}

int CategoriesModel::getCount() const
{
    return mCount;
}

void CategoriesModel::setCount(int count)
{
    CategoriesModel::mCount = count;
}
