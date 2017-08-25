#ifndef TAGTABLEMODEL_H
#define TAGTABLEMODEL_H

#include <QObject>

class TagTableModel : public QObject
{
public:
    explicit TagTableModel();
    TagTableModel(int id, int tagsId, QString notesUuid, QString name, int createDate, int updateDate);
    TagTableModel(int tagsId, QString name, int createDate, int updateDate);
    TagTableModel(int id, int tagsId, QString notesUuid, int createDate);
    TagTableModel(QString name);

    TagTableModel* operator + (TagTableModel &tagTableModel);

    void setId(int id);
    void setTagsId(int tagsId);
    void setName(QString name);
    void setNotesUuid(QString notesUuid);
    void setCreateDate(int createDate);
    void setUpdateDate(int updateDate);

    int getId();
    int getTagsId();
    QString getName();
    QString getNotesUuid();
    int getCreateDate();
    int getUpdateDate();

private:
    int id;
    int tagsId;
    QString notesUuid;
    QString name;
    int createDate;
    int updateDate;
};

#endif // TAGTABLEMODEL_H
