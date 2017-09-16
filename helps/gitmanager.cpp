#include "gitmanager.h"

#include <QObject>
#include <qDebug>

GitManager::GitManager()
{
    git_libgit2_init();
    init();
}

GitManager::GitManager(char *username, char *password)
{
    git_libgit2_init();
    init();
    setUserPass(username, password);
}


GitManager::~GitManager()
{
    git_libgit2_shutdown();
}


void GitManager::setUserPass(char *username, char *password)
{
    mUserPass.username = username;
    mUserPass.password = password;
}

void GitManager::init()
{
    mRepo = NULL;
    mOpts = {};
    mUserPass = {};

    git_checkout_options dummy_opts = GIT_CHECKOUT_OPTIONS_INIT;
    git_fetch_options dummy_fetch = GIT_FETCH_OPTIONS_INIT;

    memset(&(mOpts), 0, sizeof(git_clone_options));
    mOpts.version = GIT_CLONE_OPTIONS_VERSION;
    mOpts.checkout_opts = dummy_opts;
    mOpts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    mOpts.fetch_opts = dummy_fetch;
    mOpts.fetch_opts.callbacks.credentials = git_cred_userpass;
    mOpts.fetch_opts.callbacks.payload = &mUserPass;
}

int GitManager::initLocalRepo(const char *repoDirectory, bool initialCommit)
{
    int error = git_repository_init(&mRepo, repoDirectory, 0);

    if (initialCommit) {
        createInitialCommit();
    }

    return error;
}

int GitManager::clone(const char *url, const char *path)
{
    int error = git_clone(&mRepo, url, path, &mOpts);

    return error;
}

void GitManager::createInitialCommit()
{
    git_signature *sig;
    git_index *index;
    git_oid tree_id, commit_id;
    git_tree *tree;

    /** First use the config to initialize a commit signature for the user. */

    if (git_signature_default(&sig, mRepo) < 0) {
        qDebug() << "Unable to create a commit signature. Perhaps 'user.name' and 'user.email' are not set";
    }

    /* Now let's create an empty tree for this commit */

    if (git_repository_index(&index, mRepo) < 0) {
        qDebug() << "Could not open repository index";
    }

    /**
     * Outside of this example, you could call git_index_add_bypath()
     * here to put actual files into the index.  For our purposes, we'll
     * leave it empty for now.
     */

    if (git_index_write_tree(&tree_id, index) < 0) {
        qDebug() << "Unable to write initial tree from index";
    }

    git_index_free(index);

    if (git_tree_lookup(&tree, mRepo, &tree_id) < 0) {
        qDebug() << "Could not look up initial tree";
    }

    /**
     * Ready to create the initial commit.
     *
     * Normally creating a commit would involve looking up the current
     * HEAD commit and making that be the parent of the initial commit,
     * but here this is the first commit so there will be no parent.
     */

    if (git_commit_create_v(
            &commit_id, mRepo, "HEAD", sig, sig,
            NULL, "Initial commit", tree, 0) < 0) {
        qDebug() << "Could not create the initial commit";
    }

    /** Clean up so we don't leak memory. */

    git_tree_free(tree);
    git_signature_free(sig);
}
