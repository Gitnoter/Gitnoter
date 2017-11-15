#include "configmodel.h"
#include "globals.h"
#include "tools.h"

#include <qtinyaes.h>
#include <json.h>

ConfigModel::ConfigModel(const QString &jsonString)
{
    unserialize(jsonString);
}

ConfigModel::ConfigModel()
{
    mVersion = Globals::version;
    mNewNoteKeySequence = Globals::newNoteKeySequence;
    mLockWindowKeySequence = Globals::lockWindowKeySequence;
    mCutWindowKeySequence = Globals::cutWindowKeySequence;
    mRepoDir = "";
    mRepoUrl = "";
    mRepoEmail = "";
    mRepoUsername = "";
    mRepoPassword = "";
    mOpenNoteUuid = "";
    mSidebarSortValue = "";
    mSidebarSortKey = -1;
    mSideSelectedType = Gitnoter::All;
    mLocalRepoStatus = 0;
    mSideSelectedName = "all";
    mAutoSyncRepoTime = 15 * 60 * 1000;
    mAutoLockTime = 15 * 60 * 1000;
    mUnlockPassword = "";
    mFontPixelSize = 14;
    mTheme = ThemeManager::ThemeFlag::Default;
    splitterSizes = {0, 0, 0};

#ifdef Q_OS_MAC
    mFontFamily = "Helvetica Neue";
#elif Q_OS_WIN
    mFontFamily = "Courier New";
#elif Q_OS_LINUX
    mFontFamily = "Menlo2";
#else

#endif
}

void ConfigModel::init()
{
    Tools::createMkDir(Globals::appDataPath);
    QString jsonString = Tools::readerFile(Globals::appConfigPath);
    if (jsonString.isEmpty()) {
        serialize(Globals::appConfigPath);
    }
    else {
        unserialize(jsonString);
    }
}

QString ConfigModel::serialize(const QString &path)
{
    QTinyAes aes(QTinyAes::CBC, Globals::aesKey, Globals::aesIv);
    QtJson::JsonObject contributor;

    contributor["version"] = mVersion;
    contributor["repoDir"] = mRepoDir;
    contributor["repoUrl"] = mRepoUrl;
    contributor["repoEmail"] = mRepoEmail;
    contributor["repoUsername"] = mRepoUsername;
    contributor["repoPassword"] = mRepoPassword;
    contributor["openNotesUuid"] = mOpenNoteUuid;
    contributor["sidebarSortValue"] = mSidebarSortValue;
    contributor["sidebarSortKey"] = mSidebarSortKey;
    contributor["sideSelectedType"] = mSideSelectedType;
    contributor["localRepoStatus"] = mLocalRepoStatus;
    contributor["sideSelectedName"] = mSideSelectedName;
    contributor["autoSyncRepoTime"] = mAutoSyncRepoTime;
    contributor["autoLockTime"] = mAutoLockTime;
    contributor["unlockPassword"] = mUnlockPassword;
    contributor["fontFamily"] = mFontFamily;
    contributor["fontPixelSize"] = mFontPixelSize;
    contributor["newNoteKeySequence"] = mNewNoteKeySequence;
    contributor["lockWindowKeySequence"] = mLockWindowKeySequence;
    contributor["cutWindowKeySequence"] = mCutWindowKeySequence;
    contributor["theme"] = mTheme;

    QtJson::JsonArray splitterSizesArray;
    for (int i = 0; i < splitterSizes.length(); ++i) {
        splitterSizesArray.append(splitterSizes[i]);
    }
    contributor["splitterSizes"] = splitterSizesArray;

    QString jsonString = aes.encrypt(QtJson::serialize(contributor)).toBase64();
    if (!path.isEmpty()) {
        Tools::writerFile(path, jsonString);
    }

    return jsonString;
}

void ConfigModel::unserialize(const QString &jsonString)
{
    QTinyAes aes(QTinyAes::CBC, Globals::aesKey, Globals::aesIv);
    QtJson::JsonObject result = QtJson::parse(aes.decrypt(QByteArray::fromBase64(jsonString.toUtf8()))).toMap();

    mVersion = result["version"].toString();
    mRepoDir = result["repoDir"].toString();
    mRepoUrl = result["repoUrl"].toString();
    mRepoEmail = result["repoEmail"].toString();
    mRepoUsername = result["repoUsername"].toString();
    mRepoPassword = result["repoPassword"].toString();
    mOpenNoteUuid = result["openNotesUuid"].toString();
    mSidebarSortValue = result["sidebarSortValue"].toString();
    mSidebarSortKey = result["sidebarSortKey"].toInt();
    mSideSelectedType = (Gitnoter::GroupType) result["sideSelectedType"].toInt();
    mLocalRepoStatus = result["localRepoStatus"].toInt();
    mSideSelectedName = result["sideSelectedName"].toString();
    mAutoSyncRepoTime = result["autoSyncRepoTime"].toInt();
    mAutoLockTime = result["autoLockTime"].toInt();
    mUnlockPassword = result["unlockPassword"].toString();
    mFontFamily = result["fontFamily"].toString();
    mFontPixelSize = result["fontPixelSize"].toInt();
    mNewNoteKeySequence = result["newNoteKeySequence"].toString();
    mLockWindowKeySequence = result["lockWindowKeySequence"].toString();
    mCutWindowKeySequence = result["cutWindowKeySequence"].toString();
    mTheme = (ThemeManager::ThemeFlag) result["cutWindowKeySequence"].toInt();

    splitterSizes.clear();
    QtJson::JsonArray splitterSizesArray = result["splitterSizes"].toList();
    for (int i = 0; i < splitterSizesArray.length(); ++i) {
        splitterSizes.append(splitterSizesArray[i].toInt());
    }
}

