#ifndef TAGMODEL_H
#define TAGMODEL_H

#include <QObject>

class TagModel : public QObject
{
Q_OBJECT

public:
    TagModel(const QString &name = "", int count = 0);

    const QString &getName() const;

    void setName(const QString &name);

    int getCount() const;

    void setCount(int count);

private:
    QString mName;
    int mCount;

};

#endif // TAGMODEL_H
