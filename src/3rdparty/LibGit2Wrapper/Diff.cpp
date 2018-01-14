#include "Diff.h"

#include "SmartLibGitHeapPointerTemplate.hpp"
#include "GitEnvironment.h"

#include <stdexcept>
#include <cstring>

using Diff = Git::Diff;

Diff::Diff(const std::string& _repositoryKey) : RepositoryDependant(_repositoryKey), commitSha(""), newCommitSha("")
{
    GitEnvironment env;
    if(!env.isRepository(_repositoryKey))
    {
        throw std::invalid_argument("Provided argument is not a git repository.");
    }
}

Diff::Diff(const Commit& commit) : RepositoryDependant(commit.getRepositoryPath())
                                 , commitSha(commit.getSha())
                                 , newCommitSha("")
{
}

Diff::Diff( const Commit& oldCommit
          , const Commit& newCommit
          ) : RepositoryDependant( compareRepositories(oldCommit, newCommit)
                                  ? oldCommit.getRepositoryPath()
                                  : throw std::invalid_argument("Arguments are from different repositories")
                                  )
            , commitSha(oldCommit.getSha())
            , newCommitSha(newCommit.getSha())
{
}


std::vector<Diff::NameChange> Diff::getAllDifferentFiles()
{
    if(git_repository_head_unborn(smartLibGitRepository.get()))
    {
        throw std::logic_error("HEAD is unborn");
    }

    std::vector<Diff::NameChange> fileNameChanges;

    auto fileCallback = getGetFilesCallback();

    runDiffCallbacks(fileCallback, nullptr, nullptr, &fileNameChanges);

    return fileNameChanges;
}

std::vector<Diff::Hunk> Diff::getAllHunksForFile(const std::string& fileName)
{
    if(git_repository_head_unborn(smartLibGitRepository.get()))
    {
        throw std::logic_error("HEAD is unborn");
    }

    std::vector<Diff::Hunk> hunksForFile;

    auto fileCallback = [](const git_diff_delta *delta, float progress, void *payload) -> int
                        {
                            (void) delta, (void) progress, (void)payload;
                            return 0;
                        };

    auto hunkCallback = getHunkCallback();
    auto lineCallback = getLineCallback();

    std::vector<std::string> fileNameConstraints = {fileName};
    runDiffCallbacks(fileCallback, hunkCallback, lineCallback, &hunksForFile, fileNameConstraints);

    return hunksForFile;
}

