#include "tagsmodel.h"

TagsModel::TagsModel()
{
    this->id = 0;
    this->tagsId = 0;
    this->notesUuid = "";
    this->name = "";
    this->createDate = 0;
    this->updateDate = 0;
}

TagsModel::TagsModel(int id, int tagsId, QString notesUuid, QString name, int createDate, int updateDate)
{
    this->id = id;
    this->tagsId = tagsId;
    this->notesUuid = notesUuid;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
}

TagsModel::TagsModel(int id, int tagsId, QString notesUuid, int createDate)
{
    this->id = id;
    this->tagsId = tagsId;
    this->notesUuid = notesUuid;
    this->name = "";
    this->createDate = createDate;
    this->updateDate = 0;
}

TagsModel::TagsModel(int tagsId, QString name, int createDate, int updateDate)
{
    this->id = 0;
    this->tagsId = tagsId;
    this->notesUuid = "";
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
}

TagsModel::TagsModel(QString name)
{
    this->id = 0;
    this->tagsId = 0;
    this->notesUuid = "";
    this->name = name;
    this->createDate = 0;
    this->updateDate = 0;
}

TagsModel* TagsModel::operator+(TagsModel & tagsModel)
{
    int id = this->getId() != 0 ? this->getId() : tagsModel.getId();
    int tagsId = this->getTagsId() != 0 ? this->getTagsId() : tagsModel.getTagsId();
    QString notesUuid = this->getNotesUuid().isEmpty() ? tagsModel.getNotesUuid() : this->getNotesUuid();
    QString name = this->getName().isEmpty() ? tagsModel.getName() : this->getName();
    int createDate = this->getCreateDate() != 0 ? this->getCreateDate() : tagsModel.getCreateDate();
    int updateDate = this->getUpdateDate() != 0 ? this->getUpdateDate() : tagsModel.getUpdateDate();

    return new TagsModel(id, tagsId, notesUuid, name, createDate, updateDate);
}

void TagsModel::setName(QString name)
{
    this->name = name;
}

void TagsModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

QString TagsModel::getName()
{
    return this->name;
}

int TagsModel::getCreateDate()
{
    return this->createDate;
}

void TagsModel::setId(int id)
{
    this->id = id;
}

int TagsModel::getId()
{
    return this->id;
}

void TagsModel::setNotesUuid(QString notesUuid)
{
    this->notesUuid = notesUuid;
}

QString TagsModel::getNotesUuid()
{
    return this->notesUuid;
}

void TagsModel::setTagsId(int tagsId)
{
    this->tagsId = tagsId;
}

void TagsModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

int TagsModel::getTagsId()
{
    return this->tagsId;
}

int TagsModel::getUpdateDate()
{
    return this->updateDate;
}

int TagsModel::getCount() const
{
    return count;
}

void TagsModel::setCount(int count)
{
    TagsModel::count = count;
}
