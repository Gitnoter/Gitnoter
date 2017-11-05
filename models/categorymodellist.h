#ifndef CATEGORYMODELLIST_H
#define CATEGORYMODELLIST_H

#include <QObject>

class CategoryModel : public QObject
{
Q_OBJECT

public:
    CategoryModel(const QString &name, int count = 0);

    CategoryModel *operator+(CategoryModel &categoriseModel);

    QString getName() const;

    void setName(QString name);

    int getCount() const;

    void setCount(int count);

private:
    QString mName;
    int mCount;

};

class CategoryModelList : public QObject
{
Q_OBJECT

public:
    CategoryModelList();

    void init();

    CategoryModel *append(const QString categoryName);

    int indexOf(const QString &name);

    void rename(const QString oldName, const QString newName);

    void removeOne(const QString &name);

    const QString toString();

    void saveToLocal();

    const QList<CategoryModel *> &getList() const;

    void setList(const QList<CategoryModel *> &categoryModelList);

private:
    QList<CategoryModel *> categoryModelList;

};

#endif // CATEGORYMODELLIST_H
