#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

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

#endif // CATEGORYMODEL_H
