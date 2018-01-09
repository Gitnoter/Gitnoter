#include "gitmanager.h"

#include <string.h>

GitManager::GitManager()
{
    init();
}

GitManager::GitManager(const char *username, const char *password, const char *nickname, const char *email)
{
    init();
    setUserPass(username, password);
    setSignature(nickname, email);
}

GitManager::~GitManager()
{
    git_repository_free(mRepo);
    git_libgit2_shutdown();
}

void GitManager::setUserPass(const char *username, const char *password)
{
    mUserPass.username = username;
    mUserPass.password = password;
}

void GitManager::setSignature(const char *username, const char *email)
{
    git_signature_now(&mSignature, username, email);
}

void GitManager::init()
{
    git_libgit2_init();

    mRepo = NULL;
    mUserPass = {};
    mSignature = {};
}

int GitManager::initLocalRepo(const char *repoDirectory, bool initialCommit)
{
    git_repository_free(mRepo);

    int error = 0;

    error = git_repository_init(&mRepo, repoDirectory, 0);
    if (error) {
        fprintf(stdout, "int GitManager::initLocalRepo: git_repository_init\n");
        return error;
    }

    if (initialCommit) {
        error = createInitialCommit();
        if (error) {
            fprintf(stdout, "int GitManager::initLocalRepo: createInitialCommit\n");
            return error;
        }
    }

    return 0;
}

int GitManager::clone(const char *url, const char *path)
{
    git_clone_options opts;

    memset(&(opts), 0, sizeof(git_clone_options));
    opts.version = GIT_CLONE_OPTIONS_VERSION;
    opts.checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    opts.fetch_opts = GIT_FETCH_OPTIONS_INIT;
    opts.fetch_opts.callbacks.credentials = git_cred_userpass;
    opts.fetch_opts.callbacks.payload = &mUserPass;

    return git_clone(&mRepo, url, path, &opts);
}

int GitManager::createInitialCommit()
{
    int error = 0;
    git_signature *sig;
    git_index *index;
    git_oid tree_id, commit_id;
    git_tree *tree;

    /** First use the config to initialize a commit signature for the user. */
    error = git_signature_default(&sig, mRepo);
    if (error) {
        fprintf(stdout, "Unable to create a commit signature. Perhaps 'user.name' and 'user.email' are not set\n");
        return 0;
    }

    /* Now let's create an empty tree for this commit */
    error = git_repository_index(&index, mRepo);
    if (error) {
        fprintf(stdout, "Could not open repository index\n");
        return error;
    }

    /**
     * Outside of this example, you could call git_index_add_bypath()
     * here to put actual files into the index.  For our purposes, we'll
     * leave it empty for now.
     */
    error = git_index_write_tree(&tree_id, index);
    if (error) {
        fprintf(stdout, "Unable to write initial tree from index\n");
    }

    git_index_free(index);
    error = git_tree_lookup(&tree, mRepo, &tree_id);
    if (error) {
        fprintf(stdout, "Could not look up initial tree\n");
        return error;
    }

    /**
     * Ready to create the initial commit.
     *
     * Normally creating a commit would involve looking up the current
     * HEAD commit and making that be the parent of the initial commit,
     * but here this is the first commit so there will be no parent.
     */
    error = git_commit_create_v(&commit_id, mRepo, "HEAD", sig, sig, NULL, "Initial commit", tree, 0);
    if (error) {
        fprintf(stdout, "Could not create the initial commit\n");
        return error;
    }

    /** Clean up so we don't leak memory. */

    git_tree_free(tree);
    git_signature_free(sig);

    return 0;
}

int GitManager::open(const char *repoDirectory)
{
    return git_repository_open(&mRepo, repoDirectory);
}

int GitManager::addRemote(const char *name, const char *url)
{
    git_remote *remote = {0};

    return git_remote_create(&remote, mRepo, name, url);
}

int GitManager::removeRemote(const char *name)
{
    return git_remote_delete(mRepo, name);
}

int GitManager::renameRemote(const char *name, const char *newName)
{
    git_strarray problems = {0};

    return git_remote_rename(&problems, mRepo, name, newName);
}

