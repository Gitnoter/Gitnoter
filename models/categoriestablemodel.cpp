#include "categoriestablemodel.h"

CategoriesTableModel::CategoriesTableModel()
{
    this->id = 0;
    this->categoriesId = 0;
    this->notesUuid = "";
    this->name = "";
    this->createDate = 0;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriesTableModel::CategoriesTableModel(int id, int categoriesId,  QString notesUuid, QString name, int createDate, int updateDate, int isDefault)
{
    this->id = id;
    this->categoriesId = categoriesId;
    this->notesUuid = notesUuid;
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriesTableModel::CategoriesTableModel(int id, int categoriesId, QString notesUuid, int createDate)
{
    this->id = id;
    this->categoriesId = categoriesId;
    this->notesUuid = notesUuid;
    this->name = "";
    this->createDate = createDate;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriesTableModel::CategoriesTableModel(int categoriesId, QString name, int createDate, int updateDate, int isDefault)
{
    this->id = 0;
    this->categoriesId = categoriesId;
    this->notesUuid = "";
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriesTableModel::CategoriesTableModel(QString name)
{
    this->id = 0;
    this->categoriesId = 0;
    this->notesUuid = "";
    this->name = name;
    this->createDate = 0;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriesTableModel* CategoriesTableModel::operator+(CategoriesTableModel & categoriseTableModel)
{
    int id = this->getId() != 0 ? this->getId() : categoriseTableModel.getId();
    int categoriesId = this->getCategoriesId() != 0 ? this->getCategoriesId() : categoriseTableModel.getCategoriesId();
    QString notesUuid = this->getNotesUuid().isEmpty() ? categoriseTableModel.getNotesUuid() : this->getNotesUuid();
    QString name = this->getName().isEmpty() ? categoriseTableModel.getName() : this->getName();
    int createDate = this->getCreateDate() != 0 ? this->getCreateDate() : categoriseTableModel.getCreateDate();
    int updateDate = this->getUpdateDate() != 0 ? this->getUpdateDate() : categoriseTableModel.getUpdateDate();
    int isDefault = this->getIsDefault() != 0 ? this->getIsDefault() : categoriseTableModel.getIsDefault();

    return new CategoriesTableModel(id, categoriesId, notesUuid, name, createDate, updateDate, isDefault);
}

QString CategoriesTableModel::getName()
{
    return this->name;
}

int CategoriesTableModel::getCreateDate()
{
    return this->createDate;
}

int CategoriesTableModel::getIsDefault()
{
    return this->isDefault;
}

void CategoriesTableModel::setName(QString name)
{
    this->name = name;
}

void CategoriesTableModel::setCreateDate(int createDate)
{
    this->createDate = createDate;
}

void CategoriesTableModel::setIsDefault(int isDefault)
{
    this->isDefault = isDefault;
}

int CategoriesTableModel::getId()
{
    return this->id;
}

void CategoriesTableModel::setId(int id)
{
    this->id = id;
}

QString CategoriesTableModel::getNotesUuid()
{
    return this->notesUuid;
}

void CategoriesTableModel::setNotesUuid(QString notesUuid)
{
    this->notesUuid = notesUuid;
}

int CategoriesTableModel::getCategoriesId()
{
    return this->categoriesId;
}

int CategoriesTableModel::getUpdateDate()
{
    return this->updateDate;
}

void CategoriesTableModel::setCategoriesId(int categoriesId)
{
    this->categoriesId = categoriesId;
}

void CategoriesTableModel::setUpdateDate(int updateDate)
{
    this->updateDate = updateDate;
}

int CategoriesTableModel::getCount() const
{
    return count;
}

void CategoriesTableModel::setCount(int count)
{
    CategoriesTableModel::count = count;
}
