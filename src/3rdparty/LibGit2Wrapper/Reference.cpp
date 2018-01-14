#include "Reference.h"

#include "Commit.h"
#include "SmartLibGitHeapPointerTemplate.hpp"

using Reference = Git::Reference;
using Commit = Git::Commit;

/* protected */ Reference::Reference( const std::string& _repositoryKey
                                    , const std::string& _name) : RepositoryDependant(_repositoryKey)
                                                                , name(_name)
{
}

std::unique_ptr<Commit> Reference::getHead() const
{
    auto libGitHeadCommit = getReferenceHeadLibGitCommit(name);
    
    std::string sha = getLibGitCommitSha(libGitHeadCommit.get());
    
    return std::unique_ptr<Commit>(new Commit(pathToRepository, sha));
}

/* virtual protected */ uniquePtr<git_commit>
    Reference::getReferenceHeadLibGitCommit(const std::string& fullReferenceName) const
{
    git_repository* repository = smartLibGitRepository.get();

    git_reference* referenceRaw = nullptr;
    checkLibGitError( git_reference_lookup(&referenceRaw, repository, fullReferenceName.c_str()) );
    uniquePtr<git_reference> reference(referenceRaw, git_reference_free);

    git_annotated_commit* headCommitRaw = nullptr;
    checkLibGitError( git_annotated_commit_from_ref(&headCommitRaw, repository, reference.get()) );
    uniquePtr<git_annotated_commit> headCommit(headCommitRaw, git_annotated_commit_free);

    git_oid oid;
    git_oid_cpy(&oid, git_annotated_commit_id(headCommit.get()));

    return getLibGitCommit(libGitOidToSha(&oid));
}
