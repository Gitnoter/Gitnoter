#include "Repository.h"

#include "GitEnvironment.h"
#include "SmartLibGitHeapPointerTemplate.hpp"
#include "LibGitException.h"

#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using Repository = Git::Repository;
using Commit = Git::Commit;
using Branch = Git::Branch;
using Tag = Git::Tag;
using Remote = Git::Remote;
using LibGitException = Git::LibGitException;


Repository::Repository(const std::string& _pathToRepo) : RepositoryDependant(_pathToRepo)
{
    if(0 != getRemoteCount() && 1 != getRemoteCount())
    {
        throw std::domain_error("Currently only repositories with one or no remotes are supported.");
    }

    if(git_repository_is_bare(smartLibGitRepository.get()))
    {
        throw std::domain_error("Currently bare repositories are not supported.");
    }
}

bool Repository::hasConflicts() const
{
    git_index* indexRaw = nullptr;
    checkLibGitError( git_repository_index(&indexRaw, smartLibGitRepository.get()) );
    uniquePtr<git_index> index(indexRaw, git_index_free);
    
    std::size_t conflicts = git_index_has_conflicts(index.get());

    return (1 == conflicts);
}

void Repository::stash(const std::string& message /* = "" */)
{
    git_oid oid;
    uint32_t flags = GIT_STASH_DEFAULT;
    
    git_signature* stasherRaw = nullptr;
    checkLibGitError( git_signature_default(&stasherRaw, smartLibGitRepository.get()) );
    uniquePtr<git_signature> stasher(stasherRaw, git_signature_free);

    checkLibGitError( git_stash_save(&oid, smartLibGitRepository.get(), stasherRaw, message.c_str(), flags) );
}

void Repository::commit(const std::string& message)
{

    bool inMergeState = isInMergeState();
    bool inNormalState = isInNormalState();
    if(!inMergeState && !inNormalState)
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(getStagedFiles().size() < 1)
    {
        throw std::logic_error("There is nothing to commit");
    }

    if(hasConflicts())
    {
        std::logic_error("Cannot commit to conflicted repository.");
    }

    git_oid treeOid, commitOid;

    git_index* indexRaw = nullptr;
    checkLibGitError( git_repository_index(&indexRaw, smartLibGitRepository.get()) );
    uniquePtr<git_index> index(indexRaw, git_index_free);

    checkLibGitError( git_index_write_tree(&treeOid, index.get()) );

    git_signature* signatureRaw = nullptr;
    checkLibGitError( git_signature_default(&signatureRaw, smartLibGitRepository.get()) );
    uniquePtr<git_signature> signature(signatureRaw, git_signature_free);

    git_tree* treeRaw = nullptr;
    checkLibGitError( git_tree_lookup(&treeRaw, smartLibGitRepository.get(), &treeOid) );
    uniquePtr<git_tree> tree(treeRaw, git_tree_free);

    if(inNormalState)
    {
        bool isInitialCommit = git_repository_head_unborn(smartLibGitRepository.get());

        if(isInitialCommit)
        {
            checkLibGitError( git_commit_create_v( &commitOid
                                                 , smartLibGitRepository.get()
                                                 , "HEAD"
                                                 , signature.get()
                                                 , signature.get()
                                                 , "UTF-8"
                                                 , messagePrettify(message).c_str()
                                                 , tree.get()
                                                 , 0) );
        }
        else
        {
            uniquePtr<git_commit> parent = getHeadLibGitCommit();
            const git_commit *parents[] = { parent.get() };

            checkLibGitError( git_commit_create( &commitOid
                                               , smartLibGitRepository.get()
                                               , "HEAD"
                                               , signature.get()
                                               , signature.get()
                                               , "UTF-8"
                                               , messagePrettify(message).c_str()
                                               , tree.get()
                                               , 1
                                               , parents) );
        }
    }
    else if(inMergeState)
    {
        std::vector<git_oid> oids;
        oids.push_back(*git_commit_id(getHeadLibGitCommit().get())); //head commit have to come first

        auto callback = [](const git_oid *oid, void *payload) -> int
                        {
                            static_cast<std::vector<git_oid>*>(payload)->push_back(*oid);
                            return 0;
                        };

        checkLibGitError( git_repository_mergehead_foreach(smartLibGitRepository.get(), callback, &oids) );

        if(2 != oids.size())
        {
            throw std::domain_error("Currently only merges with two parents are supported.");
        }

        const git_commit** parents = new const git_commit*[oids.size()];

        std::vector<uniquePtr<git_commit>> parentLibGitCommits;
        for(std::size_t i = 0; i < oids.size(); ++i)
        {
            git_commit* commitRaw;
            try
            {
                checkLibGitError(git_commit_lookup(&commitRaw, smartLibGitRepository.get(), &oids[i]) );
            }
            catch(LibGitException& e)
            {
                delete[] parents;
                throw e;
            }

            uniquePtr<git_commit> commit(commitRaw, git_commit_free);

            parentLibGitCommits.push_back(std::move(commit));

            parents[i] = parentLibGitCommits[i].get();
        }

        checkLibGitError( git_commit_create( &commitOid,
                                             smartLibGitRepository.get(),
                                             "HEAD",
                                             signature.get(),
                                             signature.get(),
                                             "UTF-8",
                                             messagePrettify(message).c_str(),
                                             tree.get(),
                                             oids.size(),
                                             parents) );

        delete[] parents;

        checkLibGitError( git_repository_state_cleanup(smartLibGitRepository.get()) );
    }

}

