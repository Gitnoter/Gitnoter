#ifndef CATEGORISETABLEMODEL_H
#define CATEGORISETABLEMODEL_H

#include <QObject>

class CategoriseTableModel : public QObject
{
public:
    explicit CategoriseTableModel(int id, int categoriesId, QString notesUuid, QString name, int createDate,
                                  int updateDate, int isDefault);
    CategoriseTableModel(int categoriesId, QString name, int createDate, int updateDate, int isDefault);
    CategoriseTableModel(int id, int categoriesId, QString notesUuid, int createDate);
    CategoriseTableModel(QString name);

    CategoriseTableModel* operator+(CategoriseTableModel & categoriseTableModel);

    int getId();
    int getCategoriesId();
    QString getName();
    QString getNotesUuid();
    int getCreateDate();
    int getUpdateDate();
    int getIsDefault();

    void setId(int id);
    void setCategoriesId(int categoriesId);
    void setName(QString name);
    void setNotesUuid(QString notesUuid);
    void setCreateDate(int createDate);
    void setUpdateDate(int updateDate);
    void setIsDefault(int isDefault);

private:
    int id;
    int categoriesId;
    QString name;
    QString notesUuid;
    int createDate;
    int updateDate;
    int isDefault;
};

#endif // CATEGORISETABLEMODEL_H
