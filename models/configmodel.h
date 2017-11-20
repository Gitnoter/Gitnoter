#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include "thememanager.h"
#include "notemodel.h"
#include "groupmodel.h"

#include <QObject>
#include <QSettings>

class ConfigModel : public QObject
{
Q_OBJECT

public:
    ConfigModel();

    ConfigModel(const QString &jsonString);

    void init();

    void setRepoDir(const QString &repoDir);

    void setRepoUrl(const QString &repoUrl);

    void setRepoUsername(const QString &repoUsername);

    void setRepoPassword(const QString &repoPassword);

    void setOpenNoteUuid(const QString &openNoteUuid);

    const QString getVersion() const;

    const QString getRepoDir() const;

    const QString getRepoUrl() const;

    const QString getRepoUsername() const;

    const QString getRepoPassword() const;

    const QString getOpenNoteUuid() const;

    QString getSideSelectedName() const;

    void setSideSelected(Gitnoter::GroupType type, const QString &name);

    void setSideSelected(GroupModel *groupModel);

    Gitnoter::RepoStatus getLocalRepoStatus() const;

    void setLocalRepoStatus(Gitnoter::RepoStatus status);

    Gitnoter::GroupType getSideSelectedType() const;

    const QString getRepoEmail() const;

    void setRepoEmail(const QString &repoEmail);

    int getAutoSyncRepoTime() const;

    void setAutoSyncRepoTime(int autoSyncRepoTime);

    int getAutoLockTime() const;

    void setAutoLockTime(int autoLockTime);

    const QString getUnlockPassword() const;

    void setUnlockPassword(const QString &unlockPassword);

    QFont getEditorFont() const;

    void setEditorFont(const QFont &font);

    const QString getNewNoteKeySequence() const;

    void setNewNoteKeySequence(const QString &newNoteKeySequence);

    const QString getLockWindowKeySequence() const;

    void setLockWindowKeySequence(const QString &lockWindowKeySequence);

    const QString getCutWindowKeySequence() const;

    void setCutWindowKeySequence(const QString &cutWindowKeySequence);

    ThemeManager::ThemeFlag getTheme() const;

    void setTheme(const ThemeManager::ThemeFlag &theme);

    QList<int> getSplitterSizes();

    void setSplitterSizes(const QList<int> &splitterSizes);

    Gitnoter::SortBasis getNoteSortBasis() const;

    void setNoteSortBasis(Gitnoter::SortBasis noteSortBasis);

    Qt::SortOrder getNoteSortType() const;

    void setNoteSortType(Qt::SortOrder noteSortType);

    void setNoteSort(Gitnoter::SortBasis noteSortBasis, Qt::SortOrder noteSortType);

    void appendPopupNoteUuid(const QString &noteUuid);

    void removePopupNoteUuid(const QString &noteUuid);

    void clearPopupNoteUuid();

    QStringList getPopupNoteUuidList() const;

private:
    template <typename T>
    QVariantList toVariantList(const QList<T> &list);

    template <typename T>
    QList<T> fromVariantList(const QVariantList &list);

    QString encrypt(const QString &string);

    const QString &decrypt(const QString &string) const;

private:
    QSettings *settings;

    QString mVersion;
    QString mRepoDir;
    QString mRepoUrl;
    QString mRepoEmail;
    QString mRepoUsername;
    QString mRepoPassword;
    QString mOpenNoteUuid;
    QString mPopupNoteUuidList;

    QString mRepoStatus;
    // type: 1 - 第一层及
    //      name: all, recent, unclassified, trash, category, tag
    // type: 2 - 第二层级
    //      name: categoryName
    // type: 3
    //      name: tagName
    QString mSideSelectedType;
    QString mSideSelectedName;

    QString mNoteSortBasis;
    QString mNoteSortType;

    QString mAutoSyncRepoTime;
    QString mAutoLockTime;
    QString mUnlockPassword;

    QString mEditorFont;

    QString mNewNoteKeySequence;
    QString mLockWindowKeySequence;
    QString mCutWindowKeySequence;

    QString mTheme;
    QString mSplitterSizes;

};

#endif // CONFIGMODEL_H
