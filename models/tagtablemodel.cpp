#include "tagtablemodel.h"

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
    this->createDate = createDate;
}

TagTableModel::TagTableModel(int id, QString name, int createDate, int updateDate)
{
    this->id = id;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
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

TagTableModel::TagTableModel(QString name)
{
    this->name = name;
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
