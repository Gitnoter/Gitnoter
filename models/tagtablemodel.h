#ifndef TAGTABLEMODEL_H
#define TAGTABLEMODEL_H

#include <QObject>

class TagTableModel : public QObject
{
    Q_OBJECT
public:
    explicit TagTableModel(QString name, int createDate);
    TagTableModel(QString name);

    void setName(QString name);
    void setCreateDate(int insertDate);

    QString getName();
    int getCreateDate();

private:
    QString name;
    int insertDate;
};

#endif // TAGTABLEMODEL_H