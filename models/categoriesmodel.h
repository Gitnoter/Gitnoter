#ifndef CATEGORISEMODEL_H
#define CATEGORISEMODEL_H

#include <QObject>

class CategoriesModel : public QObject
{
public:
    explicit CategoriesModel();
    CategoriesModel(int id, int categoriesId, QString notesUuid, QString name, int createDate,
                                  int updateDate, int isDefault);
    CategoriesModel(int categoriesId, QString name, int createDate, int updateDate, int isDefault);
    CategoriesModel(int id, int categoriesId, QString notesUuid, int createDate);
    CategoriesModel(QString name);

    CategoriesModel* operator+(CategoriesModel & categoriseModel);

    int getId();
    int getCategoriesId();
    QString getName();
    QString getNotesUuid();
    int getCreateDate();
    int getUpdateDate();
    int getIsDefault();
    int getCount() const;

    void setId(int id);
    void setCategoriesId(int categoriesId);
    void setName(QString name);
    void setNotesUuid(QString notesUuid);
    void setCreateDate(int createDate);
    void setUpdateDate(int updateDate);
    void setIsDefault(int isDefault);
    void setCount(int count);

private:
    int id;
    int categoriesId;
    QString name;
    QString notesUuid;
    int createDate;
    int updateDate;
    int isDefault;
    int count;

};

#endif // CATEGORISEMODEL_H