int GitManager::setUrlRemote(const char *name, const char *url, bool push)
{
    return push ? git_remote_set_pushurl(mRepo, name, url)
                : git_remote_set_url(mRepo, name, url);
}

std::vector<git_remote *> GitManager::getRemoteList()
{
    int error = 0;
    const char *name;
    git_strarray remotes = {0};
    git_remote *remote = {0};
    std::vector<git_remote *> list = {};

    error = git_remote_list(&remotes, mRepo);
    if (error) {
        fprintf(stdout, "std::vector<git_remote *> GitManager::getRemoteList(): git_remote_list %i\n", error);
        return list;
    }

    for (int i = 0; i < (int) remotes.count; i++) {
        name = remotes.strings[i];
        error = git_remote_lookup(&remote, mRepo, name);
        if (error) {
            fprintf(stdout, "std::vector<git_remote *> GitManager::getRemoteList(): git_remote_lookup %i\n", error);
            return list;
        }
        list.push_back(remote);
        git_remote_free(remote);
    }

    git_strarray_free(&remotes);

    return list;
}

void GitManager::clearRemoteList()
{
    int error = 0;
    git_strarray remotes = {0};

    error = git_remote_list(&remotes, mRepo);
    if (error) {
        fprintf(stdout, "std::vector<git_remote *> GitManager::getRemoteList(): git_remote_list %i\n", error);
        return;
    }

    for (int i = 0; i < (int) remotes.count; i++) {
        removeRemote(remotes.strings[i]);
    }
}

std::vector<git_status_entry> GitManager::getStatusList()
{
    int error = 0;
    git_status_list *status;
    git_status_options statusopt = GIT_STATUS_OPTIONS_INIT;
    std::vector<git_status_entry> list = {};

    error = git_status_list_new(&status, mRepo, &statusopt);
    if (error) {
        fprintf(stdout, "std::vector<git_status_entry> GitManager::getStatusList(): git_status_list_new\n");
        return list;
    }

    size_t maxi = git_status_list_entrycount(status);
    for (size_t i = 0; i < maxi; ++i) {
        list.push_back(*git_status_byindex(status, i));
    }

    git_status_list_free(status);

    return list;
}

int GitManager::addAll()
{
    git_index *index;
    int error = 0;

    error = git_repository_index(&index, mRepo);
    if (error) {
        fprintf(stdout, "int GitManager::addAll(): Could not open repository index\n");
        return error;
    }

    error = git_index_add_all(index, NULL, GIT_INDEX_ADD_DEFAULT, NULL, NULL);
    if (error) {
        fprintf(stdout, "int GitManager::addAll(): git_index_add_all\n");
        return error;
    }

    error = git_index_write(index);
    if (error) {
        fprintf(stdout, "int GitManager::addAll(): git_index_write\n");
        return error;
    }

    git_index_free(index);

    return 0;
}

