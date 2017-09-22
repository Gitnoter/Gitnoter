#include "configmodel.h"

ConfigModel::ConfigModel(QString version, QString repoDir, QString repoUrl, QString repoUsername,
                                   QString repoPassword, QString openNotesUuid)
{
    this->version = version;
    this->repoDir = repoDir;
    this->repoUrl = repoUrl;
    this->repoUsername = repoUsername;
    this->repoPassword = repoPassword;
    this->openNotesUuid = openNotesUuid;
}

void ConfigModel::setVersion(const QString &version)
{
    ConfigModel::version = version;
}

void ConfigModel::setRepoDir(const QString &repoDir)
{
    ConfigModel::repoDir = repoDir;
}

void ConfigModel::setRepoUrl(const QString &repoUrl)
{
    ConfigModel::repoUrl = repoUrl;
}

void ConfigModel::setRepoUsername(const QString &repoUsername)
{
    ConfigModel::repoUsername = repoUsername;
}

void ConfigModel::setRepoPassword(const QString &repoPassword)
{
    ConfigModel::repoPassword = repoPassword;
}

void ConfigModel::setOpenNotesUuid(const QString &openNotesUuid)
{
    ConfigModel::openNotesUuid = openNotesUuid;
}

const QString &ConfigModel::getVersion() const
{
    return version;
}

const QString &ConfigModel::getRepoDir() const
{
    return repoDir;
}

const QString &ConfigModel::getRepoUrl() const
{
    return repoUrl;
}

const QString &ConfigModel::getRepoUsername() const
{
    return repoUsername;
}

const QString &ConfigModel::getRepoPassword() const
{
    return repoPassword;
}

const QString &ConfigModel::getOpenNotesUuid() const
{
    return openNotesUuid;
}

int ConfigModel::getSidebarSortKey() const
{
    return sidebarSortKey;
}

void ConfigModel::setSidebarSortKey(int sidebarSortKey)
{
    ConfigModel::sidebarSortKey = sidebarSortKey;
}

const QString &ConfigModel::getSidebarSortValue() const
{
    return sidebarSortValue;
}

void ConfigModel::setSidebarSortValue(const QString &sidebarSortValue)
{
    ConfigModel::sidebarSortValue = sidebarSortValue;
}

QString ConfigModel::getCategoriesName() const
{
    return categoriesName;
}

void ConfigModel::setCategoriesName(const QString &categoriesName)
{
    ConfigModel::isSelectedClasses = 1;
    ConfigModel::categoriesName = categoriesName;
}

QString ConfigModel::getTagsName() const
{
    return tagsName;
}

void ConfigModel::setTagsName(const QString &tagsName)
{
    ConfigModel::isSelectedClasses = 2;
    ConfigModel::tagsName = tagsName;
}

int ConfigModel::getIsLocalRepo() const
{
    return isLocalRepo;
}

void ConfigModel::setIsLocalRepo(int isLocalRepo)
{
    ConfigModel::isLocalRepo = isLocalRepo;
}

int ConfigModel::getIsSelectedClasses() const
{
    return isSelectedClasses;
}

void ConfigModel::setIsSelectedClasses(int isSelectedClasses)
{
    ConfigModel::isSelectedClasses = isSelectedClasses;
}
