#include "categoriesmodel.h"

CategoriesModel::CategoriesModel()
{
    this->id = 0;
    this->categoriesId = 0;
    this->notesUuid = "";
    this->name = "";
    this->createDate = 0;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriesModel::CategoriesModel(int id, int categoriesId,  QString notesUuid, QString name, int createDate, int updateDate, int isDefault)
{
    this->id = id;
    this->categoriesId = categoriesId;
    this->notesUuid = notesUuid;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriesModel::CategoriesModel(int id, int categoriesId, QString notesUuid, int createDate)
{
    this->id = id;
    this->categoriesId = categoriesId;
    this->notesUuid = notesUuid;
    this->name = "";
    this->createDate = createDate;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriesModel::CategoriesModel(int categoriesId, QString name, int createDate, int updateDate, int isDefault)
{
    this->id = 0;
    this->categoriesId = categoriesId;
    this->notesUuid = "";
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriesModel::CategoriesModel(QString name)
{
    this->id = 0;
    this->categoriesId = 0;
    this->notesUuid = "";
    this->name = name;
    this->createDate = 0;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriesModel* CategoriesModel::operator+(CategoriesModel & categoriseModel)
{
    int id = this->getId() != 0 ? this->getId() : categoriseModel.getId();
    int categoriesId = this->getCategoriesId() != 0 ? this->getCategoriesId() : categoriseModel.getCategoriesId();
    QString notesUuid = this->getNotesUuid().isEmpty() ? categoriseModel.getNotesUuid() : this->getNotesUuid();
    QString name = this->getName().isEmpty() ? categoriseModel.getName() : this->getName();
    int createDate = this->getCreateDate() != 0 ? this->getCreateDate() : categoriseModel.getCreateDate();
    int updateDate = this->getUpdateDate() != 0 ? this->getUpdateDate() : categoriseModel.getUpdateDate();
    int isDefault = this->getIsDefault() != 0 ? this->getIsDefault() : categoriseModel.getIsDefault();

    return new CategoriesModel(id, categoriesId, notesUuid, name, createDate, updateDate, isDefault);
}

QString CategoriesModel::getName()
{
    return this->name;
}

int CategoriesModel::getCreateDate()
{
    return this->createDate;
}

int CategoriesModel::getIsDefault()
{
    return this->isDefault;
}

void CategoriesModel::setName(QString name)
{
    this->name = name;
}

void CategoriesModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

void CategoriesModel::setIsDefault(int isDefault)
{
    this->isDefault = isDefault;
}

int CategoriesModel::getId()
{
    return this->id;
}

void CategoriesModel::setId(int id)
{
    this->id = id;
}

QString CategoriesModel::getNotesUuid()
{
    return this->notesUuid;
}

void CategoriesModel::setNotesUuid(QString notesUuid)
{
    this->notesUuid = notesUuid;
}

int CategoriesModel::getCategoriesId()
{
    return this->categoriesId;
}

int CategoriesModel::getUpdateDate()
{
    return this->updateDate;
}

void CategoriesModel::setCategoriesId(int categoriesId)
{
    this->categoriesId = categoriesId;
}

void CategoriesModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

int CategoriesModel::getCount() const
{
    return count;
}

void CategoriesModel::setCount(int count)
{
    CategoriesModel::count = count;
}
