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
    mSidebarSortKey = -1;
    mIsSelectedClasses = 0;
    mLocalRepoStatus = 0;
    mCategoriesName = "";
    mTagsName = "";
    mAutoSyncRepoTime = 15 * 60 * 1000;
    mAutoLockTime = 15 * 60 * 1000;
    mUnlockPassword = "";
#ifdef Q_OS_MAC
    mFontFamily = "Helvetica Neue";
#elif Q_OS_WIN
    mFontFamily = "Courier New";
#elif Q_OS_LINUX
    mFontFamily = "Menlo2";
#else

#endif
    mFontPixelSize = 14;
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
    contributor["repoPassword"] = mRepoPassword;
    contributor["openNotesUuid"] = mOpenNotesUuid;
    contributor["sidebarSortValue"] = mSidebarSortValue;
    contributor["sidebarSortKey"] = mSidebarSortKey;
    contributor["isSelectedClasses"] = mIsSelectedClasses;
    contributor["localRepoStatus"] = mLocalRepoStatus;
    contributor["categoriesName"] = mCategoriesName;
    contributor["tagsName"] = mTagsName;
    contributor["autoSyncRepoTime"] = mAutoSyncRepoTime;
    contributor["autoLockTime"] = mAutoLockTime;
    contributor["unlockPassword"] = mUnlockPassword;
    contributor["fontFamily"] = mFontFamily;
    contributor["fontPixelSize"] = mFontPixelSize;

    QString jsonString = aes.encrypt(QtJson::serialize(contributor)).toBase64();

    if (!path.isEmpty()) {
        Tools::writerFile(path, jsonString);
    }

    return jsonString;
}

void ConfigModel::unserialize(const QString &jsonString)
{
    QTinyAes aes(QTinyAes::CBC, Global::aesKey, Global::aesIv);
    QtJson::JsonObject result = QtJson::parse(aes.decrypt(QByteArray::fromBase64(jsonString.toUtf8()))).toMap();

    mVersion = result["version"].toString();
    mRepoDir = result["repoDir"].toString();
    mRepoUrl = result["repoUrl"].toString();
    mRepoEmail = result["repoEmail"].toString();
    mRepoUsername = result["repoUsername"].toString();
    mRepoPassword = result["repoPassword"].toString();
    mOpenNotesUuid = result["openNotesUuid"].toString();
    mSidebarSortValue = result["sidebarSortValue"].toString();
    mSidebarSortKey = result["sidebarSortKey"].toInt();
    mIsSelectedClasses = result["isSelectedClasses"].toInt();
    mLocalRepoStatus = result["localRepoStatus"].toInt();
    mCategoriesName = result["categoriesName"].toString();
    mTagsName = result["tagsName"].toString();
    mAutoSyncRepoTime = result["autoSyncRepoTime"].toInt();
    mAutoLockTime = result["autoLockTime"].toInt();
    mUnlockPassword = result["unlockPassword"].toString();
    mFontFamily = result["fontFamily"].toString();
    mFontPixelSize = result["fontPixelSize"].toInt();
}

void ConfigModel::setRepoDir(const QString &repoDir)
{
    ConfigModel::mRepoDir = repoDir;
    serialize(Global::appConfigPath);
}

void ConfigModel::setRepoUrl(const QString &repoUrl)
{
    ConfigModel::mRepoUrl = repoUrl;
    serialize(Global::appConfigPath);
}

void ConfigModel::setRepoUsername(const QString &repoUsername)
{
    ConfigModel::mRepoUsername = repoUsername;
    serialize(Global::appConfigPath);
}

void ConfigModel::setRepoPassword(const QString &repoPassword)
{
    ConfigModel::mRepoPassword = repoPassword;
    serialize(Global::appConfigPath);
}

void ConfigModel::setOpenNotesUuid(const QString &openNotesUuid)
{
    ConfigModel::mOpenNotesUuid = openNotesUuid;
    serialize(Global::appConfigPath);
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
    serialize(Global::appConfigPath);
}

const QString &ConfigModel::getSidebarSortValue() const
{
    return mSidebarSortValue;
}

void ConfigModel::setSidebarSortValue(const QString &sidebarSortValue)
{
    ConfigModel::mSidebarSortValue = sidebarSortValue;
    serialize(Global::appConfigPath);
}

QString ConfigModel::getCategoriesName() const
{
    return mCategoriesName;
}

void ConfigModel::setCategoriesName(const QString &categoriesName)
{
    ConfigModel::mIsSelectedClasses = 1;
    ConfigModel::mCategoriesName = categoriesName;
    serialize(Global::appConfigPath);
}

QString ConfigModel::getTagsName() const
{
    return mTagsName;
}

void ConfigModel::setTagsName(const QString &tagsName)
{
    ConfigModel::mIsSelectedClasses = 2;
    ConfigModel::mTagsName = tagsName;
    serialize(Global::appConfigPath);
}

int ConfigModel::getLocalRepoStatus() const
{
    return mLocalRepoStatus;
}

void ConfigModel::setLocalRepoStatus(int localRepoStatus)
{
    ConfigModel::mLocalRepoStatus = localRepoStatus;
    serialize(Global::appConfigPath);
}

int ConfigModel::getIsSelectedClasses() const
{
    return mIsSelectedClasses;
}

void ConfigModel::setIsSelectedClasses(int isSelectedClasses)
{
    ConfigModel::mIsSelectedClasses = isSelectedClasses;
    serialize(Global::appConfigPath);
}

const QString &ConfigModel::getRepoEmail() const
{
    return mRepoEmail;
}

void ConfigModel::setRepoEmail(const QString &repoEmail)
{
    ConfigModel::mRepoEmail = repoEmail;
    serialize(Global::appConfigPath);
}

int ConfigModel::getAutoSyncRepoTime() const
{
    return mAutoSyncRepoTime;
}

void ConfigModel::setAutoSyncRepoTime(int autoSyncRepoTime)
{
    ConfigModel::mAutoSyncRepoTime = autoSyncRepoTime;
    serialize(Global::appConfigPath);
}

int ConfigModel::getAutoLockTime() const
{
    return mAutoLockTime;
}

void ConfigModel::setAutoLockTime(int autoLockTime)
{
    ConfigModel::mAutoLockTime = autoLockTime;
    serialize(Global::appConfigPath);
}

const QString &ConfigModel::getUnlockPassword() const
{
    return mUnlockPassword;
}

void ConfigModel::setUnlockPassword(const QString &unlockPassword)
{
    ConfigModel::mUnlockPassword = unlockPassword;
    serialize(Global::appConfigPath);
}

const QString &ConfigModel::getFontFamily() const
{
    return mFontFamily;
}

void ConfigModel::setFontFamily(const QString &fontFamily)
{
    ConfigModel::mFontFamily = fontFamily;
    serialize(Global::appConfigPath);
}

int ConfigModel::getFontPixelSize() const
{
    return mFontPixelSize;
}

void ConfigModel::setFontPixelSize(int fontPixelSize)
{
    ConfigModel::mFontPixelSize = fontPixelSize;
    serialize(Global::appConfigPath);
}
