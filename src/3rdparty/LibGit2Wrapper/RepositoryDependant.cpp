#include "RepositoryDependant.h"

#include "GitEnvironment.h"
#include "SmartLibGitHeapPointerTemplate.hpp"

#include <stdexcept>
#include <vector>

using RepositoryDependant = Git::RepositoryDependant;

std::map<std::string, std::shared_ptr<git_repository>> RepositoryDependant::instances = {};

RepositoryDependant::RepositoryDependant(std::string _fullPathToRepo) : pathToRepository(_fullPathToRepo)
{
    setLibGitRepository();
}

RepositoryDependant::~RepositoryDependant()
{
    auto instance = RepositoryDependant::instances.find(pathToRepository);

    smartLibGitRepository.reset();

    if( RepositoryDependant::instances.end() != instance && 1 == instance->second.use_count())
    {
        RepositoryDependant::instances.erase(instance);
    }
}

uniquePtr<git_commit> RepositoryDependant::getHeadLibGitCommit() const
{
    git_repository* repository = smartLibGitRepository.get();

    git_oid oid;

    checkLibGitError( git_reference_name_to_id( &oid, repository, "HEAD" ) );

    git_commit* commitRaw = nullptr;
    checkLibGitError( git_commit_lookup( &commitRaw, repository, &oid ) );
    uniquePtr<git_commit> commit(commitRaw, git_commit_free);

    return commit;
}

std::string RepositoryDependant::getLibGitCommitSha(const git_commit* commit) const
{
    char sha[GIT_OID_HEXSZ + 1];
    const git_oid* oid = git_commit_id(commit);
    git_oid_tostr(sha, GIT_OID_HEXSZ + 1, oid);

    return std::string(sha);
}

git_oid RepositoryDependant::shaToLibGitOid(const std::string& sha) const
{
    git_oid out;
    checkLibGitError( git_oid_fromstr(&out, sha.c_str()) );
    return out;
}

uniquePtr<git_commit> RepositoryDependant::getLibGitCommit(const std::string& sha) const
{
    git_repository* repository = smartLibGitRepository.get();
    git_oid oid;

    checkLibGitError( git_oid_fromstr(&oid, sha.c_str()) );

    git_commit* commitRaw = nullptr;
    checkLibGitError( git_commit_lookup( &commitRaw, repository, &oid ) );
    uniquePtr<git_commit> commit(commitRaw, git_commit_free);

    return commit;
}

std::string RepositoryDependant::libGitOidToSha(const git_oid* oid) const
{
    return std::string(git_oid_tostr_s(oid));
}

uniquePtr<git_remote> RepositoryDependant::getLibGitRemote(const std::string& name) const
{
    git_repository* repository = smartLibGitRepository.get();

    git_remote* remoteRaw = nullptr;
    checkLibGitError( git_remote_lookup(&remoteRaw, repository, name.c_str()) );
    uniquePtr<git_remote> remote(remoteRaw, git_remote_free);

    return remote;
}
std::vector<std::string> RepositoryDependant::getParentShas(const git_commit* commit) const
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

uniquePtr<git_tree> RepositoryDependant::getLibGitTreeForCommit(const git_commit* commit) const
{
    git_tree* treeRaw = nullptr;
    checkLibGitError( git_commit_tree(&treeRaw, commit) );
    return uniquePtr<git_tree>(treeRaw, git_tree_free);
}


/* private */ void RepositoryDependant::setLibGitRepository()
{
    auto instance = RepositoryDependant::instances.find(pathToRepository);
    if( RepositoryDependant::instances.end() != instance )
    {
        smartLibGitRepository = instance->second;
    }
    else
    {
        smartLibGitRepository = openRepository();
        std::string fullPathToRepo = git_repository_workdir(smartLibGitRepository.get()); //relative path might be given
        //std::string fullPathToRepo = git_repository_path(smartLibGitRepository.get()); //would return .../.git/

        if(pathToRepository != fullPathToRepo)
        {
            pathToRepository = fullPathToRepo;
            setLibGitRepository();
        }
        else
        {

            std::pair<std::string, std::shared_ptr<git_repository>> newElem( pathToRepository
                                                                           , std::move(smartLibGitRepository)
                                                                           );

            RepositoryDependant::instances.insert(std::move(newElem));

            smartLibGitRepository = RepositoryDependant::instances[pathToRepository];
        }
    }
}

/* private */ std::shared_ptr<git_repository> RepositoryDependant::openRepository()
{
    GitEnvironment env;
    if(!env.isRepository(pathToRepository))
    {
        throw std::invalid_argument("No repository at the given location.");
    }

    git_repository* repositoryRaw = nullptr;
    checkLibGitError( git_repository_open(&repositoryRaw, pathToRepository.c_str()) );

    std::shared_ptr<git_repository> repository(repositoryRaw, git_repository_free);

    if(git_repository_is_bare(repository.get()))
    {
        throw std::domain_error("Currently bare repositories are not supported.");
    }

    return repository;
}
