#include "configtablemodel.h"

ConfigTableModel::ConfigTableModel(QString version, QString repoDir, QString repoUrl, QString repoUsername,
                                   QString repoPassword, QString openNotesUuid)
{
    this->version = version;
    this->repoDir = repoDir;
    this->repoUrl = repoUrl;
    this->repoUsername = repoUsername;
    this->repoPassword = repoPassword;
    this->openNotesUuid = openNotesUuid;
}

void ConfigTableModel::setVersion(const QString &version)
{
    ConfigTableModel::version = version;
}

void ConfigTableModel::setRepoDir(const QString &repoDir)
{
    ConfigTableModel::repoDir = repoDir;
}

void ConfigTableModel::setRepoUrl(const QString &repoUrl)
{
    ConfigTableModel::repoUrl = repoUrl;
}

void ConfigTableModel::setRepoUsername(const QString &repoUsername)
{
    ConfigTableModel::repoUsername = repoUsername;
}

void ConfigTableModel::setRepoPassword(const QString &repoPassword)
{
    ConfigTableModel::repoPassword = repoPassword;
}

void ConfigTableModel::setOpenNotesUuid(const QString &openNotesUuid)
{
    ConfigTableModel::openNotesUuid = openNotesUuid;
}

const QString &ConfigTableModel::getVersion() const
{
    return version;
}

const QString &ConfigTableModel::getRepoDir() const
{
    return repoDir;
}

const QString &ConfigTableModel::getRepoUrl() const
{
    return repoUrl;
}

const QString &ConfigTableModel::getRepoUsername() const
{
    return repoUsername;
}

const QString &ConfigTableModel::getRepoPassword() const
{
    return repoPassword;
}

const QString &ConfigTableModel::getOpenNotesUuid() const
{
    return openNotesUuid;
}

int ConfigTableModel::getSidebarSortKey() const
{
    return sidebarSortKey;
}

void ConfigTableModel::setSidebarSortKey(int sidebarSortKey)
{
    ConfigTableModel::sidebarSortKey = sidebarSortKey;
}

const QString &ConfigTableModel::getSidebarSortValue() const
{
    return sidebarSortValue;
}

void ConfigTableModel::setSidebarSortValue(const QString &sidebarSortValue)
{
    ConfigTableModel::sidebarSortValue = sidebarSortValue;
}

int ConfigTableModel::getCategoriesId() const
{
    return categoriesId;
}

void ConfigTableModel::setCategoriesId(int categoriesId)
{
    ConfigTableModel::categoriesId = categoriesId;
}

int ConfigTableModel::getTagsId() const
{
    return tagsId;
}

void ConfigTableModel::setTagsId(int tagsId)
{
    ConfigTableModel::tagsId = tagsId;
}