void Repository::stage(const std::vector<std::string>& relativePaths)
{
    git_index* indexRaw = nullptr;
    checkLibGitError( git_repository_index(&indexRaw, smartLibGitRepository.get()) );
    uniquePtr<git_index> index(indexRaw, git_index_free);
    
    for(const auto& relativePath : relativePaths)
    {
        if(!isFileInRepository(relativePath))
        {
            throw std::invalid_argument("File is not in repository: " + relativePath);
        }
        if(doesGitIgnoreApply(relativePath))
        {
            throw std::invalid_argument("Gitignore rule applies for file: " + relativePath);
        }
    }

    for(const auto& relativePath : relativePaths)
    {
        unsigned int status;
        checkLibGitError( git_status_file(&status, smartLibGitRepository.get(), relativePath.c_str()) );

        if(GIT_STATUS_WT_DELETED & status)
        {
            checkLibGitError( git_index_remove_bypath(index.get(), relativePath.c_str()) );
        }
        else
        {
            checkLibGitError( git_index_add_bypath(index.get(), relativePath.c_str()) );
        }
    }
    
    checkLibGitError( git_index_write(index.get()) );
}

void Repository::unstage(const std::vector<std::string>& relativePaths)
{
    for(const auto& relativePath : relativePaths)
    {
        if(!isFileInRepository(relativePath))
        {
            throw std::invalid_argument("File is not in repository: " + relativePath);
        }
    }

    for(const auto& file : relativePaths)
    {
        if (isStaged(file))
        {
            // get head as libgit object            
            git_oid headOid;
            
            checkLibGitError( git_reference_name_to_id( &headOid, smartLibGitRepository.get(), "HEAD" ) );
            
            git_object* targetObjectRaw = nullptr;
            checkLibGitError( git_object_lookup(&targetObjectRaw
                                               , smartLibGitRepository.get()
                                               , &headOid
                                               , GIT_OBJ_COMMIT
                                               )
                            );
            uniquePtr<git_object> targetObject(targetObjectRaw, git_object_free);
            

            char** cStrings = new char*[relativePaths.size()];
            for(std::size_t i = 0; i < relativePaths.size(); ++i)
            {
                char* cString = new char[relativePaths[i].size() + 1];
                strcpy(cString, relativePaths[i].c_str());
                cStrings[i] = cString;
            }

            git_strarray pathArray = {cStrings, relativePaths.size()};

            //we will free the heap because it was allocated with new (not malloc)
            //no "uniquePtr<git_strarray> pathArrayWrapper(&pathArray, git_strarray_free);" for you
            
            int error = ( git_reset_default(smartLibGitRepository.get()
                                               , targetObject.get()
                                               , &pathArray
                                               )
                            );

            for(std::size_t i = 0; i < relativePaths.size(); ++i)
            {
                delete[] cStrings[i];
            }
            delete[] cStrings;

            checkLibGitError(error);
        }
    }

}

void Repository::hardReset()
{
    checkLibGitError(git_repository_state_cleanup(smartLibGitRepository.get()));

    git_checkout_options options;
    checkLibGitError(  git_checkout_init_options(&options, GIT_CHECKOUT_OPTIONS_VERSION) );


    git_oid headOid = shaToLibGitOid(getHead()->getSha());

    git_object* targetObjectRaw = nullptr;
    checkLibGitError( git_object_lookup(&targetObjectRaw
                                       , smartLibGitRepository.get()
                                       , &headOid
                                       , GIT_OBJ_COMMIT
                                       )
                    );
    uniquePtr<git_object> targetObject(targetObjectRaw, git_object_free);

    checkLibGitError( git_reset(smartLibGitRepository.get(), targetObject.get(), GIT_RESET_HARD, &options) );

}

void Repository::abortRebase()
{
    if(isInRebaseState())
    {
        git_rebase* currentRebaseRaw = nullptr;
        checkLibGitError( git_rebase_open(&currentRebaseRaw, smartLibGitRepository.get(), nullptr) );
        uniquePtr<git_rebase> currentRebase(currentRebaseRaw, git_rebase_free);
        
        checkLibGitError(git_rebase_abort(currentRebase.get()));
    }
    else
    {
        throw std::logic_error("Repository state prevents operation.");
    }
}

void Repository::tag(const std::string& name)
{
    git_commit* head = getHeadLibGitCommit().get();
    std::string sha = getLibGitCommitSha(head);
    
    tag(*getHead(), name);
}

void Repository::tag(const Commit& toTag, const std::string& name)
{
    if(!isOwnedByRepo(toTag))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    std::string targetCommitSha = toTag.getSha();

    git_oid oid = shaToLibGitOid(targetCommitSha);
    
    git_object* parentCommitRaw = nullptr;
    checkLibGitError( git_object_lookup_prefix( &parentCommitRaw
                                              , smartLibGitRepository.get()
                                              , &oid
                                              , targetCommitSha.size()
                                              , GIT_OBJ_COMMIT)
                    );
    uniquePtr<git_object> parentCommit(parentCommitRaw, git_object_free);
    
    checkLibGitError( git_tag_create_lightweight( &oid                         /* target ID */
                                                , smartLibGitRepository.get()  /* repository */
                                                , name.c_str()                 /* name */
                                                , parentCommit.get()           /* target */
                                                , false                        /* force? */
                                                )                    
                    );
}

void Repository::branch(const std::string& name)
{
    branch(*getHead(), name);
}


void Repository::branch(const Commit& toStartBranchFrom, const std::string& name)
{
    if(!isOwnedByRepo(toStartBranchFrom))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    uniquePtr<git_commit> libGitCommit = getLibGitCommit(toStartBranchFrom.getSha());

    /* create branch */
    git_reference* branchRaw = nullptr;
    checkLibGitError( git_branch_create( &branchRaw
                                       , smartLibGitRepository.get()
                                       , name.c_str()
                                       , libGitCommit.get()
                                       , 0
                                       )
                    );
    uniquePtr<git_reference> branch(branchRaw, git_reference_free);

    // Make HEAD point to this branch - would imitate `git checkout -b`
    //git_reference* headRaw = nullptr;
    //checkLibGitError( git_reference_symbolic_create( &headRaw
    //                                               , smartLibGitRepository.get()
    //                                               , "HEAD"
    //                                               , git_reference_name( branch.get() )
    //                                               , 0
    //                                               , ""
    //                                               )
    //                );
    //uniquePtr<git_reference> head(headRaw, git_reference_free);
}

