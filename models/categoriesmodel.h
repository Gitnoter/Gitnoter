#ifndef CATEGORISEMODEL_H
#define CATEGORISEMODEL_H

#include <QObject>

class CategoriesModel : public QObject
{
Q_OBJECT

public:
    CategoriesModel(const QString &name = "", int count = 0, int isDefault = 0);

    CategoriesModel *operator+(CategoriesModel &categoriseModel);

    QString getName() const;

    int getIsDefault();

    int getCount() const;

    void setName(QString name);

    void setIsDefault(int isDefault);

    void setCount(int count);

private:
    QString mName;
    int mIsDefault;
    int mCount;

};

#endif // CATEGORISEMODEL_H
