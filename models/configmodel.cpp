#include "configmodel.h"
#include "globals.h"
#include "tools.h"

#include <qtinyaes.h>

ConfigModel::ConfigModel(const QString &jsonString)
{
    decrypt(jsonString);
}

ConfigModel::ConfigModel()
{
    mVersion = "version";
    mNewNoteKeySequence = "keySequence/newNote";
    mLockWindowKeySequence = "keySequence/lockWindow";
    mCutWindowKeySequence = "keySequence/cutWindow";
    mRepoDir = "git/repoDir";
    mRepoUrl = "git/repoUrl";
    mRepoEmail = "git/repoEmail";
    mRepoUsername = "git/repoUsername";
    mRepoPassword = "git/repoPassword";
    mRepoStatus = "git/repoStatus";
    mAutoSyncRepoTime = "git/autoSyncRepoTime";
    mOpenNoteUuid = "editor/openNoteUuid";
    mPopupNoteUuidList = "editor/popupNoteUuid";
    mSideSelectedType = "groupTree/selectedType";
    mSideSelectedName = "groupTree/selectedName";
    mAutoLockTime = "lock/autoLockTime";
    mUnlockPassword = "lock/unlockPassword";
    mEditorFont = "editor/font";
    mTheme = "window/theme";
    mSplitterSizes = "window/splitterSizes";
    mNoteSortBasis = "noteList/noteSortBasis";
    mNoteSortType = "noteList/noteSortType";
}

void ConfigModel::init()
{
    Tools::createMkDir(Globals::appDataPath);
    settings = new QSettings(Globals::appConfigPath, QSettings::NativeFormat);
}

QString ConfigModel::encrypt(const QString &string)
{
    QTinyAes aes(QTinyAes::CBC, Globals::aesKey, Globals::aesIv);
    return aes.encrypt(string.toUtf8()).toBase64();
}

const QString &ConfigModel::decrypt(const QString &string) const
{
    QTinyAes aes(QTinyAes::CBC, Globals::aesKey, Globals::aesIv);
    return aes.decrypt(QByteArray::fromBase64(string.toUtf8()));
}

template <typename T>
QVariantList ConfigModel::toVariantList(const QList<T> &list)
{
    QVariantList newList;
    for (auto &&item : list) {
        newList << item;
    }

    return newList;
}

template <typename T>
QList<T> ConfigModel::fromVariantList(const QVariantList &list)
{
    QList<T> newList;
    for (auto &&item : list) {
        newList << item.value<T>();
    }

    return newList;
}


void ConfigModel::setRepoDir(const QString &repoDir)
{
    settings->setValue(mRepoDir, repoDir);
}

void ConfigModel::setRepoUrl(const QString &repoUrl)
{
    settings->setValue(mRepoUrl, repoUrl);
}

void ConfigModel::setRepoUsername(const QString &repoUsername)
{
    settings->setValue(mRepoUsername, repoUsername);
}

void ConfigModel::setRepoPassword(const QString &repoPassword)
{
    settings->setValue(mRepoPassword, encrypt(repoPassword));
}

void ConfigModel::setOpenNoteUuid(const QString &openNoteUuid)
{
    settings->setValue(mOpenNoteUuid, openNoteUuid);
}

const QString ConfigModel::getVersion() const
{
    return settings->value(mVersion, Globals::version).toString();
}

const QString ConfigModel::getRepoDir() const
{
    return settings->value(mRepoDir, "").toString();
}

const QString ConfigModel::getRepoUrl() const
{
    return settings->value(mRepoUrl, "").toString();
}

const QString ConfigModel::getRepoUsername() const
{
    return settings->value(mRepoUsername, "").toString();
}

const QString ConfigModel::getRepoPassword() const
{
    return decrypt(settings->value(mRepoPassword, "").toString());
}

const QString ConfigModel::getOpenNoteUuid() const
{
    return settings->value(mOpenNoteUuid, "").toString();
}

QString ConfigModel::getSideSelectedName() const
{
    return settings->value(mSideSelectedName, "").toString();
}

void ConfigModel::setSideSelected(Gitnoter::GroupType type, const QString &name)
{
    settings->setValue(mSideSelectedType, type);
    settings->setValue(mSideSelectedName, name);
}

void ConfigModel::setSideSelected(GroupModel *groupModel)
{
    settings->setValue(mSideSelectedType, groupModel->getType());
    settings->setValue(mSideSelectedName, groupModel->getName());
}

Gitnoter::RepoStatus ConfigModel::getLocalRepoStatus() const
{
    return (Gitnoter::RepoStatus) settings->value(mRepoStatus, Gitnoter::NoneRepo).toInt();
}

void ConfigModel::setLocalRepoStatus(Gitnoter::RepoStatus status)
{
    settings->setValue(mRepoStatus, status);
}

Gitnoter::GroupType ConfigModel::getSideSelectedType() const
{
    return (Gitnoter::GroupType) settings->value(mSideSelectedType, Gitnoter::All).toInt();
}

