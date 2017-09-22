#ifndef TAGSMODEL_H
#define TAGSMODEL_H

#include <QObject>

class TagsModel : public QObject
{
public:
    explicit TagsModel();
    TagsModel(int id, int tagsId, QString notesUuid, QString name, int createDate, int updateDate);
    TagsModel(int tagsId, QString name, int createDate, int updateDate);
    TagsModel(int id, int tagsId, QString notesUuid, int createDate);
    TagsModel(QString name);

    TagsModel* operator + (TagsModel &tagsModel);

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

    int getCount() const;
    void setCount(int count);

private:
    int id;
    int tagsId;
    QString notesUuid;
    QString name;
    int createDate;
    int updateDate;
    int count;

};

#endif // TAGSMODEL_H