/* private */ bool Diff::compareRepositories(const Commit& commit1, const Commit& commit2)
{

    if(commit1.getRepositoryPath() == commit2.getRepositoryPath())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*private*/ void Diff::runDiffCallbacks( FileCallbackType fileCallback
                                       , HunkCallbackType hunkCallback
                                       , LineCallbackType lineCallback
                                       , void* payload
                                       , std::vector<std::string> fileNameConstraints /* = {} */
                                       ) const
{

    uniquePtr<git_diff> smartLibGitDiff(nullptr, git_diff_free);

    if("" != commitSha && "" != newCommitSha)
    {

        auto oldLibGitCommit = getLibGitCommit(commitSha);
        auto newLibGitCommit = getLibGitCommit(newCommitSha);

        smartLibGitDiff = getLibGitDiff(oldLibGitCommit.get(), newLibGitCommit.get(), fileNameConstraints);
    }
    else if("" != commitSha)
    {
        smartLibGitDiff = getLibGitDiff(getLibGitCommit(commitSha).get(), fileNameConstraints);
    }
    else
    {
        smartLibGitDiff = getLibGitDiff(getHeadLibGitCommit().get(), fileNameConstraints);
    }

    checkLibGitError( git_diff_foreach( smartLibGitDiff.get()
                                      , fileCallback
                                      , nullptr
                                      , hunkCallback
                                      , lineCallback
                                      , payload
                                      )
                    );
}


//payload need to be a std::vector<Diff::Hunk>*
/* private */ Diff::LineCallbackType Diff::getLineCallback() const
{

    auto lineCallback = []( const git_diff_delta *delta
                          , const git_diff_hunk* hunk
                          , const git_diff_line* line
                          , void *payload
                          ) -> int
                        {
                            (void)delta, (void)hunk;

                            std::vector<Diff::Hunk>& hunks = *static_cast<std::vector<Diff::Hunk>*>(payload);
                            Diff::Hunk& currentHunk = hunks[hunks.size() - 1]; //hunk callback already run

                            Diff::ModificationType howChanged;
                            switch(line->origin)
                            {
                                case '+':
                                {
                                    howChanged = Diff::ModificationType::ADDED;
                                    break;
                                }
                                case '-':
                                {
                                    howChanged = Diff::ModificationType::DELETED;
                                    break;
                                }
                                default:
                                {
                                    howChanged = Diff::ModificationType::UNKNOWN;
                                }
                            }

                            Line currentLine = std::string(line->content, line->content_len);
                            LineNumberChange lineNumberChange(line->old_lineno, line->new_lineno);
                            currentHunk.push_back(std::make_tuple(howChanged, lineNumberChange, currentLine));

                            return 0;
                        };
    return lineCallback;
}

//payload need to be a std::vector<Diff::Hunk>*
/* private */ Diff::HunkCallbackType Diff::getHunkCallback() const
{

    auto hunkCallback = [](const git_diff_delta *delta, const git_diff_hunk* hunk, void *payload) -> int
                        {
                            (void)delta, (void)hunk;

                            std::vector<Diff::Hunk>& hunks = *static_cast<std::vector<Diff::Hunk>*>(payload);
                            hunks.push_back(Diff::Hunk());

                            return 0;
                        };
    return hunkCallback;
}

//payload need to be a std::vector<NameChange>*
/* private */ Diff::FileCallbackType Diff::getGetFilesCallback() const
{

    auto fileCallback = [](const git_diff_delta *delta, float progress, void *payload) -> int
                          {
                              (void)delta, (void)progress;

                              if( delta->status != GIT_DELTA_IGNORED
                                && delta->status != GIT_DELTA_UNMODIFIED
                                && delta->status != GIT_DELTA_UNTRACKED
                                && delta->status != GIT_DELTA_UNREADABLE
                                && delta->status != GIT_DELTA_CONFLICTED
                                )
                              {
                                  std::vector<NameChange>& fileNames = *static_cast<std::vector<NameChange>*>(payload);
                                  fileNames.push_back(NameChange(std::string(delta->old_file.path)
                                                                , std::string(delta->new_file.path)
                                                                )
                                                     );
                              }
                              return 0;
                          };
    return fileCallback;
}


/* private */ uniquePtr<git_diff> Diff::getLibGitDiff( const git_commit* toDiff
                                                     , const std::vector<std::string>& fileNames /* = {} */
                                                     ) const
{
    git_repository* repository = smartLibGitRepository.get();

    git_tree* treeRaw = nullptr;
    checkLibGitError( git_commit_tree(&treeRaw, toDiff) );
    uniquePtr<git_tree> smartCommitTree(treeRaw, git_tree_free);

    git_diff_options options;
    checkLibGitError( git_diff_init_options(&options, GIT_DIFF_OPTIONS_VERSION) );

    if(fileNames.size() > 0)
    {
        char** cStrings = new char*[fileNames.size()];
        for(std::size_t i = 0; i < fileNames.size(); ++i)
        {
            char* cString = new char[fileNames[i].size() + 1];
            strcpy(cString, fileNames[i].c_str());
            cStrings[i] = cString;
        }

        git_strarray pathArray = {cStrings, fileNames.size()};

        //we will free the heap because it was allocated with new (not malloc)
        //no "uniquePtr<git_strarray> pathArrayWrapper(&pathArray, git_strarray_free);" for you

        options.pathspec = pathArray;


        git_diff* diffRaw = nullptr;
        int error = git_diff_tree_to_workdir_with_index( &diffRaw
                                                       , repository
                                                       , smartCommitTree.get()
                                                       , &options
                                                       );

        for(std::size_t i = 0; i < fileNames.size(); ++i)
        {
            delete[] cStrings[i];
        }
        delete[] cStrings;

        checkLibGitError(error);

        return uniquePtr<git_diff> (diffRaw, git_diff_free);
    }
    else
    {
        git_diff* diffRaw = nullptr;
        checkLibGitError( git_diff_tree_to_workdir_with_index(&diffRaw, repository, smartCommitTree.get(), &options) );
        return uniquePtr<git_diff> (diffRaw, git_diff_free);
    }
}

/* private */ uniquePtr<git_diff> Diff::getLibGitDiff( const git_commit* oldCommit
                                                     , const git_commit* newCommit
                                                     , const std::vector<std::string>& fileNames /* = {} */
                                                     ) const
{
    git_repository* repository = smartLibGitRepository.get();

    git_tree* oldTreeRaw = nullptr;
    checkLibGitError( git_commit_tree(&oldTreeRaw, oldCommit) );
    uniquePtr<git_tree> smartOldTree(oldTreeRaw, git_tree_free);

    git_tree* newTreeRaw = nullptr;
    checkLibGitError( git_commit_tree(&newTreeRaw, newCommit) );
    uniquePtr<git_tree> smartNewTree(newTreeRaw, git_tree_free);

    git_diff_options options;
    checkLibGitError( git_diff_init_options(&options, GIT_DIFF_OPTIONS_VERSION) );

    if(fileNames.size() > 0)
    {
        char** cStrings = new char*[fileNames.size()];
        for(std::size_t i = 0; i < fileNames.size(); ++i)
        {
            char* cString = new char[fileNames[i].size() + 1];
            strcpy(cString, fileNames[i].c_str());
            cStrings[i] = cString;
        }

        git_strarray pathArray = {cStrings, fileNames.size()};

        //we will free the heap because it was allocated with new (not malloc)
        //no "uniquePtr<git_strarray> pathArrayWrapper(&pathArray, git_strarray_free);" for you

        options.pathspec = pathArray;


        git_diff* diffRaw = nullptr;
        int error = git_diff_tree_to_tree( &diffRaw
                                         , repository
                                         , smartOldTree.get()
                                         , smartNewTree.get()
                                         , &options
                                         );

        for(std::size_t i = 0; i < fileNames.size(); ++i)
        {
            delete[] cStrings[i];
        }
        delete[] cStrings;

        checkLibGitError(error);

        return uniquePtr<git_diff> (diffRaw, git_diff_free);
    }
    else
    {
        git_diff* diffRaw = nullptr;
        checkLibGitError( git_diff_tree_to_tree( &diffRaw
                                               , repository
                                               , smartOldTree.get()
                                               , smartNewTree.get()
                                               , &options
                                               )
                        );
        return uniquePtr<git_diff> (diffRaw, git_diff_free);
    }
}
