#ifndef CATEGORISETABLEMODEL_H
#define CATEGORISETABLEMODEL_H

#include <QObject>

class CategoriesTableModel : public QObject
{
public:
    explicit CategoriesTableModel();
    CategoriesTableModel(int id, int categoriesId, QString notesUuid, QString name, int createDate,
                                  int updateDate, int isDefault);
    CategoriesTableModel(int categoriesId, QString name, int createDate, int updateDate, int isDefault);
    CategoriesTableModel(int id, int categoriesId, QString notesUuid, int createDate);
    CategoriesTableModel(QString name);

    CategoriesTableModel* operator+(CategoriesTableModel & categoriseTableModel);

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

#endif // CATEGORISETABLEMODEL_H
