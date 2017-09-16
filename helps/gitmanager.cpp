#include "gitmanager.h"

#include <QObject>

GitManager::GitManager()
{
    git_libgit2_init();
    this->init();
}

GitManager::GitManager(char *username, char *password)
{
    git_libgit2_init();
    this->init();
    this->setUserPass(username, password);
}


GitManager::~GitManager()
{
    git_libgit2_shutdown();
}


void GitManager::setUserPass(char *username, char *password)
{
    this->userPass.username = username;
    this->userPass.password = password;
}

void GitManager::init()
{
    this->repo = NULL;
    this->opts = {};
    this->userPass = {};

    git_checkout_options dummy_opts = GIT_CHECKOUT_OPTIONS_INIT;
    git_fetch_options dummy_fetch = GIT_FETCH_OPTIONS_INIT;

    memset(&(this->opts), 0, sizeof(git_clone_options));
    this->opts.version = GIT_CLONE_OPTIONS_VERSION;
    this->opts.checkout_opts = dummy_opts;
    this->opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    this->opts.fetch_opts = dummy_fetch;
    this->opts.fetch_opts.callbacks.credentials = git_cred_userpass;
    this->opts.fetch_opts.callbacks.payload = &(this->userPass);
}

int GitManager::initLocalRepo(git_repository *repo, const char *repo_directory)
{
    int error = git_repository_init(&repo, repo_directory, false);

    return error;
}

int GitManager::clone(const char *url, const char *path)
{
    int error = git_clone(&(this->repo), url, path, &(this->opts));

    return error;
}
