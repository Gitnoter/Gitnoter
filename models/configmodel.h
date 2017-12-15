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

    void init();

    void setRepoDir(const QString &repoDir);

    void setRepoUrl(const QString &repoUrl);

    void setRepoUsername(const QString &repoUsername);

    void setRepoPassword(const QString &repoPassword);

    void setOpenMainWindowNoteUuid(const QString &openNoteUuid);

    const QString getVersion() const;

    const QString getRepoDir() const;

    const QString getRepoUrl() const;

    const QString getRepoUsername() const;

    const QString getRepoPassword() const;

    const QString openMainWindowNoteUuid() const;

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

    const QString getCutRectKeySequence() const;

    void setCutRectKeySequence(const QString &keySequence);

    const QString getLockWindowKeySequence() const;

    void setLockWindowKeySequence(const QString &keySequence);

    const QString getCutWindowKeySequence() const;

    void setCutWindowKeySequence(const QString &keySequence);

    const QString getCutFullKeySequence() const;
    void setCutFullKeySequence(const QString &keySequence);

    ThemeManager::ThemeFlag getTheme() const;

    void setTheme(const ThemeManager::ThemeFlag &theme);

    QList<int> getMainWindowSplitterSizes();

    void setMainWindowSplitterSizes(const QList<int> &splitterSizes);

    QRect getMainWindowRect();

    void setMainWindowRect(const QRect &rect);

    Gitnoter::SortBasis getNoteSortBasis() const;

    void setNoteSortBasis(Gitnoter::SortBasis noteSortBasis);

    Qt::SortOrder getNoteSortType() const;

    void setNoteSortType(Qt::SortOrder noteSortType);

    void setNoteSort(Gitnoter::SortBasis noteSortBasis, Qt::SortOrder noteSortType);

    void setOpenWindowListNoteUuid(QWidgetList widgetList);

    void appendOpenWindowListNoteUuid(const QString &noteUuid);

    void removeOpenWindowListNoteUuid(const QString &noteUuid);

    void clearOpenWindowListNoteUuid();

    QStringList openWindowListNoteUuid() const;

    QList<int> getEditorSplitterSizes();

    void setEditorSplitterSizes(const QList<int> &splitterSizes);

    bool getMainWindowFullScreen() const;

    void setMainWindowFullScreen(bool fullScreen);

    bool getSidebarWidget() const;
    void setSidebarWidget(bool b);

    bool getListBarWidget() const;
    void setListBarWidget(bool b);

    bool getToolbarWidget() const;
    void setToolbarWidget(bool b);

    bool getNavigationBarWidget() const;
    void setNavigationBarWidget(bool b);

    QList<int> getEditorBackgroundSplitterSizes();
    void setEditorBackgroundSplitterSizes(const QList<int> &sizes);

    QRect getEditorWindowRect();
    void setEditorWindowRect(const QRect &rect);

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
    QString mOpenMainWindowNoteUuid;
    QString mOpenWindowListNoteUuid;

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

    QString mLockWindowKeySequence;
    QString mCutRectKeySequence;
    QString mCutFullKeySequence;
    QString mCutWindowKeySequence;

    QString mTheme;
    QString mMainWindowSplitterSizes;
    QString mMainWindowRect;
    QString mMainWindowFullScreen;
    QString mEditorSplitterSizes;
    QString mEditorBackgroundSplitterSizes;

    QString mEditorWindowRect;

    QString mSidebarWidget;
    QString mListBarWidget;
    QString mToolbarWidget;
    QString mNavigationBarWidget;

};

#endif // CONFIGMODEL_H