//after this command you need to explicitly delete it from all remotes (see deleteTag)
void Repository::deleteBranch(const Branch& toDelete)
{
    if(!isOwnedByRepo(toDelete))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    if(!toDelete.isLocal())
    {
        throw std::invalid_argument("Cannot delete remote tracking branches.");
    }

    std::string fullReferenceName = "refs/heads/" + toDelete.getName();
    
    git_reference* branchRaw = nullptr;
    checkLibGitError( git_reference_lookup(&branchRaw, smartLibGitRepository.get(), fullReferenceName.c_str()) );
    uniquePtr<git_reference> branch(branchRaw, git_reference_free);
    
    checkLibGitError( git_branch_delete(branch.get()) );
}

//after this command you need to explicitly delete it from all remotes as explained in:
//http://stackoverflow.com/questions/1028649/how-do-you-rename-a-git-tag
void Repository::deleteTag(const Tag& toDelete)
{
    if(!isOwnedByRepo(toDelete))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    checkLibGitError( git_tag_delete(smartLibGitRepository.get(), toDelete.getName().c_str()) );
}

std::vector<std::unique_ptr<Branch>> Repository::getBranches() const
{
    std::vector<std::unique_ptr<Branch>> branches;
    
    //GIT_BRANCH_LOCAL GIT_BRANCH_REMOTE GIT_BRANCH_ALL
    git_branch_t branchType = GIT_BRANCH_ALL;    
    
    git_branch_iterator* branchIteratorRaw = nullptr;
    checkLibGitError( git_branch_iterator_new(&branchIteratorRaw, smartLibGitRepository.get(), branchType) );
    uniquePtr<git_branch_iterator> branchIterator(branchIteratorRaw, git_branch_iterator_free);
    
    git_reference* branchRaw = nullptr;
    int error = git_branch_next(&branchRaw, &branchType, branchIterator.get());
    uniquePtr<git_reference> branch(branchRaw, git_reference_free);
    
    while( GIT_ITEROVER != error )
    {
        checkLibGitError(error);
        
        const char* branchNameAsCharArray;
        
        checkLibGitError( git_branch_name(&branchNameAsCharArray, branch.get()) );
                
        std::string branchName = std::string(branchNameAsCharArray);
        branches.push_back( std::unique_ptr<Branch>( new Branch( pathToRepository
                                                               , branchName
                                                               , !git_reference_is_remote(branch.get())
                                                               )
                                                    )
                           );

        branchRaw = nullptr;
        error = git_branch_next(&branchRaw, &branchType, branchIterator.get());
        branch.reset(branchRaw); //calls deleter
    }
    
    return branches;
}

std::vector<std::unique_ptr<Tag>> Repository::getTags() const
{
    std::vector<std::unique_ptr<Tag>> tags;
    
    git_reference_iterator* referenceIteratorRaw = nullptr;
    checkLibGitError( git_reference_iterator_new(&referenceIteratorRaw, smartLibGitRepository.get()) );
    uniquePtr<git_reference_iterator> referenceIterator(referenceIteratorRaw, git_reference_iterator_free);
    
    git_reference* referenceRaw = nullptr;
    int error = git_reference_next(&referenceRaw, referenceIterator.get());
    uniquePtr<git_reference> reference(referenceRaw, git_reference_free);
    
    while( GIT_ITEROVER != error )
    {
        checkLibGitError(error);
        
        if(git_reference_is_tag(reference.get()))
        {            
            std::string tagName = std::string(git_reference_name(reference.get()));
            std::string normalizedTagName = tagName.substr(tagName.rfind('/') + 1, tagName.size());
            
            tags.push_back(std::unique_ptr<Tag>(new Tag(pathToRepository, normalizedTagName)));
        }
        
        referenceRaw = nullptr;
        error = git_reference_next(&referenceRaw, referenceIterator.get());
        reference.reset(referenceRaw); //calls deleter
    }
        
    return tags;
}

std::vector<std::string> Repository::getStagedFiles() const
{
    std::vector<std::string> stagedFiles;
    
    auto callback = [](const char *path, unsigned int status_flags, void *payload)
    {
        if(status_flags & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED |
                       GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE))
            static_cast<std::vector<std::string>*>(payload)->push_back(path);
        return 0;
    };
    
    checkLibGitError( git_status_foreach(smartLibGitRepository.get(), callback, &stagedFiles) );
    
    return stagedFiles;
}

std::vector<std::string> Repository::getDirtyFiles() const
{
    std::vector<std::string> unstagedFiles;
    
    auto callback = [](const char *path, unsigned int status_flags, void *payload)
    {
        if(status_flags & (GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED |
                           GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_RENAMED | GIT_STATUS_WT_UNREADABLE)
          )
            static_cast<std::vector<std::string>*>(payload)->push_back(path);
        return 0;
    };
    
    checkLibGitError( git_status_foreach(smartLibGitRepository.get(), callback, &unstagedFiles) );
    
    return unstagedFiles;
}

std::vector<std::string> Repository::getConflictedFiles() const
{
    std::vector<std::string> conflictedFiles;
    
    auto callback = [](const char *path, unsigned int status_flags, void *payload)
    {
        if(status_flags & GIT_STATUS_CONFLICTED)
            static_cast<std::vector<std::string>*>(payload)->push_back(path);
        return 0;
    };
    
    checkLibGitError( git_status_foreach(smartLibGitRepository.get(), callback, &conflictedFiles) );
    
    return conflictedFiles;
}


