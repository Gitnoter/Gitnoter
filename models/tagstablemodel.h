#ifndef TAGSTABLEMODEL_H
#define TAGSTABLEMODEL_H

#include <QObject>

class TagsTableModel : public QObject
{
public:
    explicit TagsTableModel();
    TagsTableModel(int id, int tagsId, QString notesUuid, QString name, int createDate, int updateDate);
    TagsTableModel(int tagsId, QString name, int createDate, int updateDate);
    TagsTableModel(int id, int tagsId, QString notesUuid, int createDate);
    TagsTableModel(QString name);

    TagsTableModel* operator + (TagsTableModel &tagsTableModel);

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

    int count;
public:
    int getCount() const;

    void setCount(int count);
};

#endif // TAGSTABLEMODEL_H
