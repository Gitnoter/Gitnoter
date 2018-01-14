#include "Commit.h"

#include "Repository.h"
#include "LibGitException.h"
#include "SmartLibGitHeapPointerTemplate.hpp"

using Commit = Git::Commit;
using Repository = Git::Repository;

Commit::Commit(const std::string& _repositoryKey
              , const std::string& _sha
              ) : RepositoryDependant(_repositoryKey)
                , sha(_sha)
{

    Repository repository(_repositoryKey);

    if(!repository.isOwnedByRepo(*this))
    {
        throw std::invalid_argument("Not found in repository.");
    }

    git_commit* commit = nullptr;

    commit = getLibGitCommit(sha).get();

    message = std::string(git_commit_message(commit));
    author = getAuthor(commit);
    committer = getCommitter(commit);
    parentShas = getParentShas(commit);
    commitDate = committer.when;

    if(parentShas.size() > 2)
    {
        throw std::domain_error("Currently only commits with at most two parents are supported.");
    }
}

/*private*/ Commit::Signature Commit::getAuthor(const git_commit* commit) const
{
    uniquePtr<git_signature> signature = getAuthorLibGitSignature(commit);
    Signature author( signature->name
                    , signature->email
                    , static_cast<std::time_t>(signature->when.time) + signature->when.offset * 60
                    );

    return author;
}

/*private*/ Commit::Signature Commit::getCommitter(const git_commit* commit) const
{
    uniquePtr<git_signature> signature = getCommitterLibGitSignature(commit);
    Signature committer( signature->name
                       , signature->email
                       , static_cast<std::time_t>(signature->when.time) + signature->when.offset * 60
                       );

    return committer;
}

/* private */ std::vector<std::string> Commit::getParentShas(const git_commit* commit) const
{
    unsigned int parentCount = git_commit_parentcount(commit);
    if(parentCount > 2)
    {
        throw std::domain_error("Currently commits with more than two parents are not supported.");
    }

    std::vector<std::string> shas;
    for(std::size_t i = 0; i < parentCount; ++i)
    {
        shas.push_back(libGitOidToSha(git_commit_parent_id(commit, i)));
    }

    return shas;
}

/* private */ std::unique_ptr< git_signature
                             , void(*)(git_signature*)
                             > Commit::getAuthorLibGitSignature(const git_commit* commit) const
{
    git_signature* signatureRaw = nullptr;
    checkLibGitError( git_signature_dup(&signatureRaw, git_commit_author(commit)) );
    uniquePtr<git_signature> author(signatureRaw, git_signature_free);

    return author;
}

/* private */ std::unique_ptr< git_signature
                             , void(*)(git_signature*)
                             > Commit::getCommitterLibGitSignature(const git_commit* commit) const
{
    git_signature* signatureRaw = nullptr;
    checkLibGitError( git_signature_dup(&signatureRaw, git_commit_committer(commit)) );
    uniquePtr<git_signature> committer(signatureRaw, git_signature_free);

    return committer;
}