void Repository::checkout(const Commit& toCheckout)
{
    forceableCheckout(toCheckout, false);
}

/* private */ void Repository::forceableCheckout(const Commit& toCheckout, bool force)
{
    if(!isOwnedByRepo(toCheckout))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }
    
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }
    
    git_checkout_options options;
    checkLibGitError( git_checkout_init_options(&options, GIT_CHECKOUT_OPTIONS_VERSION) );

    if(force)
    {
        options.checkout_strategy = GIT_CHECKOUT_SAFE;
    }
    else
    {
        options.checkout_strategy = GIT_CHECKOUT_FORCE;
    }

    auto smartLibGitCommit = getLibGitCommit(toCheckout.getSha());

    git_oid treeOid;
    git_oid_cpy(&treeOid, git_commit_tree_id(smartLibGitCommit.get()));

    git_object* treeishRaw = nullptr;
    checkLibGitError( git_object_lookup( &treeishRaw
                                       , smartLibGitRepository.get()
                                       , &treeOid
                                       , GIT_OBJ_TREE
                                       )
                    );

    uniquePtr<git_object> treeish(treeishRaw, git_object_free);
    
    git_oid_cpy(&treeOid, git_object_id(treeish.get()));
    checkLibGitError( git_checkout_tree(smartLibGitRepository.get(), treeish.get(), &options) );

    git_oid commitOid;
    git_oid_cpy(&commitOid, git_commit_id(smartLibGitCommit.get()));
    checkLibGitError( git_repository_set_head_detached(smartLibGitRepository.get(), &commitOid) );
}

void Repository::checkout(const Branch& toCheckout)
{
    forceableCheckout(toCheckout, false);
}

void Repository::forceableCheckout(const Branch& toCheckout, bool force)
{
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(!isOwnedByRepo(toCheckout))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }
    
    std::string fullReferenceName;
    if(toCheckout.isLocal())
    {
        fullReferenceName = "refs/heads/" + toCheckout.getName();
    }
    else
    {
        fullReferenceName = "refs/remotes/" + toCheckout.getName();
    }

    git_checkout_options options;
    checkLibGitError(  git_checkout_init_options(&options, GIT_CHECKOUT_OPTIONS_VERSION) );

    if(force)
    {
        options.checkout_strategy = GIT_CHECKOUT_SAFE;
    }
    else
    {
        options.checkout_strategy = GIT_CHECKOUT_FORCE;
    }
    
    git_reference* branchRaw = nullptr;
    checkLibGitError( git_reference_lookup(&branchRaw, smartLibGitRepository.get(), fullReferenceName.c_str()) );
    uniquePtr<git_reference> branch(branchRaw, git_reference_free);

    git_object* treeishRaw = nullptr;
    checkLibGitError( git_reference_peel(&treeishRaw, branch.get(), GIT_OBJ_TREE) );
    uniquePtr<git_object> treeish(treeishRaw, git_object_free);

    checkLibGitError( git_checkout_tree(smartLibGitRepository.get(), treeish.get(), &options) );

    checkLibGitError( git_repository_set_head(smartLibGitRepository.get(), fullReferenceName.c_str()) );
}

void Repository::merge(const Branch& toMergeIntoHead) //git merge --no-commit -Srecursive -Xno-renames
{
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(!isOwnedByRepo(toMergeIntoHead))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    if(isCheckedOut(*toMergeIntoHead.getHead()))
    {
        throw std::invalid_argument("Source and destination are the same.");
    }

    if(isHeadDetached()) //cannot checkout a remote branch so head is on a local one
    {
        throw std::domain_error("Currently merge is only supported between branches but head is detached.");
    }
    
    std::string fullMergeeBranchName;
    if(toMergeIntoHead.isLocal())
    {
        fullMergeeBranchName = "refs/heads/" + toMergeIntoHead.getName();
    }
    else
    {
        fullMergeeBranchName = "refs/remotes/" + toMergeIntoHead.getName();
    }
    
    git_reference* branchRaw = nullptr;
    checkLibGitError( git_reference_lookup(&branchRaw, smartLibGitRepository.get(), fullMergeeBranchName.c_str()) );
    uniquePtr<git_reference> branch(branchRaw, git_reference_free);
    
    git_annotated_commit* headToMergeRaw = nullptr;
    checkLibGitError( git_annotated_commit_from_ref(&headToMergeRaw, smartLibGitRepository.get(), branch.get()) );
    uniquePtr<git_annotated_commit> headToMerge(headToMergeRaw, git_annotated_commit_free);
    
    git_merge_analysis_t analysis;
    git_merge_preference_t preference;
    
    headToMergeRaw = headToMerge.get();
    checkLibGitError( git_merge_analysis( &analysis
                                        , &preference
                                        , smartLibGitRepository.get()
                                        , const_cast<const git_annotated_commit **>(&headToMergeRaw)
                                        , 1)
                    );

    if(GIT_MERGE_ANALYSIS_UNBORN & analysis)
    {
        throw std::domain_error("Currently the Repository class does not support bare repositories.");
    }
    
    if(GIT_MERGE_ANALYSIS_UP_TO_DATE & analysis)
    {
        return;
    }
                    
    if((GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY & preference) && !(GIT_MERGE_ANALYSIS_FASTFORWARD  & analysis))
    {
        return;
    }
    
    if(!(GIT_MERGE_PREFERENCE_NO_FASTFORWARD & preference) && (GIT_MERGE_ANALYSIS_FASTFORWARD & analysis))
    {
        git_reference* headBranchRaw = nullptr;
        checkLibGitError( git_repository_head(&headBranchRaw, smartLibGitRepository.get()) );
        uniquePtr<git_reference> headBranch(headBranchRaw, git_reference_free);
        
        checkout(toMergeIntoHead);
        
        std::string fastforwardSha = toMergeIntoHead.getHead()->getSha();
        git_oid fastforwardOid = shaToLibGitOid(fastforwardSha);
        
        //we know the checked out branch is local
        std::string fullHeadBranchName = std::string(git_reference_name(headBranch.get())); 
        std::string reflogMessage = std::string("branch ")
                                  + fullHeadBranchName
                                  + " fast-forwarded to "
                                  + toMergeIntoHead.getHead()->getSha();
        
        git_reference* newHeadBranchRaw = nullptr;
        checkLibGitError( git_reference_set_target(&newHeadBranchRaw
                                                  , headBranch.get()
                                                  , &fastforwardOid
                                                  , reflogMessage.c_str()
                                                  )
                        );
        uniquePtr<git_reference> newHeadBranch(newHeadBranchRaw, git_reference_free);
                        
        checkLibGitError( git_repository_set_head(smartLibGitRepository.get(), fullHeadBranchName.c_str()) );
    }
    else
    {
        if(getStagedFiles().size() > 0)
        {
            std::logic_error("Cannot non-fastforward merge when staged files are in the repository.");
        }

        git_checkout_options checkoutOptions;
        checkLibGitError( git_checkout_init_options(&checkoutOptions, GIT_CHECKOUT_OPTIONS_VERSION) );
        checkoutOptions.checkout_strategy = GIT_CHECKOUT_SAFE;
        checkoutOptions.checkout_strategy |= GIT_CHECKOUT_RECREATE_MISSING;
        
        git_merge_options mergeOptions;
        checkLibGitError( git_merge_init_options(&mergeOptions, GIT_MERGE_OPTIONS_VERSION) );
        
        headToMergeRaw = headToMerge.get();
        checkLibGitError( git_merge( smartLibGitRepository.get()
                                   , const_cast<const git_annotated_commit **>(&headToMergeRaw)
                                   , 1
                                   , &mergeOptions
                                   , &checkoutOptions
                                   )
                        );
    }
}

