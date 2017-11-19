#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include "thememanager.h"
#include "notemodel.h"
#include "groupmodel.h"

#include <QObject>

class ConfigModel : public QObject
{
Q_OBJECT

public:
    ConfigModel();

    ConfigModel(const QString &jsonString);

    void init();

    QString serialize(const QString &path = "");

    void unserialize(const QString &jsonString);

    void setRepoDir(const QString &repoDir);

    void setRepoUrl(const QString &repoUrl);

    void setRepoUsername(const QString &repoUsername);

    void setRepoPassword(const QString &repoPassword);

    void setOpenNoteUuid(const QString &openNoteUuid);

    const QString &getVersion() const;

    const QString &getRepoDir() const;

    const QString &getRepoUrl() const;

    const QString &getRepoUsername() const;

    const QString &getRepoPassword() const;

    const QString &getOpenNoteUuid() const;

    QString getSideSelectedName() const;

    void setSideSelected(Gitnoter::GroupType type, const QString &name);

    void setSideSelected(GroupModel *groupModel);

    Gitnoter::RepoStatus getLocalRepoStatus() const;

    void setLocalRepoStatus(Gitnoter::RepoStatus status);

    Gitnoter::GroupType getSideSelectedType() const;

    const QString &getRepoEmail() const;

    void setRepoEmail(const QString &repoEmail);

    int getAutoSyncRepoTime() const;

    void setAutoSyncRepoTime(int autoSyncRepoTime);

    int getAutoLockTime() const;

    void setAutoLockTime(int autoLockTime);

    const QString &getUnlockPassword() const;

    void setUnlockPassword(const QString &unlockPassword);

    const QString &getFontFamily() const;

    void setFontFamily(const QString &fontFamily);

    int getFontPixelSize() const;

    void setFontPixelSize(int fontPixelSize);

    const QString &getNewNoteKeySequence() const;

    void setNewNoteKeySequence(const QString &newNoteKeySequence);

    const QString &getLockWindowKeySequence() const;

    void setLockWindowKeySequence(const QString &lockWindowKeySequence);

    const QString &getCutWindowKeySequence() const;

    void setCutWindowKeySequence(const QString &cutWindowKeySequence);

    const ThemeManager::ThemeFlag &getTheme() const;

    void setTheme(const ThemeManager::ThemeFlag &theme);

    const QList<int> &getSplitterSizes() const;

    void setSplitterSizes(const QList<int> &splitterSizes);

    Gitnoter::SortBasis getNoteSortBasis() const;

    void setNoteSortBasis(Gitnoter::SortBasis noteSortBasis);

    Qt::SortOrder getNoteSortType() const;

    void setNoteSortType(Qt::SortOrder noteSortType);

    void setNoteSort(Gitnoter::SortBasis noteSortBasis, Qt::SortOrder noteSortType);

private:
    QString mVersion;
    QString mRepoDir;
    QString mRepoUrl;
    QString mRepoEmail;
    QString mRepoUsername;
    QString mRepoPassword;
    QString mOpenNoteUuid;

    Gitnoter::RepoStatus mLocalRepoStatus;
    // type: 1 - 第一层及
    //      name: all, recent, unclassified, trash, category, tag
    // type: 2 - 第二层级
    //      name: categoryName
    // type: 3
    //      name: tagName
    Gitnoter::GroupType mSideSelectedType;
    QString mSideSelectedName;

    Gitnoter::SortBasis mNoteSortBasis;
    Qt::SortOrder mNoteSortType;

    int mAutoSyncRepoTime;
    int mAutoLockTime;
    QString mUnlockPassword;

    QString mFontFamily;
    int mFontPixelSize;

    QString mNewNoteKeySequence;
    QString mLockWindowKeySequence;
    QString mCutWindowKeySequence;

    ThemeManager::ThemeFlag mTheme;
    QList<int> mSplitterSizes;

};

#endif // CONFIGMODEL_H
