#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include <QObject>

class ConfigModel : public QObject
{
Q_OBJECT

public:
    ConfigModel(const QString &jsonString);

    ConfigModel(QString version = "", QString repoDir = "", QString repoUrl = "",
                QString repoUsername = "", QString repoPassword = "", QString openNotesUuid = "");

    QString serialize();

    void unserialize(const QString &jsonString);

    void setVersion(const QString &version);

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

    QString getCategoriesName() const;

    void setCategoriesName(const QString &categoriesName);

    QString getTagsName() const;

    void setTagsName(const QString &tagsName);

    int getIsLocalRepo() const;

    void setIsLocalRepo(int isLocalRepo);

    int getIsSelectedClasses() const;

    void setIsSelectedClasses(int isSelectedClasses);

private:
    QString mVersion;
    QString mRepoDir;
    QString mRepoUrl;
    QString mRepoUsername;
    QString mRepoPassword;
    QString mOpenNotesUuid;
    QString mSidebarSortValue;
    int mSidebarSortKey;
    int mIsSelectedClasses;
    int mIsLocalRepo;
    QString mCategoriesName;
    QString mTagsName;
};

#endif // CONFIGMODEL_H