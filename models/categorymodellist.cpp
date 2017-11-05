#include "categorymodellist.h"
#include "notemodellist.h"
#include "globals.h"
#include "tools.h"

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

/**
 *
 * CategoryModelList
 *
 */

CategoryModelList::CategoryModelList()
{
    categoryModelList = {};
}

void CategoryModelList::init()
{
    if (!Globals::noteModelList.getList().length()) {
        return;
    }

    categoryModelList.clear();
    QMap<QString, CategoryModel *> map;
    for (auto &&noteModel : Globals::noteModelList.getList()) {
        if (map.contains(noteModel->getCategory())) {
            CategoryModel *categoryModel = map[noteModel->getCategory()];
            categoryModel->setCount(categoryModel->getCount() + 1);
        }
        else {
            map[noteModel->getCategory()] = new CategoryModel(noteModel->getCategory(), 0);
        }
    }

    QStringList stringList = Tools::readerFileToList(Globals::repoCategoryListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new CategoryModel(str);
        }
    }

    categoryModelList = map.values();
}

CategoryModel *CategoryModelList::append(const QString categoryName)
{
    for (auto &&item : categoryModelList) {
        if (item->getName() == categoryName) {
            item->setCount(item->getCount() + 1);
            return item;
        }
    }

    CategoryModel *categoryModel = new CategoryModel(categoryName, 0);
    categoryModelList.append(categoryModel);

    return categoryModel;
}

int CategoryModelList::indexOf(const QString &name)
{
    for (int i = 0; i < categoryModelList.length(); ++i) {
        if (categoryModelList[i]->getName() == name) {
            return i;
        }
    }

    return -1;
}

void CategoryModelList::rename(const QString oldName, const QString newName)
{
    for (auto &&item : Globals::noteModelList.getList()) {
        if (item->getCategory() == oldName) {
            item->setCategory(newName);
            item->saveNoteDataToLocal();
        }
    }
}

void CategoryModelList::removeOne(const QString &name)
{
    for (auto &&item : categoryModelList) {
        if (item->getName() == name) {
            item->setCount(item->getCount() - 1);
        }
    }
}

void CategoryModelList::removeAt(int index)
{
    categoryModelList.removeAt(index);
}

const QString CategoryModelList::toString()
{
    QString str = "";
    for (auto &&item : categoryModelList) {
        str += item->getName() + "\n";
    }
    return str.trimmed();
}

void CategoryModelList::saveToLocal()
{
    Tools::writerFile(Globals::repoCategoryListPath, toString());
}

const QList<CategoryModel *> &CategoryModelList::getList() const
{
    return categoryModelList;
}

void CategoryModelList::setList(const QList<CategoryModel *> &categoryModelList)
{
    CategoryModelList::categoryModelList = categoryModelList;
}
