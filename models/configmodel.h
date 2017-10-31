#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include "thememanager.h"
#include "notemodellist.h"

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

    void setOpenNotesUuid(const QString &openNotesUuid);

    const QString &getVersion() const;

    const QString &getRepoDir() const;

    const QString &getRepoUrl() const;

    const QString &getRepoUsername() const;

    const QString &getRepoPassword() const;

    const QString &getOpenNotesUuid() const;

    int getSidebarSortKey() const;

    void setSidebarSortKey(int sidebarSortKey);

    const QString &getSidebarSortValue() const;

    void setSidebarSortValue(const QString &sidebarSortValue);

    QString getCategory() const;

    void setCategory(const QString &category);

    QString getTagsName() const;

    void setTagsName(const QString &tagsName);

    int getLocalRepoStatus() const;

    void setLocalRepoStatus(int localRepoStatus);

    int getIsSelectedClasses() const;

    void setIsSelectedClasses(int isSelectedClasses);

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

    NoteModel *getOpenNoteModel() const;

    void setOpenNoteModel(NoteModel *noteModel);

private:
    QString mVersion;
    QString mRepoDir;
    QString mRepoUrl;
    QString mRepoEmail;
    QString mRepoUsername;
    QString mRepoPassword;
    QString mOpenNotesUuid;
    // -1: 没有选择没任何排序, 0: 选择名称排序, 1: 选择时间排序
    int mSidebarSortKey;
    QString mSidebarSortValue;
    // 0: 还没有初始化仓库, 1: 初始化远程仓库, 2: 初始化本地仓库
    int mLocalRepoStatus;
    // 0: 没有选择任何类型, 1: 选择笔记本, 2: 选择标签
    int mIsSelectedClasses;
    QString mCategory;
    QString mTagsName;

    int mAutoSyncRepoTime;
    int mAutoLockTime;
    QString mUnlockPassword;

    QString mFontFamily;
    int mFontPixelSize;

    QString mNewNoteKeySequence;
    QString mLockWindowKeySequence;
    QString mCutWindowKeySequence;

    ThemeManager::ThemeFlag mTheme;
    QList<int> splitterSizes;

    NoteModel *openNoteModel;

};

#endif // CONFIGMODEL_H
