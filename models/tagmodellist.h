#ifndef TAGMODELLIST_H
#define TAGMODELLIST_H

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

class TagModelList : public QObject
{
Q_OBJECT

public:
    void init();

    void append(const QString tagName);

    int indexOf(const QString &name);

    void rename(const QString oldName, const QString newName);

    void removeOne(const QString &name);

    void removeList(const QStringList &nameList);

    const QString toString();

    void saveToLocal();

    const QList<TagModel *> &getList() const;

    void setList(const QList<TagModel *> &tagModelList);

private:
    QList<TagModel *> tagModelList;

};

#endif // TAGMODELLIST_H
