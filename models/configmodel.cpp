#include "configmodel.h"
#include "json.h"


ConfigModel::ConfigModel(const QString &jsonString)
{
    unserialize(jsonString);
}

ConfigModel::ConfigModel(QString version, QString repoDir, QString repoUrl, QString repoUsername,
                         QString repoPassword, QString openNotesUuid)
{
    mVersion = version;
    mRepoDir = repoDir;
    mRepoUrl = repoUrl;
    mRepoUsername = repoUsername;
    mRepoPassword = repoPassword;
    mOpenNotesUuid = openNotesUuid;
}

QString ConfigModel::serialize()
{
    QtJson::JsonObject contributor;
    contributor["version"] = mVersion;

    contributor["version"] = mVersion;
    contributor["repoDir"] = mRepoDir;
    contributor["repoUrl"] = mRepoUrl;
    contributor["repoUsername"] = mRepoUsername;
    contributor["repoPassword"] = mRepoPassword;
    contributor["openNotesUuid"] = mOpenNotesUuid;
    contributor["sidebarSortValue"] = mSidebarSortValue;
    contributor["sidebarSortKey"] = mSidebarSortKey;
    contributor["isSelectedClasses"] = mIsSelectedClasses;
    contributor["isLocalRepo"] = mIsLocalRepo;
    contributor["categoriesName"] = mCategoriesName;
    contributor["tagsName"] = mTagsName;

    return QString(QtJson::serialize(contributor).data());
}

void ConfigModel::unserialize(const QString &jsonString)
{
    QtJson::JsonObject result = QtJson::parse(jsonString).toMap();

    mVersion = result["version"].toString();
    mRepoDir = result["repoDir"].toString();
    mRepoUrl = result["repoUrl"].toString();
    mRepoUsername = result["repoUsername"].toString();
    mRepoPassword = result["repoPassword"].toString();
    mOpenNotesUuid = result["openNotesUuid"].toString();
    mSidebarSortValue = result["sidebarSortValue"].toString();
    mSidebarSortKey = result["sidebarSortKey"].toInt();
    mIsSelectedClasses = result["isSelectedClasses"].toInt();
    mIsLocalRepo = result["isLocalRepo"].toInt();
    mCategoriesName = result["categoriesName"].toString();
    mTagsName = result["tagsName"].toString();
}

void ConfigModel::setVersion(const QString &version)
{
    ConfigModel::mVersion = version;
}

void ConfigModel::setRepoDir(const QString &repoDir)
{
    ConfigModel::mRepoDir = repoDir;
}

void ConfigModel::setRepoUrl(const QString &repoUrl)
{
    ConfigModel::mRepoUrl = repoUrl;
}

void ConfigModel::setRepoUsername(const QString &repoUsername)
{
    ConfigModel::mRepoUsername = repoUsername;
}

void ConfigModel::setRepoPassword(const QString &repoPassword)
{
    ConfigModel::mRepoPassword = repoPassword;
}

void ConfigModel::setOpenNotesUuid(const QString &openNotesUuid)
{
    ConfigModel::mOpenNotesUuid = openNotesUuid;
}

const QString &ConfigModel::getVersion() const
{
    return mVersion;
}

const QString &ConfigModel::getRepoDir() const
{
    return mRepoDir;
}

const QString &ConfigModel::getRepoUrl() const
{
    return mRepoUrl;
}

const QString &ConfigModel::getRepoUsername() const
{
    return mRepoUsername;
}

const QString &ConfigModel::getRepoPassword() const
{
    return mRepoPassword;
}

const QString &ConfigModel::getOpenNotesUuid() const
{
    return mOpenNotesUuid;
}

int ConfigModel::getSidebarSortKey() const
{
    return mSidebarSortKey;
}

void ConfigModel::setSidebarSortKey(int sidebarSortKey)
{
    ConfigModel::mSidebarSortKey = sidebarSortKey;
}

const QString &ConfigModel::getSidebarSortValue() const
{
    return mSidebarSortValue;
}

void ConfigModel::setSidebarSortValue(const QString &sidebarSortValue)
{
    ConfigModel::mSidebarSortValue = sidebarSortValue;
}

QString ConfigModel::getCategoriesName() const
{
    return mCategoriesName;
}

void ConfigModel::setCategoriesName(const QString &categoriesName)
{
    ConfigModel::mIsSelectedClasses = 1;
    ConfigModel::mCategoriesName = categoriesName;
}

QString ConfigModel::getTagsName() const
{
    return mTagsName;
}

void ConfigModel::setTagsName(const QString &tagsName)
{
    ConfigModel::mIsSelectedClasses = 2;
    ConfigModel::mTagsName = tagsName;
}

int ConfigModel::getIsLocalRepo() const
{
    return mIsLocalRepo;
}

void ConfigModel::setIsLocalRepo(int isLocalRepo)
{
    ConfigModel::mIsLocalRepo = isLocalRepo;
}

int ConfigModel::getIsSelectedClasses() const
{
    return mIsSelectedClasses;
}

void ConfigModel::setIsSelectedClasses(int isSelectedClasses)
{
    ConfigModel::mIsSelectedClasses = isSelectedClasses;
}
