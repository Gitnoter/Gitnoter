#include "categoriestablemodel.h"

CategoriseTableModel::CategoriseTableModel(int id, int categoriesId,  QString notesUuid, QString name, int createDate, int updateDate, int isDefault) {
    this->id = id;
    this->categoriesId = categoriesId;
    this->notesUuid = notesUuid;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriseTableModel::CategoriseTableModel(int id, int categoriesId, QString notesUuid, int createDate)
{
    this->id = id;
    this->categoriesId = categoriesId;
    this->notesUuid = notesUuid;
    this->createDate = createDate;
}

CategoriseTableModel::CategoriseTableModel(int id, QString name, int createDate, int updateDate, int isDefault)
{
    this->id = id;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriseTableModel::CategoriseTableModel(QString name)
{
    this->name = name;
    this->isDefault = 0;
}

QString CategoriseTableModel::getName()
{
    return this->name;
}

int CategoriseTableModel::getCreateDate()
{
    return this->createDate;
}

int CategoriseTableModel::getIsDefault()
{
    return this->isDefault;
}

void CategoriseTableModel::setName(QString name)
{
    this->name = name;
}

void CategoriseTableModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

void CategoriseTableModel::setIsDefault(int isDefault)
{
    this->isDefault = isDefault;
}

int CategoriseTableModel::getId()
{
    return this->id;
}

void CategoriseTableModel::setId(int id)
{
    this->id = id;
}

QString CategoriseTableModel::getNotesUuid()
{
    return this->notesUuid;
}

void CategoriseTableModel::setNotesUuid(QString notesUuid)
{
    this->notesUuid = notesUuid;
}

int CategoriseTableModel::getCategoriesId()
{
    return this->categoriesId;
}

int CategoriseTableModel::getUpdateDate()
{
    return this->updateDate;
}

void CategoriseTableModel::setCategoriesId(int categoriesId)
{
    this->categoriesId = categoriesId;
}

void CategoriseTableModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}
