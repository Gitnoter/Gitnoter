#include "categorisetablemodel.h"

CategoriseTableModel::CategoriseTableModel(QString name, int createDate, int isDefault) {
    this->name = name;
    this->insertDate = createDate;
    this->isDefault = isDefault;
}

CategoriseTableModel::CategoriseTableModel(QString name) {
    this->name = name;
}

QString CategoriseTableModel::getName() {
    return this->name;
}

int CategoriseTableModel::getCreateDate() {
    return this->insertDate;
}

int CategoriseTableModel::getIsDefault() {
    return this->isDefault;
}

void CategoriseTableModel::setName(QString name) {
    this->name = name;
}

void CategoriseTableModel::setCreateDate(int insertDate) {
    this->insertDate = insertDate;
}

void CategoriseTableModel::setIsDefault(int isDefault) {
    this->isDefault = isDefault;
}
