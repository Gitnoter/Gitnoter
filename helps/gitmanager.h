#ifndef GITMANAGER_H
#define GITMANAGER_H

#include <QObject>
#include <QMap>
#include <QList>

#include <git2.h>

struct print_payload {
    int options;
    git_repository *repo;
};


class GitManager : public QObject
{
public:
    GitManager();
    GitManager(QString username, QString password);
    ~GitManager();

    void setUserPass(const char *username, const char *password);

    int initLocalRepo(const char * repoDirectory, bool initialCommit = false);
    int clone(const char * url, const char * path);

    int open(const char * repoDirectory);

    int addRemote(const char * name, const char * url);
    int removeRemote(const char * name);
    int renameRemote(const char * name, const char * newName);
    int setUrlRemote(const char * name, const char * url, bool push);
    QList<git_remote *> getRemoteList();

    QList<git_status_entry> getStatusList();

    void add();
    bool commit();
    void push();

private:
    git_repository *mRepo;
    git_cred_userpass_payload mUserPass;

    void createInitialCommit();

    void init();
};

#endif // GITMANAGER_H
