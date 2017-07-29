#ifndef GITMANAGER_H
#define GITMANAGER_H

#include "git2.h"

class GitManager
{
public:
    git_repository *repo;
    git_clone_options opts;

    GitManager();
    GitManager(char *username, char *password);
    ~GitManager();

    void setUserPass(char *username, char *password);

    int initLocalRepo(git_repository *repo, const char *repo_directory);
    int clone(const char *url, const char *path);

private:
    git_cred_userpass_payload userPass;

    void init();
};

#endif // GITMANAGER_H
