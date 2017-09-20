#ifndef GITMANAGER_H
#define GITMANAGER_H

#include <vector>
#include <git2.h>

#include <assert.h>

typedef struct
{
    const char **heads;
    size_t heads_count;

    git_annotated_commit **annotated;
    size_t annotated_count;

    int no_commit : 1;
    int did_merge : 1;
} merge_options;

class GitMerge
{
public:

    void merge_options_init(merge_options *opts)
    {
        opts->heads = NULL;
        opts->heads_count = 0;
        opts->annotated = NULL;
        opts->annotated_count = 0;
    }

    int resolve_refish(git_annotated_commit **commit, git_repository *repo, const char *refish)
    {
        git_reference *ref;
        int err = 0;
        git_oid oid;

        assert(commit != NULL);

        err = git_reference_dwim(&ref, repo, refish);
        if (err == GIT_OK) {
            git_annotated_commit_from_ref(commit, repo, ref);
            git_reference_free(ref);
            return 0;
        }

        err = git_oid_fromstr(&oid, refish);
        if (err == GIT_OK) {
            err = git_annotated_commit_lookup(commit, repo, &oid);
        }

        return err;
    }

    int resolve_heads(git_repository *repo, merge_options *opts)
    {
        git_annotated_commit **annotated = static_cast<git_annotated_commit **>(
                calloc(opts->heads_count, sizeof(git_annotated_commit *)));
        size_t annotated_count = 0, i;
        int err = 0;

        for (i = 0; i < opts->heads_count; i++) {
            err = resolve_refish(&annotated[annotated_count++], repo, opts->heads[i]);
            if (err != 0) {
                fprintf(stderr, "failed to resolve refish %s (%d)\n", opts->heads[i], err);
                annotated_count--;
                continue;
            }
        }

        if (annotated_count <= 0) {
            fprintf(stderr, "unable to parse any refish\n");
            free(annotated);
            return -1;
        }

        opts->annotated = annotated;
        opts->annotated_count = annotated_count;
        return 0;
    }

    int perform_fastforward(git_repository *repo, const git_oid *target_oid, int is_unborn)
    {
        git_checkout_options ff_checkout_options = GIT_CHECKOUT_OPTIONS_INIT;
        git_reference *head_ref;
        git_reference *new_head_ref;
        int err = 0;

        printf("is_unborn: %i\n", is_unborn);

        if (is_unborn) {
            const char *symbolic_ref;
            git_reference *master_ref;

            /* master reference is unborn, lookup HEAD manually, */
            err = git_reference_lookup(&head_ref, repo, "HEAD");
            if (err != 0) {
                fprintf(stderr, "failed to lookup HEAD ref\n");
                return -1;
            }

            /* grab its target ref, */
            symbolic_ref = git_reference_symbolic_target(head_ref);

            /* force-create it on the targets' OID, */
            err = git_reference_create(&master_ref, repo, symbolic_ref, target_oid, 1, NULL);
            if (err != 0) {
                fprintf(stderr, "failed to create master reference\n");
                return -1;
            }
        }
        else {
            /* The repo HEAD is pointing to an existing reference. */
            err = git_repository_head(&head_ref, repo);
            if (err != 0) {
                fprintf(stderr, "failed to get HEAD reference\n");
                return -1;
            }

            /* Move the HEAD reference to the new OID... */
            err = git_reference_set_target(&new_head_ref, head_ref, target_oid, NULL);
            if (err != 0) {
                fprintf(stderr, "failed to move HEAD reference\n");
                return -1;
            }
        }

        /* force-checkout the result. */
        err = git_checkout_head(repo, &ff_checkout_options);
        if (err != 0) {
            fprintf(stderr, "failed to checkout HEAD reference\n");
            return -1;
        }

        git_object *object;
        err = git_object_lookup(&object, repo, target_oid, GIT_OBJ_COMMIT);
        printf("git_object_lookup: %i\n", err);
        err = git_reset(repo, object, GIT_RESET_HARD, &ff_checkout_options);
        printf("git_reset_default: %i\n", err);

        return 0;
    }

