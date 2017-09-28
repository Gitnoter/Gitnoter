#include "configmodel.h"
#include "json.h"
#include "qtinyaes/qtinyaes.h"
#include "globals.h"
#include "tools.h"


ConfigModel::ConfigModel(const QString &jsonString)
{
    unserialize(jsonString);
}

ConfigModel::ConfigModel()
{
    mVersion = Global::version;
    mRepoDir = "";
    mRepoUrl = "";
    mRepoEmail = "";
    mRepoUsername = "";
    mRepoPassword = "";
    mOpenNotesUuid = "";
    mSidebarSortValue = "";
    mSidebarSortKey = 0;
    mIsSelectedClasses = 0;
    mIsLocalRepo = 1;
    mCategoriesName = "";
    mTagsName = "";
}

QString ConfigModel::serialize(const QString &path)
{
    QTinyAes aes(QTinyAes::CBC, Global::aesKey, Global::aesIv);
    QtJson::JsonObject contributor;

    contributor["version"] = mVersion;
    contributor["repoDir"] = mRepoDir;
    contributor["repoUrl"] = mRepoUrl;
    contributor["repoEmail"] = mRepoEmail;
    contributor["repoUsername"] = mRepoUsername;
    contributor["repoPassword"] = aes.encrypt(mRepoPassword.toLocal8Bit()).toBase64();
    contributor["openNotesUuid"] = mOpenNotesUuid;
    contributor["sidebarSortValue"] = mSidebarSortValue;
    contributor["sidebarSortKey"] = mSidebarSortKey;
    contributor["isSelectedClasses"] = mIsSelectedClasses;
    contributor["isLocalRepo"] = mIsLocalRepo;
    contributor["categoriesName"] = mCategoriesName;
    contributor["tagsName"] = mTagsName;

    QString jsonString = QString(QtJson::serialize(contributor).data());
    if (!path.isEmpty()) {
        Tools::writerFile(path, jsonString);
    }

    return jsonString;
}

void ConfigModel::unserialize(const QString &jsonString)
{
    QTinyAes aes(QTinyAes::CBC, Global::aesKey, Global::aesIv);
    QtJson::JsonObject result = QtJson::parse(jsonString).toMap();

    mVersion = result["version"].toString();
    mRepoDir = result["repoDir"].toString();
    mRepoUrl = result["repoUrl"].toString();
    mRepoEmail = result["repoEmail"].toString();
    mRepoUsername = result["repoUsername"].toString();
    mRepoPassword = aes.decrypt(QByteArray::fromBase64(result["repoPassword"].toByteArray()));
    mOpenNotesUuid = result["openNotesUuid"].toString();
    mSidebarSortValue = result["sidebarSortValue"].toString();
    mSidebarSortKey = result["sidebarSortKey"].toInt();
    mIsSelectedClasses = result["isSelectedClasses"].toInt();
    mIsLocalRepo = result["isLocalRepo"].toInt();
    mCategoriesName = result["categoriesName"].toString();
    mTagsName = result["tagsName"].toString();
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

const QString &ConfigModel::getRepoEmail() const
{
    return mRepoEmail;
}

void ConfigModel::setRepoEmail(const QString &repoEmail)
{
    ConfigModel::mRepoEmail = repoEmail;
}