void Repository::startRebase(const Branch& toRebaseTo) // you cannot call this without a valid branch => then you can
                                                       // neither do so before init commit
{
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(!isOwnedByRepo(toRebaseTo))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    if(isHeadDetached())
    {
        throw std::domain_error("Currently rebase is not supported in detached HEAD state.");
    }

    std::string toRebaseToSha = toRebaseTo.getHead()->getSha();
    std::string headSha = getHead()->getSha();
    if(toRebaseToSha == headSha)
    {
        return;
    }

    std::string fullToRebaseToName = "refs/heads/" + toRebaseTo.getName(); //if it would be remote it would make this
                                                                           //detached HEAD
    std::string fullHeadBranchName = "refs/heads/" + getHeadBranch()->getName(); // currently no rebase allowed in
                                                                                 //detached head state

    git_oid toRebaseToOid = shaToLibGitOid(toRebaseToSha);
    git_oid headOid = shaToLibGitOid(headSha);
    int errorCode = git_graph_descendant_of( smartLibGitRepository.get()
                                           , &toRebaseToOid
                                           , &headOid
                                           );

    if(1 == errorCode) // head is ancestor of toRebaseTo -> needs fastforward
    {
        git_reference* headBranchRaw = nullptr;
        checkLibGitError( git_repository_head(&headBranchRaw, smartLibGitRepository.get()) );
        uniquePtr<git_reference> headBranch(headBranchRaw, git_reference_free);

        std::string reflogMessage = std::string("branch ")
                                  + fullHeadBranchName
                                  + " fast-forwarded to "
                                  + toRebaseToSha;

        git_reference* newHeadBranchRaw = nullptr;
        checkLibGitError( git_reference_set_target( &newHeadBranchRaw
                                                  , headBranch.get()
                                                  , &toRebaseToOid
                                                  , reflogMessage.c_str()
                                                  )
                        );
        uniquePtr<git_reference> newHeadBranch(newHeadBranchRaw, git_reference_free);

        checkLibGitError( git_repository_set_head(smartLibGitRepository.get(), fullToRebaseToName.c_str()) );
        hardReset();
        return;
    }
    else if(0 != errorCode)
    {
        checkLibGitError(errorCode);
    }
    else
    {
        int errorCode = git_graph_descendant_of( smartLibGitRepository.get()
                                               , &headOid
                                               , &toRebaseToOid
                                               );
        if(1 == errorCode) // toRebaseTo is ancestor of head
        {
            return;
        }
        else if(0 != errorCode)
        {
            checkLibGitError(errorCode);
        }
    }

    
    git_reference* branchToRebaseToRaw = nullptr;
    checkLibGitError( git_reference_lookup( &branchToRebaseToRaw
                                          , smartLibGitRepository.get()
                                          , fullToRebaseToName.c_str()
                                          )
                    );
    uniquePtr<git_reference> branchToRebaseTo(branchToRebaseToRaw, git_reference_free);
    
    git_annotated_commit* headToRebaseToRaw = nullptr;
    checkLibGitError( git_annotated_commit_from_ref( &headToRebaseToRaw
                                                   , smartLibGitRepository.get()
                                                   , branchToRebaseTo.get()
                                                   )
                    );
    uniquePtr<git_annotated_commit> headToRebaseTo(headToRebaseToRaw, git_annotated_commit_free);

    startRebase(std::move(headToRebaseTo));

}

/* private */ void Repository::startRebase(uniquePtr<git_annotated_commit> headToRebaseTo)
{
    git_rebase_options options;
    checkLibGitError( git_rebase_init_options(&options, GIT_REBASE_OPTIONS_VERSION) );

    //nullptr -> const git_annotated_commit* The terminal commit to rebase, or NULL to rebase the current branch
    //nullptr -> const git_annotated_commit* The commit to begin rebasing from, or NULL to rebase all reachable commits
    git_rebase* rebaseObjectRaw = nullptr;
    checkLibGitError( git_rebase_init( &rebaseObjectRaw
                                     , smartLibGitRepository.get()
                                     , nullptr
                                     , nullptr
                                     , headToRebaseTo.get()
                                     , &options
                                     )
                    );
    uniquePtr<git_rebase> rebaseObject(rebaseObjectRaw, git_rebase_free);
}

