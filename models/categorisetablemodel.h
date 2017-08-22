#ifndef CATEGORISETABLEMODEL_H
#define CATEGORISETABLEMODEL_H

#include <QObject>

class CategoriseTableModel : public QObject
{
    Q_OBJECT
public:
    explicit CategoriseTableModel(QString name, int createDate, int isDefault);
    CategoriseTableModel(QString name);

    QString getName();
    int getCreateDate();
    int getIsDefault();

    void setName(QString name);
    void setCreateDate(int insertDate);
    void setIsDefault(int isDefault);

private:
    QString name;
    int insertDate;
    int isDefault;
};

#endif // CATEGORISETABLEMODEL_H