    int analyze_merge(git_repository *repo, merge_options *opts)
    {
        git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
        git_merge_analysis_t analysis;
        git_merge_preference_t preference;
        int err = 0;

        merge_opts.flags = static_cast<git_merge_flag_t>(0);    // GIT_MERGE_FIND_RENAMES
                                                                // 有一个很神奇的地方, 必须要设置成0当有冲突时冲突文件才不会被删除
        merge_opts.file_flags = GIT_MERGE_FILE_STYLE_DIFF3;

        checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_ALLOW_CONFLICTS;

        err = git_merge_analysis(&analysis, &preference, repo,
                                 (const git_annotated_commit **) opts->annotated,
                                 opts->annotated_count);
        if (err) {
            fprintf(stderr, "merge analysis failed\n");
            return err;
        }

        if (analysis & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
            fprintf(stdout, "Already up-to-date\n");
            return 0;
        }

        if (analysis & GIT_MERGE_ANALYSIS_UNBORN
                 || (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD
                     && !(preference & GIT_MERGE_PREFERENCE_NO_FASTFORWARD))) {
            if (analysis & GIT_MERGE_ANALYSIS_UNBORN) {
                fprintf(stdout, "Unborn\n");
            }
            else {
                fprintf(stdout, "Fast-forward\n");
            }

            /* Since this is a fast-forward, there can be only one merge head */
            const git_oid *target_oid = git_annotated_commit_id(opts->annotated[0]);

            return perform_fastforward(repo, target_oid, (analysis & GIT_MERGE_ANALYSIS_UNBORN));
        }

        if (analysis & GIT_MERGE_ANALYSIS_NORMAL) {
            if (preference & GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY) {
                fprintf(stdout, "Fast-forward is preferred, but only a merge is possible\n");
                return 1;
            }

            err = git_merge(repo, (const git_annotated_commit **) opts->annotated,
                            opts->annotated_count, &merge_opts, &checkout_opts);
            if (err) {
                fprintf(stdout, "git_merge\n");
                return err;
            }

            /* Inform that a merge was done */
            opts->did_merge = 1;

            return 0;
        }

        return 1;
    }

    int merge(git_repository *repo)
    {
        merge_options opts;
        git_index *index;
        int err = 0;

        merge_options_init(&opts);

        // opts_add_refish(&opts, "origin/master");
        size_t sz = ++opts.heads_count * sizeof(opts.heads[0]);
        opts.heads = static_cast<const char **>(realloc(opts.heads, sz));
        opts.heads[opts.heads_count - 1] = "origin/master";

        err = resolve_heads(repo, &opts);
        if (err) {
            fprintf(stderr, "unable to parse any refish\n");
            return err;
        }

        err = analyze_merge(repo, &opts);
        if (err) {
            fprintf(stderr, "merge failed\n");
            return err;
        }

        if (!opts.did_merge) {
            fprintf(stdout, "Was either up-to-date, unborn, or a fast-forward, nothing left to do\n");
            return 0;
        }

        err = git_repository_index(&index, repo);
        if (err) {
            fprintf(stdout, "failed to get repository index\n");
            return err;
        }

        if (git_index_has_conflicts(index)) {
            /* Handle conflicts */
            git_index_conflict_iterator *conflicts;
            const git_index_entry *ancestor;
            const git_index_entry *our;
            const git_index_entry *their;

            err = git_index_conflict_iterator_new(&conflicts, index);
            if (err) {
                fprintf(stdout, "failed to create conflict iterator\n");
                return err;
            }

            while ((err = git_index_conflict_next(&ancestor, &our, &their, conflicts)) == 0) {
                fprintf(stderr, "conflict: a:%s o:%s t:%s\n", ancestor->path, our->path, their->path);
            }

            if (err != GIT_ITEROVER) {
                fprintf(stderr, "error iterating conflicts\n");
            }

            // git checkout --theirs <file>
            git_checkout_options opt = GIT_CHECKOUT_OPTIONS_INIT;
            opt.checkout_strategy |= GIT_CHECKOUT_USE_THEIRS;
            git_checkout_index(repo, index, &opt);

            git_index_conflict_iterator_free(conflicts);

            fprintf(stderr, "git_index_has_conflicts\n");
            return 1000;
        }

        return 0;
    }
};


class GitManager
{
public:
    GitManager();

    GitManager(const char *username, const char *password, const char *nickname, const char *email);

    ~GitManager();

    void setUserPass(const char *username, const char *password);

    void setSignature(const char *username, const char *email);

    int initLocalRepo(const char *repoDirectory, bool initialCommit = false);

    int clone(const char *url, const char *path);

    int open(const char *repoDirectory);

    int addRemote(const char *name, const char *url);

    int removeRemote(const char *name);

    int renameRemote(const char *name, const char *newName);

    int setUrlRemote(const char *name, const char *url, bool push = false);

    std::vector<git_remote *> getRemoteList();

    std::vector<git_status_entry> getStatusList();

    int addAll();

    int commit(const char *message = "Hello Gitnoter");

    int commitA(const char *message = "Merge remote-tracking branch 'refs/remotes/origin/master'");

    int commitU(const char *message = "Merge remote-tracking branch 'refs/remotes/origin/master'");

    int push();

    int fetch(bool prune = true);

    int merge();

    int pull();

    int resetHard();

    void test();

private:
    git_repository *mRepo;
    git_cred_userpass_payload mUserPass;
    git_signature *mSignature;

    int createInitialCommit();

    void init();

};

#endif // GITMANAGER_H