void Repository::applyNextRebasePatch()
{
    if(!isInRebaseState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }
    
    git_rebase_options options;
    checkLibGitError( git_rebase_init_options(&options, GIT_REBASE_OPTIONS_VERSION) );
    
    git_rebase* rebaseRaw = nullptr;
    checkLibGitError( git_rebase_open(&rebaseRaw, smartLibGitRepository.get(), &options) );
    uniquePtr<git_rebase> rebase(rebaseRaw, git_rebase_free);
    
    std::size_t operationIndex = git_rebase_operation_current(rebase.get());

    git_rebase_operation* operation = nullptr;
    if(GIT_REBASE_NO_OPERATION != operationIndex)
    {
        git_rebase_operation* operation = git_rebase_operation_byindex(rebase.get(), operationIndex);
        if(nullptr == operation)
        {
            throw std::logic_error("Rebase operation out of index.");
        }
    }

    checkLibGitError( git_rebase_next(&operation, rebase.get()) );
}

//including the current one
std::size_t Repository::numOfRemainingRebasePatches() const
{
    if(!isInRebaseState())
    {
        return 0;
    }
    
    git_rebase_options options;
    checkLibGitError( git_rebase_init_options(&options, GIT_REBASE_OPTIONS_VERSION) );
    
    git_rebase* rebaseRaw = nullptr;
    checkLibGitError( git_rebase_open(&rebaseRaw, smartLibGitRepository.get(), &options) );
    uniquePtr<git_rebase> rebase(rebaseRaw, git_rebase_free);

    std::size_t operationIndex = git_rebase_operation_current(rebase.get());

    if(GIT_REBASE_NO_OPERATION == operationIndex)
    {
        return git_rebase_operation_entrycount(rebase.get());
    }
    
    return git_rebase_operation_entrycount(rebase.get()) - operationIndex - 1;
}

void Repository::commitCurrentRebasePatch()
{
    if(!isInRebaseState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }
    
    git_rebase_options options;
    checkLibGitError( git_rebase_init_options(&options, GIT_REBASE_OPTIONS_VERSION) );
    
    git_rebase* rebaseRaw = nullptr;
    checkLibGitError( git_rebase_open(&rebaseRaw, smartLibGitRepository.get(), &options) );
    uniquePtr<git_rebase> rebase(rebaseRaw, git_rebase_free);
        
    git_signature* signatureRaw = nullptr;
    checkLibGitError( git_signature_default(&signatureRaw, smartLibGitRepository.get()) );
    uniquePtr<git_signature> signature(signatureRaw, git_signature_free);
    
    git_oid oid;
    
    //null -> keep original commit author
    //null -> keep original commit message encoding
    //null -> keep original commit message
    int error = git_rebase_commit(&oid, rebase.get(), nullptr, signature.get(), nullptr, nullptr);

    if(GIT_EAPPLIED == error)
    {
        //nothing to do in this case
    }
    else
    {
        checkLibGitError( error );
    }

    if(numOfRemainingRebasePatches() == 0)
    {
        checkLibGitError( git_rebase_finish(rebase.get(), signature.get()) );
    }
}

std::unique_ptr<Remote> Repository::getRemote() const
{
    git_strarray remoteNameList = {nullptr, 0};
    
    int error = git_remote_list(&remoteNameList, smartLibGitRepository.get());
    
    if(error)
    {
        git_strarray_free(&remoteNameList);
        checkLibGitError(error);
    }
    
    std::size_t remoteCount = remoteNameList.count;
    if(0 == remoteCount)
    {
        git_strarray_free(&remoteNameList);
        return std::unique_ptr<Remote>(nullptr);
    }
    else if(1 != remoteCount)
    {
        git_strarray_free(&remoteNameList);
        throw std::domain_error("Currently only repositories with one or no remotes are supported.");
    }
    
    std::string remoteName = remoteNameList.strings[0];

    git_strarray_free(&remoteNameList);

    return std::unique_ptr<Remote>(new Remote(getPath(), remoteName));
}

void Repository::fetch(AuthenticationCallback getUserCredentialsCallback /* = nullptr */
                      , std::string reflog /* = "fetch" */)
{
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(!hasRemote())
    {
        throw std::logic_error("Repository does not have a remote.");
    }

    Remote remote = *getRemote();
    
    auto smartLibGitRemote = getLibGitRemote(remote.getName());
    
    git_fetch_options fetchOptions;
    checkLibGitError( git_fetch_init_options( &fetchOptions, GIT_FETCH_OPTIONS_VERSION ) );
    
    if(nullptr != getUserCredentialsCallback)
    {
        auto credentialCallback = getCredentialsCallbackWrapper(getUserCredentialsCallback);
        fetchOptions.callbacks.credentials = credentialCallback;
        fetchOptions.callbacks.payload = &getUserCredentialsCallback;
    }
    
    //null -> refspecs, NULL to use the configured ones
    checkLibGitError( git_remote_fetch(smartLibGitRemote.get(), nullptr, &fetchOptions, reflog.c_str()) );
}