const QString ConfigModel::getRepoEmail() const
{
    return settings->value(mRepoEmail, "").toString();
}

void ConfigModel::setRepoEmail(const QString &repoEmail)
{
    settings->setValue(mRepoEmail, repoEmail);
}

int ConfigModel::getAutoSyncRepoTime() const
{
    return settings->value(mAutoSyncRepoTime, 15 * 60 * 1000).toInt();
}

void ConfigModel::setAutoSyncRepoTime(int autoSyncRepoTime)
{
    settings->setValue(mAutoSyncRepoTime, autoSyncRepoTime);
}

int ConfigModel::getAutoLockTime() const
{
    return settings->value(mAutoLockTime, 15 * 60 * 1000).toInt();
}

void ConfigModel::setAutoLockTime(int autoLockTime)
{
    settings->setValue(mAutoLockTime, autoLockTime);
}

const QString ConfigModel::getUnlockPassword() const
{
    return settings->value(mUnlockPassword, "").toString();
}

void ConfigModel::setUnlockPassword(const QString &unlockPassword)
{
    settings->setValue(mUnlockPassword, unlockPassword);
}

QFont ConfigModel::getEditorFont() const
{
    QFont font;
    font.fromString(settings->value(mEditorFont, QFont().defaultFamily()).toString());

    return font;
}

void ConfigModel::setEditorFont(const QFont &font)
{
    settings->setValue(mEditorFont, font.toString());
}

const QString ConfigModel::getNewNoteKeySequence() const
{
    return settings->value(mNewNoteKeySequence, Globals::newNoteKeySequence).toString();
}

void ConfigModel::setNewNoteKeySequence(const QString &newNoteKeySequence)
{
    settings->setValue(mNewNoteKeySequence, newNoteKeySequence);
}

const QString ConfigModel::getLockWindowKeySequence() const
{
    return settings->value(mLockWindowKeySequence, Globals::lockWindowKeySequence).toString();
}

void ConfigModel::setLockWindowKeySequence(const QString &lockWindowKeySequence)
{
    mLockWindowKeySequence = lockWindowKeySequence;
    settings->setValue(mLockWindowKeySequence, lockWindowKeySequence);
}

const QString ConfigModel::getCutWindowKeySequence() const
{
    return settings->value(mCutWindowKeySequence, Globals::cutWindowKeySequence).toString();
}

void ConfigModel::setCutWindowKeySequence(const QString &cutWindowKeySequence)
{
    mCutWindowKeySequence = cutWindowKeySequence;
    settings->setValue(mCutWindowKeySequence, cutWindowKeySequence);
}

ThemeManager::ThemeFlag ConfigModel::getTheme() const
{
    return (ThemeManager::ThemeFlag) settings->value(mTheme, ThemeManager::Default).toInt();
}

void ConfigModel::setTheme(const ThemeManager::ThemeFlag &theme)
{
    mTheme = theme;
    settings->setValue(mTheme, theme);
}

QList<int> ConfigModel::getSplitterSizes()
{
    return fromVariantList<int>(settings->value(mSplitterSizes, {0, 0, 0}).toList());
}

void ConfigModel::setSplitterSizes(const QList<int> &splitterSizes)
{
    settings->setValue(mSplitterSizes, toVariantList(splitterSizes));
}

Gitnoter::SortBasis ConfigModel::getNoteSortBasis() const
{
    return (Gitnoter::SortBasis) settings->value(mNoteSortBasis, Gitnoter::Title).toInt();
}

void ConfigModel::setNoteSortBasis(Gitnoter::SortBasis noteSortBasis)
{
    settings->setValue(mNoteSortBasis, noteSortBasis);
}

Qt::SortOrder ConfigModel::getNoteSortType() const
{
    return (Qt::SortOrder) settings->value(mNoteSortType, Qt::AscendingOrder).toInt();
}

void ConfigModel::setNoteSortType(Qt::SortOrder noteSortType)
{
    settings->setValue(mNoteSortType, noteSortType);
}

void ConfigModel::setNoteSort(Gitnoter::SortBasis noteSortBasis, Qt::SortOrder noteSortType)
{
    settings->setValue(mNoteSortBasis, noteSortBasis);
    settings->setValue(mNoteSortType, noteSortType);
}

void ConfigModel::appendPopupNoteUuid(const QString &noteUuid)
{
    settings->setValue(mPopupNoteUuidList, noteUuid);
}

void ConfigModel::removePopupNoteUuid(const QString &noteUuid)
{
    QStringList uuidList = getPopupNoteUuidList();
    uuidList.removeOne(noteUuid);
    settings->setValue(mPopupNoteUuidList, uuidList);
}

void ConfigModel::clearPopupNoteUuid()
{
    settings->setValue(mPopupNoteUuidList, {});
}

QStringList ConfigModel::getPopupNoteUuidList() const
{
    return settings->value(mPopupNoteUuidList, {}).toStringList();
}
