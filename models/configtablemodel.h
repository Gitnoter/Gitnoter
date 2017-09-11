#ifndef CONFIGTABLEMODEL_H
#define CONFIGTABLEMODEL_H

#include <QObject>

class ConfigTableModel : public QObject
{
public:
    explicit ConfigTableModel(QString version = "", QString repoDir = "", QString repoUrl = "", QString repoUsername = "",
                              QString repoPassword = "", QString openNotesUuid = "");

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

    int getCategoriesId() const;
    void setCategoriesId(int categoriesId);

    int getTagsId() const;
    void setTagsId(int tagsId);

private:
    QString version;
    QString repoDir;
    QString repoUrl;
    QString repoUsername;
    QString repoPassword;
    QString openNotesUuid;
    int sidebarSortKey;
    QString sidebarSortValue;
    int categoriesId;
    int tagsId;

};

#endif // CONFIGTABLEMODEL_H