void Repository::pull(AuthenticationCallback getUserCredentialsCallback /* = nullptr */)
{
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(!hasRemote())
    {
        throw std::logic_error("Repository does not have a remote.");
    }

    if(isHeadDetached())
    {
        throw std::domain_error("Currently pull is only possible from a branch.");
    }
    Branch checkedOutBranch = *getHeadBranch();
    
    std::string upstreamRemote = getConfigEntry("branch." + checkedOutBranch.getName() + ".remote");
    if("" == upstreamRemote)
    {
        throw std::logic_error("Remote for the branch could not be found.");
    }

    std::string fullUpstreamMerge = getConfigEntry("branch." + checkedOutBranch.getName() + ".merge");
    std::string upstreamMerge = fullUpstreamMerge.substr(fullUpstreamMerge.rfind('/') + 1, fullUpstreamMerge.size());
    
    if("" == upstreamMerge)
    {
        throw std::logic_error("Upstream branch could not be found.");
    }
    
    //std::string fullMergeBranchName = "refs/remotes/" + upstreamRemote + "/" + upstreamMerge;
    
    fetch(getUserCredentialsCallback, "pull");
    merge(Branch(getPath(), upstreamRemote + "/" + upstreamMerge, false));
}

void Repository::push( AuthenticationCallback getUserCredentialsCallback /* = nullptr */) const
{
    if(!isInNormalState())
    {
        throw std::logic_error("Repository state prevents operation.");
    }

    if(!hasRemote())
    {
        throw std::logic_error("Repository does not have a remote.");
    }
    
    Remote remote = *getRemote();
    
    auto smartLibGitRemote = getLibGitRemote(remote.getName());
    
    git_push_options options;
    checkLibGitError( git_push_init_options( &options, GIT_PUSH_OPTIONS_VERSION ) );
    
    if(nullptr != getUserCredentialsCallback)
    {
        auto credentialCallback = getCredentialsCallbackWrapper(getUserCredentialsCallback);
        options.callbacks.credentials = credentialCallback;
        options.callbacks.payload = &getUserCredentialsCallback;
    }
    
    git_strarray refspecs = {nullptr, 0};
    
    uniquePtr<git_strarray> smartRefspecs(&refspecs, git_strarray_free);
    
    checkLibGitError( git_remote_get_push_refspecs(smartRefspecs.get(), smartLibGitRemote.get()) );
    
    
    checkLibGitError( git_remote_push( smartLibGitRemote.get(), smartRefspecs.get(), &options) );
}

std::unique_ptr<Branch> Repository::getHeadBranch() const
{
    if(!isABranchCheckedOut())
    {
        return std::unique_ptr<Branch>(nullptr);
    }
    
    git_reference* referenceRaw = nullptr;
    checkLibGitError( git_repository_head(&referenceRaw, smartLibGitRepository.get()) );
    uniquePtr<git_reference> reference(referenceRaw, git_reference_free);
    
    if(git_reference_is_remote(reference.get()))
    {
        throw std::domain_error("Invalid state: checked out branch is remote.");
    }
    
    std::string fullBranchName = git_reference_name(reference.get());
    std::string branchName = fullBranchName.substr(fullBranchName.rfind('/') + 1, fullBranchName.size());
    
    return std::unique_ptr<Branch>(new Branch(getPath(), branchName, true));
}

bool Repository::isHeadDetached() const
{
    int error = git_repository_head_detached(smartLibGitRepository.get());
    
    if(1 == error)
    {
        return true;
    }
    else if(0 == error)
    {
        return false;
    }
    else
    {
        checkLibGitError(error); //throws exception in any other case
        throw std::logic_error("Execution should never reach this statement."); //We don't like warnings right?
    }
}


bool Repository::isRemote(const std::string& name) const
{
    git_repository* repository = smartLibGitRepository.get();
    
    git_remote* remoteRaw = nullptr;
    int error = git_remote_lookup(&remoteRaw, repository, name.c_str());
    uniquePtr<git_remote> remote(remoteRaw, git_remote_free);
    
    if(GIT_ENOTFOUND == error || GIT_EINVALIDSPEC == error)
    {
        return false;
    }
    
    checkLibGitError( error );
    
    return 0 == error;
}

bool Repository::hasRemote() const
{
    return 0 != getRemoteCount();
}

void Repository::addRemote(const std::string& url, const std::string& name /* = "origin" */)
{
    if(hasRemote())
    {
        throw std::domain_error("Currently only repositories with one or no remotes are supported.");
    }

    git_remote* remoteRaw = nullptr;
    checkLibGitError( git_remote_create(&remoteRaw, smartLibGitRepository.get(), name.c_str(), url.c_str()) );
    uniquePtr<git_remote> remote(remoteRaw, git_remote_free);
}

std::vector<std::unique_ptr<Commit>> Repository::getParentCommits(const Commit& commit) const
{
    if(!isOwnedByRepo(commit))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }
    
    std::vector<std::unique_ptr<Commit>> parentCommits;
    
    uniquePtr<git_commit> smartLibGitCommit = getLibGitCommit(commit.getSha());
    std::vector<std::string> parentShas = getParentShas(smartLibGitCommit.get());
    
    for(const auto& sha : parentShas)
    {
        parentCommits.push_back(std::unique_ptr<Commit>(new Commit(pathToRepository, sha)));
    }
    
    return parentCommits;
}

std::unique_ptr<Commit> Repository::getHead() const
{
    git_commit* head = getHeadLibGitCommit().get();
    std::string sha = getLibGitCommitSha(head);
    
    return std::unique_ptr<Commit>(new Commit(pathToRepository, sha));
}

