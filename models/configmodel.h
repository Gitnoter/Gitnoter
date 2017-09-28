#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include <QObject>

class ConfigModel : public QObject
{
Q_OBJECT

public:
    ConfigModel();

    ConfigModel(const QString &jsonString);

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

    QString getCategoriesName() const;

    void setCategoriesName(const QString &categoriesName);

    QString getTagsName() const;

    void setTagsName(const QString &tagsName);

    int getIsLocalRepo() const;

    void setIsLocalRepo(int isLocalRepo);

    int getIsSelectedClasses() const;

    void setIsSelectedClasses(int isSelectedClasses);

    const QString &getRepoEmail() const;

    void setRepoEmail(const QString &repoEmail);

private:
    QString mVersion;
    QString mRepoDir;
    QString mRepoUrl;
    QString mRepoEmail;
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