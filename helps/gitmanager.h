#ifndef GITMANAGER_H
#define GITMANAGER_H

#include <git2.h>

class GitManager
{
public:
    git_repository *mRepo;
    git_clone_options mOpts;

    GitManager();
    GitManager(char *username, char *password);
    ~GitManager();

    void setUserPass(char *username, char *password);

    int initLocalRepo(const char *repoDirectory, bool initialCommit = false);
    int clone(const char *url, const char *path);

    int addRemote();


private:
    git_cred_userpass_payload mUserPass;
    void createInitialCommit();

    void init();
};

#endif // GITMANAGER_H
