#ifndef TAGSMODEL_H
#define TAGSMODEL_H

#include <QObject>

class TagsModel : public QObject
{
Q_OBJECT

public:
    TagsModel(const QString &name = "", int count = 0);

    const QString &getName() const;

    void setName(const QString &name);

    int getCount() const;

    void setCount(int count);

private:
    QString name;
    int count;

};

#endif // TAGSMODEL_H