int GitManager::commit(const char *message)
{
    std::vector<git_status_entry> statusList = getStatusList();
    if (statusList.size() == 0) {
        fprintf(stdout, "int GitManager::commit(const char *message): Nothing to it...\n");
        return 1000;
    }
    for (auto &&item : statusList) {
        if (GIT_STATUS_INDEX_NEW <= item.status && item.status <= GIT_STATUS_INDEX_TYPECHANGE) {
            continue;
        }
        fprintf(stdout, "int GitManager::commit(const char *message): "
                "Changes not staged for commit: ser \"git add . \" \n");
        return 1001;
    }

    git_index *index;
    int error = 0;
    error = git_repository_index(&index, mRepo);
    if (error) {
        fprintf(stdout, "int GitManager::commit(const char *message): git_index_write_tree: "
                "Could not open repository index\n");
        return error;
    }

    git_tree *tree;
    git_oid tree_id, parent_id, commit_id;

    error = git_index_write_tree(&tree_id, index); //we need to put index into a tree object for commit
    if (error) {
        fprintf(stdout, "int GitManager::commit(const char *message): git_index_write_tree\n");
        return error;
    }
    error = git_tree_lookup(&tree, mRepo, &tree_id);
    if (error) {
        fprintf(stdout, "int GitManager::commit(const char *message): git_tree_lookup\n");
        return error;
    }

    git_commit *parent;

    //get HEAD and use it as parent of commit, put it in parent
    error = git_reference_name_to_id(&parent_id, mRepo, "HEAD");
    if (error) {
        fprintf(stdout, "int GitManager::commit(const char *message): git_reference_name_to_id\n");
        return error;
    }
    error = git_commit_lookup(&parent, mRepo, &parent_id);
    if (error) {
        fprintf(stdout, "int GitManager::commit(const char *message): git_commit_lookup\n");
        return error;
    }

    error = git_commit_create_v(&commit_id, mRepo, "HEAD", mSignature, mSignature, NULL, message, tree, 1, parent);
    if (error) {
        fprintf(stdout, "int GitManager::commit(const char *message): git_commit_create_v\n");
        return error;
    }

    git_index_free(index);
    git_tree_free(tree);
    git_commit_free(parent);

    return 0;
}

int GitManager::push()
{
    git_remote *remote;
    int error = 0;

    error = git_remote_lookup(&remote, mRepo, "origin");
    if (error) {
        fprintf(stdout, "int GitManager::push(): git_remote_lookup\n");
        return error;
    }

    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    opts.callbacks.credentials = git_cred_userpass;
    opts.callbacks.payload = &mUserPass;

    const char *refs[] = {"refs/heads/master:refs/heads/master"};
    git_strarray refspecs = {(char **) refs, 1};

    return git_remote_push(remote, &refspecs, &opts);
}

int GitManager::fetch(bool prune)
{
    int error = 0;
    git_remote *remote;
    git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;

    // get a remote
    error = git_remote_lookup(&remote, mRepo, "origin");
    if (error) {
        fprintf(stdout, "int GitManager::fetch(bool prune): git_remote_lookup\n");
        return error;
    }

    // git fetch
    if (prune) {
        opts.prune = GIT_FETCH_PRUNE;
    }
    opts.callbacks.credentials = git_cred_userpass;
    opts.callbacks.payload = &mUserPass;

    return git_remote_fetch(remote, nullptr, &opts, nullptr);
}

int GitManager::merge()
{
    GitMerge *gitMerge = new GitMerge();

    return gitMerge->merge(mRepo);
}

int GitManager::resetHard()
{
    int error = 0;
    // todo: 莫名其妙, 不开辟这块内存, 就会崩溃, 分明没有用到这个变量
    git_oid bug;
    git_reference *ref;
    git_annotated_commit **their_head[1];
    git_checkout_options options = GIT_CHECKOUT_OPTIONS_INIT;
    git_object *object;

    error = git_reference_dwim(&ref, mRepo, "heads/master");
    if (error) {
        fprintf(stdout, "int GitManager::resetHard(): git_reference_dwim\n");
        return error;
    }
    error = git_annotated_commit_from_ref((git_annotated_commit **) &their_head[1], mRepo, ref);
    if (error) {
        fprintf(stdout, "int GitManager::resetHard(): git_reference_dwim\n");
        return error;
    }

    const git_oid *target_oid = git_annotated_commit_id((const git_annotated_commit *) their_head[1]);

    error = git_object_lookup(&object, mRepo, target_oid, GIT_OBJ_COMMIT);
    if (error) {
        fprintf(stdout, "int GitManager::resetHard(): git_object_lookup\n");
        return error;
    }
    error = git_reset(mRepo, object, GIT_RESET_HARD, &options);
    if (error) {
        fprintf(stdout, "int GitManager::resetHard(): git_reset\n");
        return error;
    }

    git_reference_free(ref);

    return 0;
}