bool Repository::isCheckedOut(const Git::Commit& toCheck) const
{
    if(!isOwnedByRepo(toCheck))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    auto smartHeadLibGitCommit = getHeadLibGitCommit();

    std::string headSha = getLibGitCommitSha(smartHeadLibGitCommit.get());

    if(toCheck.getSha() == headSha)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Repository::isCheckedOut(const Git::Branch& toCheck) const
{
    if(!isOwnedByRepo(toCheck))
    {
        throw std::invalid_argument("Argument is not owned by the repository.");
    }

    if(!toCheck.isLocal())
    {
        return false;
    }

    git_reference* branchRaw = nullptr;
    checkLibGitError( git_branch_lookup( &branchRaw
                                       , smartLibGitRepository.get()
                                       , toCheck.getName().c_str()
                                       , GIT_BRANCH_LOCAL
                                       )
                    );
    uniquePtr<git_reference> branch(branchRaw, git_reference_free);

    int error = git_branch_is_head(branch.get());
    if(0 == error || 1 == error)
    {
        return error;
    }
    else
    {
        checkLibGitError(error);
        return false; //checkLibGitError throws exception but i don't like warnings
    }
}

bool Repository::isABranchCheckedOut() const
{
    return !git_repository_head_unborn(smartLibGitRepository.get()) && !isHeadDetached();
}

bool Repository::isOwnedByRepo(const Commit& toCheck) const
{
    if(getPath() != toCheck.getRepositoryPath())
    {
        return false;
    }

    git_oid oid = shaToLibGitOid(toCheck.getSha());
    
    git_commit* commitRaw = nullptr;
    int error = git_commit_lookup(&commitRaw, smartLibGitRepository.get(), &oid);
    uniquePtr<git_commit> commit(commitRaw, git_commit_free);
    
    if(GIT_ENOTFOUND == error)
    {
        return false;
    }
    
    checkLibGitError( error );
    
    return 0 == error;
}

bool Repository::isOwnedByRepo(const Branch& toCheck) const
{
    if(getPath() != toCheck.getRepositoryPath())
    {
        return false;
    }

    std::string fullReferenceName;
    if(toCheck.isLocal())
    {
        fullReferenceName = "refs/heads/" + toCheck.getName();
    }
    else
    {
        fullReferenceName = "refs/remotes/" + toCheck.getName();
    }
    
    return isReferenceOwnedByRepo(fullReferenceName);
}

bool Repository::isOwnedByRepo(const Tag& toCheck) const
{
    if(getPath() != toCheck.getRepositoryPath())
    {
        return false;
    }
        
    std::string fullReferenceName = "refs/tags/" + toCheck.getName();
    return isReferenceOwnedByRepo(fullReferenceName);
}

bool Repository::isOwnedByRepo(const Remote& toCheck) const
{
    if(getPath() != toCheck.getRepositoryPath())
    {
        return false;
    }
    
    return isRemote(toCheck.getName());
}

bool Repository::isReferenceOwnedByRepo(const std::string& fullReferenceName) const
{
    git_reference* referenceRaw = nullptr;
    int error = git_reference_lookup(&referenceRaw, smartLibGitRepository.get(), fullReferenceName.c_str());
    uniquePtr<git_reference> reference(referenceRaw, git_reference_free);
    
    if(GIT_ENOTFOUND == error)
    {
        return false;
    }
    
    checkLibGitError( error );
    
    return 0 == error;
}

/*private*/ std::string Repository::getConfigEntry(const std::string& key) const
{
    git_config* cfgRaw = nullptr;
    checkLibGitError( git_repository_config(&cfgRaw, smartLibGitRepository.get()) );
    uniquePtr<git_config> cfg(cfgRaw, git_config_free);
    
    git_config_entry* entryRaw = nullptr;
    checkLibGitError( git_config_get_entry(&entryRaw, cfg.get(), key.c_str()) );
    uniquePtr<git_config_entry> entry(entryRaw, git_config_entry_free);
    
    if(nullptr == entry.get())
    {
        return "";
    }
    else
    {
        return entry->value;
    }
}

//relative path to repository root
/*private*/ bool Repository::isFileInRepository(const std::string& relativePath) const
{
    unsigned int status;
    int errorCode = git_status_file(&status, smartLibGitRepository.get(), relativePath.c_str());
    return 0 == errorCode;
}

//relative path to repository root
/*private*/ bool Repository::doesGitIgnoreApply(const std::string& relativePath)
{
    int shouldBeIgnored;
    checkLibGitError( git_status_should_ignore(&shouldBeIgnored, smartLibGitRepository.get(), relativePath.c_str()) );
    return shouldBeIgnored != 0;
}

/*private*/ Repository::State Repository::getState() const
{
    Repository::State state;
    int libGitState = git_repository_state(smartLibGitRepository.get());
    
    switch(libGitState)
    {
        case GIT_REPOSITORY_STATE_NONE :
            state = State::NORMAL;
            break;
        case GIT_REPOSITORY_STATE_MERGE :
            state = State::MERGE;
            break;
        case GIT_REPOSITORY_STATE_REBASE :
        case GIT_REPOSITORY_STATE_REBASE_INTERACTIVE:
        case GIT_REPOSITORY_STATE_REBASE_MERGE:
            state = State::REBASE;
            break;
        default:
            state = State::UNKNOWN;
    }
    
    return state;
}

/*private*/ bool Repository::isStaged(const std::string& relativePath) const
{
    unsigned int status;
    checkLibGitError( git_status_file(&status, smartLibGitRepository.get(), relativePath.c_str()) );
    
    if(status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED |
                 GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE))
    {
        return true;
    }
    
    return false;
}


/*private*/ std::size_t Repository::getRemoteCount() const
{
    git_strarray remoteNameList = {nullptr, 0};
    
    int error = git_remote_list(&remoteNameList, smartLibGitRepository.get());
    
    if(error)
    {
        git_strarray_free(&remoteNameList);
        checkLibGitError(error);
    }
    
    std::size_t remoteCount = remoteNameList.count;

    git_strarray_free(&remoteNameList);

    return remoteCount;
}

/*private*/ std::string Repository::messagePrettify(std::string message) const
{
    {
        message.erase(message.begin(), std::find_if( message.begin()
                                                   , message.end()
                                                   , std::not1(std::ptr_fun<int, int>(std::isspace))
                                                   )
                     ); // ltrim

        return message + '\n';
    }
}