void ConfigModel::setRepoDir(const QString &repoDir)
{
    ConfigModel::mRepoDir = repoDir;
    serialize(Globals::appConfigPath);
}

void ConfigModel::setRepoUrl(const QString &repoUrl)
{
    ConfigModel::mRepoUrl = repoUrl;
    serialize(Globals::appConfigPath);
}

void ConfigModel::setRepoUsername(const QString &repoUsername)
{
    ConfigModel::mRepoUsername = repoUsername;
    serialize(Globals::appConfigPath);
}

void ConfigModel::setRepoPassword(const QString &repoPassword)
{
    ConfigModel::mRepoPassword = repoPassword;
    serialize(Globals::appConfigPath);
}

void ConfigModel::setOpenNoteUuid(const QString &openNoteUuid)
{
    ConfigModel::mOpenNoteUuid = openNoteUuid;
    serialize(Globals::appConfigPath);
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

const QString &ConfigModel::getOpenNoteUuid() const
{
    return mOpenNoteUuid;
}

int ConfigModel::getSidebarSortKey() const
{
    return mSidebarSortKey;
}

void ConfigModel::setSidebarSortKey(int sidebarSortKey)
{
    ConfigModel::mSidebarSortKey = sidebarSortKey;
    serialize(Globals::appConfigPath);
}

const QString &ConfigModel::getSidebarSortValue() const
{
    return mSidebarSortValue;
}

void ConfigModel::setSidebarSortValue(const QString &sidebarSortValue)
{
    ConfigModel::mSidebarSortValue = sidebarSortValue;
    serialize(Globals::appConfigPath);
}

QString ConfigModel::getSideSelectedName() const
{
    return mSideSelectedName;
}

void ConfigModel::setSideSelected(Gitnoter::GroupType type, const QString &name)
{
    ConfigModel::mSideSelectedType = type;
    ConfigModel::mSideSelectedName = name;
    serialize(Globals::appConfigPath);
}

void ConfigModel::setSideSelected(GroupModel *groupModel)
{
    ConfigModel::mSideSelectedType = groupModel->getType();
    ConfigModel::mSideSelectedName = groupModel->getName();
    serialize(Globals::appConfigPath);
}

int ConfigModel::getLocalRepoStatus() const
{
    return mLocalRepoStatus;
}

void ConfigModel::setLocalRepoStatus(int localRepoStatus)
{
    ConfigModel::mLocalRepoStatus = localRepoStatus;
    serialize(Globals::appConfigPath);
}

Gitnoter::GroupType ConfigModel::getSideSelectedType() const
{
    return mSideSelectedType;
}

const QString &ConfigModel::getRepoEmail() const
{
    return mRepoEmail;
}

void ConfigModel::setRepoEmail(const QString &repoEmail)
{
    ConfigModel::mRepoEmail = repoEmail;
    serialize(Globals::appConfigPath);
}

int ConfigModel::getAutoSyncRepoTime() const
{
    return mAutoSyncRepoTime;
}

void ConfigModel::setAutoSyncRepoTime(int autoSyncRepoTime)
{
    ConfigModel::mAutoSyncRepoTime = autoSyncRepoTime;
    serialize(Globals::appConfigPath);
}

int ConfigModel::getAutoLockTime() const
{
    return mAutoLockTime;
}

void ConfigModel::setAutoLockTime(int autoLockTime)
{
    ConfigModel::mAutoLockTime = autoLockTime;
    serialize(Globals::appConfigPath);
}

const QString &ConfigModel::getUnlockPassword() const
{
    return mUnlockPassword;
}

void ConfigModel::setUnlockPassword(const QString &unlockPassword)
{
    ConfigModel::mUnlockPassword = unlockPassword;
    serialize(Globals::appConfigPath);
}

const QString &ConfigModel::getFontFamily() const
{
    return mFontFamily;
}

void ConfigModel::setFontFamily(const QString &fontFamily)
{
    ConfigModel::mFontFamily = fontFamily;
    serialize(Globals::appConfigPath);
}

int ConfigModel::getFontPixelSize() const
{
    return mFontPixelSize;
}

void ConfigModel::setFontPixelSize(int fontPixelSize)
{
    ConfigModel::mFontPixelSize = fontPixelSize;
    serialize(Globals::appConfigPath);
}

const QString &ConfigModel::getNewNoteKeySequence() const
{
    return mNewNoteKeySequence;
}

void ConfigModel::setNewNoteKeySequence(const QString &newNoteKeySequence)
{
    ConfigModel::mNewNoteKeySequence = newNoteKeySequence;
    serialize(Globals::appConfigPath);
}

const QString &ConfigModel::getLockWindowKeySequence() const
{
    return mLockWindowKeySequence;
}

void ConfigModel::setLockWindowKeySequence(const QString &lockWindowKeySequence)
{
    ConfigModel::mLockWindowKeySequence = lockWindowKeySequence;
    serialize(Globals::appConfigPath);
}

const QString &ConfigModel::getCutWindowKeySequence() const
{
    return mCutWindowKeySequence;
}

void ConfigModel::setCutWindowKeySequence(const QString &cutWindowKeySequence)
{
    ConfigModel::mCutWindowKeySequence = cutWindowKeySequence;
    serialize(Globals::appConfigPath);
}

const ThemeManager::ThemeFlag &ConfigModel::getTheme() const
{
    return mTheme;
}

void ConfigModel::setTheme(const ThemeManager::ThemeFlag &theme)
{
    mTheme = theme;
    serialize(Globals::appConfigPath);
}

const QList<int> &ConfigModel::getSplitterSizes() const
{
    return splitterSizes;
}

void ConfigModel::setSplitterSizes(const QList<int> &splitterSizes)
{
    this->splitterSizes = splitterSizes;
    serialize(Globals::appConfigPath);
}