int GitManager::pull()
{
    int error = 0;

    error = commitA();
    if (error) {
        fprintf(stdout, "int GitManager::pull(): commitA\n");
        return error;
    }

    error = fetch();
    if (error) {
        fprintf(stdout, "int GitManager::pull(): fetch\n");
        return error;
    }

    if (merge() == 1000) {
        error = commitU();
        if (error) {
            fprintf(stdout, "int GitManager::pull(): commitU\n");
            return error;
        }

        error = push();
        if (error) {
            fprintf(stdout, "int GitManager::pull(): push\n");
            return error;
        }
    }
    return 0;
}

int GitManager::commitA(const char *message)
{
    int error = 0;

    error = addAll();
    if (error) {
        fprintf(stdout, "int GitManager::commitA(const char *message): addAll\n");
        return error;
    }

    error = commit(message);
    if (!(error == 1000 || error == 0)) {
        fprintf(stdout, "int GitManager::commitA(const char *message): commit\n");
        return error;
    }

    return 0;
}

int GitManager::commitU(const char *message)
{
    int error = 0;
    git_reference* origin_master = nullptr;
    git_reference* local_master = nullptr;
    git_commit* their_commit = nullptr;
    git_commit* our_commit = nullptr;
    git_index* index = nullptr;
    git_oid new_tree_id;
    git_tree* new_tree = nullptr;
    git_oid commit_id;

    // merge
    error = git_branch_lookup(&origin_master, mRepo, "origin/master", GIT_BRANCH_REMOTE);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_branch_lookup\n");
        return error;
    }
    error = git_branch_lookup(&local_master, mRepo, "master", GIT_BRANCH_LOCAL);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_branch_lookup\n");
        return error;
    }

    // reslove conflicts
    error = git_repository_index(&index, mRepo);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_repository_index\n");
        return error;
    }

    error = git_commit_lookup(&their_commit, mRepo, git_reference_target(origin_master));
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_commit_lookup\n");
        return error;
    }
    error = git_commit_lookup(&our_commit, mRepo, git_reference_target(local_master));
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_commit_lookup\n");
        return error;
    }

    // add and commit
    error = git_index_update_all(index, nullptr, nullptr, nullptr);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_index_update_all\n");
        return error;
    }
    error = git_index_write(index);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_index_write\n");
        return error;
    }
    error = git_index_write_tree(&new_tree_id, index);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_index_write_tree\n");
        return error;
    }
    error = git_tree_lookup(&new_tree, mRepo, &new_tree_id);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_tree_lookup\n");
        return error;
    }

    error = git_commit_create_v(&commit_id, mRepo, git_reference_name(local_master), mSignature, mSignature,
                                "UTF-8", message, new_tree, 2, our_commit, their_commit);
    if (error) {
        fprintf(stdout, "int GitManager::commitU(const char *message): git_commit_create_v\n");
        return error;
    }

    return 0;
}

void GitManager::test()
{
    const char *purl = "https://git.oschina.net/make/libgit2-test2.git";
    const char *prepo_directory = "/Users/MakeHui/Desktop/test2";

    const char *username = "MakeHui";
    const char *email = "makehuir@gmail.com";
    const char *password = "oschinaMail003";

    setUserPass(email, password);
    setSignature(username, email);

//    clone(purl, prepo_directory);

//    addRemote("o1", "https://git.oschina.net/make/libgit2-test.git");
//    addRemote("o2", "https://git.oschina.net/make/libgit2-test.git");
//
//    removeRemote("o1");
//    renameRemote("o2", "o3");
//    setUrlRemote("o4", "https://git.oschina.net/make/libgit2-test.git");
//    setUrlRemote("o5", "https://git.oschina.net/make/libgit2-test.git", true);
//
//    std::vector<git_remote *> remoteList = getRemoteList();
//    printf("remoteList: %lu\n", remoteList.size());

//    open(prepo_directory);
//    printf("getStatusList().size: %lu\n", getStatusList().size());
//    printf("commitA: %i\n", commitA("test commit~"));
//    printf("push: %i\n", push());
//    printf("pull: %i\n", pull());
//    printf("resetHard: %i\n", resetHard());

//    initLocalRepo(prepo_directory, true);
//    addRemote("origin", purl);
//    pull();
//    printf("push: %i\n", push());
}
