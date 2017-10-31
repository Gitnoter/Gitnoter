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
    if (!Global::noteModelList.getList().length()) {
        return;
    }

    categoryModelList.clear();
    QMap<QString, CategoryModel *> map;
    for (auto &&noteModel : Global::noteModelList.getList()) {
        if (map.contains(noteModel->getCategory())) {
            CategoryModel *categoryModel = map[noteModel->getCategory()];
            categoryModel->setCount(categoryModel->getCount() + 1);
        }
        else {
            map[noteModel->getCategory()] = new CategoryModel(noteModel->getCategory(), 1);
        }
    }

    QStringList stringList = Tools::readerFileToList(Global::repoCategoryListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new CategoryModel(str);
        }
    }

    categoryModelList = map.values();
}

void CategoryModelList::append(const QString categoryName)
{
    for (auto &&item : categoryModelList) {
        if (item->getName() == categoryName) {
            item->setCount(item->getCount() + 1);
            return;
        }
    }

    categoryModelList.append(new CategoryModel(categoryName, 1));
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
    for (auto &&item : Global::noteModelList.getList()) {
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

const QString CategoryModelList::toString()
{
    QString str = "";
    for (auto &&item : categoryModelList) {
        str += item->getName() + "\n";
    }
    return str.trimmed();
}

const QList<CategoryModel *> &CategoryModelList::getList() const
{
    return categoryModelList;
}

void CategoryModelList::setList(const QList<CategoryModel *> &categoryModelList)
{
    CategoryModelList::categoryModelList = categoryModelList;
}

void CategoryModelList::saveToLocal()
{
    Tools::writerFile(Global::repoCategoryListPath, toString());
}
