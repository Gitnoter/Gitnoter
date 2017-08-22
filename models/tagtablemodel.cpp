#include "tagtablemodel.h"

TagTableModel::TagTableModel(QString name, int createDate) {
    this->name = name;
    this->insertDate = createDate;
}

void TagTableModel::setName(QString name) {
    this->name = name;
}

void TagTableModel::setCreateDate(int insertDate) {
    this->insertDate = insertDate;
}

QString TagTableModel::getName() {
    return this->name;
}

int TagTableModel::getCreateDate() {
    return this->insertDate;
}

TagTableModel::TagTableModel(QString name) {
    this->name = name;
}

