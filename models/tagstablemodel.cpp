#include "tagstablemodel.h"

TagsTableModel::TagsTableModel()
{
    this->id = 0;
    this->tagsId = 0;
    this->notesUuid = "";
    this->name = "";
    this->createDate = 0;
    this->updateDate = 0;
}

TagsTableModel::TagsTableModel(int id, int tagsId, QString notesUuid, QString name, int createDate, int updateDate)
{
    this->id = id;
    this->tagsId = tagsId;
    this->notesUuid = notesUuid;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
}

TagsTableModel::TagsTableModel(int id, int tagsId, QString notesUuid, int createDate)
{
    this->id = id;
    this->tagsId = tagsId;
    this->notesUuid = notesUuid;
    this->name = "";
    this->createDate = createDate;
    this->updateDate = 0;
}

TagsTableModel::TagsTableModel(int tagsId, QString name, int createDate, int updateDate)
{
    this->id = 0;
    this->tagsId = tagsId;
    this->notesUuid = "";
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
}

TagsTableModel::TagsTableModel(QString name)
{
    this->id = 0;
    this->tagsId = 0;
    this->notesUuid = "";
    this->name = name;
    this->createDate = 0;
    this->updateDate = 0;
}

TagsTableModel* TagsTableModel::operator+(TagsTableModel & tagsTableModel)
{
    int id = this->getId() != 0 ? this->getId() : tagsTableModel.getId();
    int tagsId = this->getTagsId() != 0 ? this->getTagsId() : tagsTableModel.getTagsId();
    QString notesUuid = this->getNotesUuid().isEmpty() ? tagsTableModel.getNotesUuid() : this->getNotesUuid();
    QString name = this->getName().isEmpty() ? tagsTableModel.getName() : this->getName();
    int createDate = this->getCreateDate() != 0 ? this->getCreateDate() : tagsTableModel.getCreateDate();
    int updateDate = this->getUpdateDate() != 0 ? this->getUpdateDate() : tagsTableModel.getUpdateDate();

    return new TagsTableModel(id, tagsId, notesUuid, name, createDate, updateDate);
}

void TagsTableModel::setName(QString name)
{
    this->name = name;
}

void TagsTableModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

QString TagsTableModel::getName()
{
    return this->name;
}

int TagsTableModel::getCreateDate()
{
    return this->createDate;
}

void TagsTableModel::setId(int id)
{
    this->id = id;
}

int TagsTableModel::getId()
{
    return this->id;
}

void TagsTableModel::setNotesUuid(QString notesUuid)
{
    this->notesUuid = notesUuid;
}

QString TagsTableModel::getNotesUuid()
{
    return this->notesUuid;
}

void TagsTableModel::setTagsId(int tagsId)
{
    this->tagsId = tagsId;
}

void TagsTableModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

int TagsTableModel::getTagsId()
{
    return this->tagsId;
}

int TagsTableModel::getUpdateDate()
{
    return this->updateDate;
}

int TagsTableModel::getCount() const
{
    return count;
}

void TagsTableModel::setCount(int count)
{
    TagsTableModel::count = count;
}
