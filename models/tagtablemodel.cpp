#include "tagtablemodel.h"

TagTableModel::TagTableModel()
{
    this->id = 0;
    this->tagsId = 0;
    this->notesUuid = "";
    this->name = "";
    this->createDate = 0;
    this->updateDate = 0;
}

TagTableModel::TagTableModel(int id, int tagsId, QString notesUuid, QString name, int createDate, int updateDate)
{
    this->id = id;
    this->tagsId = tagsId;
    this->notesUuid = notesUuid;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
}

TagTableModel::TagTableModel(int id, int tagsId, QString notesUuid, int createDate)
{
    this->id = id;
    this->tagsId = tagsId;
    this->notesUuid = notesUuid;
    this->name = "";
    this->createDate = createDate;
    this->updateDate = 0;
}

TagTableModel::TagTableModel(int tagsId, QString name, int createDate, int updateDate)
{
    this->id = 0;
    this->tagsId = tagsId;
    this->notesUuid = "";
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
}

TagTableModel::TagTableModel(QString name)
{
    this->id = 0;
    this->tagsId = 0;
    this->notesUuid = "";
    this->name = name;
    this->createDate = 0;
    this->updateDate = 0;
}

TagTableModel* TagTableModel::operator+(TagTableModel & tagTableModel)
{
    int id = this->getId() != 0 ? this->getId() : tagTableModel.getId();
    int tagsId = this->getTagsId() != 0 ? this->getTagsId() : tagTableModel.getTagsId();
    QString notesUuid = this->getNotesUuid().isEmpty() ? tagTableModel.getNotesUuid() : this->getNotesUuid();
    QString name = this->getName().isEmpty() ? tagTableModel.getName() : this->getName();
    int createDate = this->getCreateDate() != 0 ? this->getCreateDate() : tagTableModel.getCreateDate();
    int updateDate = this->getUpdateDate() != 0 ? this->getUpdateDate() : tagTableModel.getUpdateDate();

    return new TagTableModel(id, tagsId, notesUuid, name, createDate, updateDate);
}

void TagTableModel::setName(QString name)
{
    this->name = name;
}

void TagTableModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

QString TagTableModel::getName()
{
    return this->name;
}

int TagTableModel::getCreateDate()
{
    return this->createDate;
}

void TagTableModel::setId(int id)
{
    this->id = id;
}

int TagTableModel::getId()
{
    return this->id;
}

void TagTableModel::setNotesUuid(QString notesUuid)
{
    this->notesUuid = notesUuid;
}

QString TagTableModel::getNotesUuid()
{
    return this->notesUuid;
}

void TagTableModel::setTagsId(int tagsId)
{
    this->tagsId = tagsId;
}

void TagTableModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

int TagTableModel::getTagsId()
{
    return this->tagsId;
}

int TagTableModel::getUpdateDate()
{
    return this->updateDate;
}
