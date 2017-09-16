#ifndef GITMANAGER_H
#define GITMANAGER_H

#include <vector>
#include <git2.h>

struct print_payload {
    int options;
    git_repository *repo;
};

class GitManager
{
public:
    GitManager();
    GitManager(const char *username, const char *password);
    ~GitManager();

    void setUserPass(const char *username, const char *password);

    int initLocalRepo(const char * repoDirectory, bool initialCommit = false);
    int clone(const char * url, const char * path);

    int open(const char * repoDirectory);

    int addRemote(const char * name, const char * url);
    int removeRemote(const char * name);
    int renameRemote(const char * name, const char * newName);
    int setUrlRemote(const char * name, const char * url, bool push);
    std::vector<git_remote *> getRemoteList();

    std::vector<git_status_entry> getStatusList();

    void addAll();
    bool commit();
    void push();

private:
    git_repository *mRepo;
    git_cred_userpass_payload mUserPass;

    void createInitialCommit();
    void init();
};

#endif // GITMANAGER_H
