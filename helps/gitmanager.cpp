#include "gitmanager.h"

#include <qDebug>

GitManager::GitManager()
{
    git_libgit2_init();
    init();
}

GitManager::GitManager(const char *username, const char *password)
{
    git_libgit2_init();
    init();
    setUserPass(username, password);
}

GitManager::~GitManager()
{
    git_libgit2_shutdown();
}

void GitManager::setUserPass(const char *username, const char *password)
{
    mUserPass.username = username;
    mUserPass.password = password;
}

void GitManager::init()
{
    mRepo = NULL;
    mUserPass = {};
}

int GitManager::initLocalRepo(const char * repoDirectory, bool initialCommit)
{
    int error = git_repository_init(&mRepo, repoDirectory, 0);

    if (initialCommit) {
        createInitialCommit();
    }

    return error;
}

int GitManager::clone(const char * url, const char * path)
{
    git_clone_options opts;

    git_checkout_options dummy_opts = GIT_CHECKOUT_OPTIONS_INIT;
    git_fetch_options dummy_fetch = GIT_FETCH_OPTIONS_INIT;

    memset(&(opts), 0, sizeof(git_clone_options));
    opts.version = GIT_CLONE_OPTIONS_VERSION;
    opts.checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    opts.fetch_opts = GIT_FETCH_OPTIONS_INIT;
    opts.fetch_opts.callbacks.credentials = git_cred_userpass;
    opts.fetch_opts.callbacks.payload = &mUserPass;

    return git_clone(&mRepo, url, path, &opts);;
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

int GitManager::open(const char * repoDirectory)
{
    return git_repository_open(&mRepo, repoDirectory);
}

int GitManager::addRemote(const char * name, const char * url)
{
    git_remote *remote = {0};

    return git_remote_create(&remote, mRepo, name, url);
}

int GitManager::removeRemote(const char * name)
{
    return git_remote_delete(mRepo, name);
}

int GitManager::renameRemote(const char * name, const char * newName)
{
    git_strarray *problems = {0};

    return git_remote_rename(problems, mRepo, name, newName);;
}

int GitManager::setUrlRemote(const char * name, const char * url, bool push)
{
    return push ? git_remote_set_pushurl(mRepo, name, url)
                : git_remote_set_url(mRepo, name, url);
}

std::vector<git_remote *> GitManager::getRemoteList()
{
    const char *name, *fetch, *push;
    git_strarray remotes = {0};
    git_remote *remote = {0};

    git_remote_list(&remotes, mRepo);

    std::vector<git_remote *> list = {};
    for (int i = 0; i < (int) remotes.count; i++) {
        name = remotes.strings[i];
        git_remote_lookup(&remote, mRepo, name);
        list.push_back(remote);
//        QMap<QString, QString> map = {};
//        fetch = git_remote_url(remote);
//        if (fetch) {
//            map["type"] = "fetch";
//            map["name"] = name;
//            map["url"] = fetch;
//            list.append(map);
//        }
//        push = git_remote_pushurl(remote);
//        /* use fetch URL if no distinct push URL has been set */
//        push = push ? push : fetch;
//        if (push) {
//            map["type"] = "push";
//            map["name"] = name;
//            map["url"] = push;
//            list.append(map);
//        }

        git_remote_free(remote);
    }

    git_strarray_free(&remotes);

    return list;
}

std::vector<git_status_entry> GitManager::getStatusList()
{
    git_status_list *status = {0};
    size_t maxi = git_status_list_entrycount(status);

    std::vector<git_status_entry> list = {};
    for (size_t i = 0; i < maxi; ++i) {
        list.push_back(*git_status_byindex(status, i));
    }

    git_status_list_free(status);

    return list;
}

void GitManager::addAll()
{
    git_index *index;
    git_strarray array = {0};

    if (git_repository_index(&index, mRepo)) {
        qDebug() << "Could not open repository index";
    }

    struct print_payload payload = {2, mRepo};

    git_index_add_all(index, &array, 0, NULL, &payload);

    git_index_write(index);
    git_index_free(index);
}

bool GitManager::commit()
{
    std::vector<git_status_entry> statusList = getStatusList();
    if (statusList.size() == 0) {
        qDebug() << "Nothing to it...";
        return true;
    }
    for (auto &&item : statusList) {
        if (GIT_STATUS_INDEX_NEW <= item.status && item.status <= GIT_STATUS_INDEX_TYPECHANGE) {
            continue;
        }
        qDebug() << "Changes not staged for commit: ser \"git add . \" ";
        return false;
    }

    git_index *index;

    if (git_repository_index(&index, mRepo)) {
        qDebug() << "Could not open repository index";
    }

    //end random: list references

	//for a commit, we need:
	// -repo (obviously)
	// - name of ref (usually "HEAD")
	// - author commiter sig
	// - encoding+ message
	// - root tree (pointer
	// - parent count+parents (why? can have multiple parents)

	git_tree *tree;
	git_oid tree_id, parent_id, commit_id;

	git_index_write_tree(&tree_id, index); //we need to put index into a tree object for commit
	git_tree_lookup(&tree, mRepo, &tree_id);

	//tree_id = *git_object_id((git_object*)tree);
	git_commit *parent;

	//get HEAD and use it as parent of commit, put it in parent
	git_reference_name_to_id(&parent_id, mRepo, "HEAD");
	git_commit_lookup(&parent, mRepo, &parent_id);


	//do commit
	git_signature* sig;
	git_signature_now(&sig, "johnty", "info@johnty.ca");

	const char* msg = "Test Commit Message";

	git_commit_create_v(&commit_id, mRepo, "HEAD", sig, sig, NULL, msg, tree, 1, parent);

	git_index_free(index);
	git_signature_free(sig);
	git_tree_free(tree);
	git_commit_free(parent);

    return true;
}

void GitManager::push()
{
    std::vector<git_remote *> remoteList = getRemoteList();

    if (remoteList.size() == 0) {
        return;
    }

    git_strarray *refspecs = {0};
    git_push_options opts = GIT_PUSH_OPTIONS_INIT;

    memset(&(opts), 0, sizeof(git_clone_options));
    opts.version = GIT_CLONE_OPTIONS_VERSION;
    opts.callbacks.credentials = git_cred_userpass;
    opts.callbacks.payload = &mUserPass;

    git_remote_push(remoteList[0], refspecs, &opts);
}
