#include "categoriestablemodel.h"

CategoriseTableModel::CategoriseTableModel()
{
    this->id = 0;
    this->categoriesId = 0;
    this->notesUuid = "";
    this->name = "";
    this->createDate = 0;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriseTableModel::CategoriseTableModel(int id, int categoriesId,  QString notesUuid, QString name, int createDate, int updateDate, int isDefault)
{
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
    this->name = "";
    this->createDate = createDate;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriseTableModel::CategoriseTableModel(int categoriesId, QString name, int createDate, int updateDate, int isDefault)
{
    this->id = 0;
    this->categoriesId = categoriesId;
    this->notesUuid = "";
    this->name = name;
    this->createDate = createDate;
    this->updateDate = updateDate;
    this->isDefault = isDefault;
}

CategoriseTableModel::CategoriseTableModel(QString name)
{
    this->id = 0;
    this->categoriesId = 0;
    this->notesUuid = "";
    this->name = name;
    this->createDate = 0;
    this->updateDate = 0;
    this->isDefault = 0;
}

CategoriseTableModel* CategoriseTableModel::operator+(CategoriseTableModel & categoriseTableModel)
{
    int id = this->getId() != 0 ? this->getId() : categoriseTableModel.getId();
    int categoriesId = this->getCategoriesId() != 0 ? this->getCategoriesId() : categoriseTableModel.getCategoriesId();
    QString notesUuid = this->getNotesUuid().isEmpty() ? categoriseTableModel.getNotesUuid() : this->getNotesUuid();
    QString name = this->getName().isEmpty() ? categoriseTableModel.getName() : this->getName();
    int createDate = this->getCreateDate() != 0 ? this->getCreateDate() : categoriseTableModel.getCreateDate();
    int updateDate = this->getUpdateDate() != 0 ? this->getUpdateDate() : categoriseTableModel.getUpdateDate();
    int isDefault = this->getIsDefault() != 0 ? this->getIsDefault() : categoriseTableModel.getIsDefault();

    return new CategoriseTableModel(id, categoriesId, notesUuid, name, createDate, updateDate, isDefault);
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
