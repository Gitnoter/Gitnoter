#include "Branch.h"

#include "Repository.h"
#include "SmartLibGitHeapPointerTemplate.hpp"

#include <stdexcept>

using Reference = Git::Reference;
using Branch = Git::Branch;
using Repository = Git::Repository;

Branch::Branch(const std::string& _repositoryKey
              , const std::string& _name
              , bool _isLocal /* = true */
              ) : Reference(_repositoryKey, _name), local(_isLocal)
{
    Repository repository(_repositoryKey);
    
    if(!repository.isOwnedByRepo(*this))
    {
        throw std::invalid_argument("Not found in repository.");
    }
}


/* private */ uniquePtr<git_commit> Branch::getReferenceHeadLibGitCommit(const std::string& branchName) const
{
    std::string fullReferenceName;
    if(isLocal())
    {
        fullReferenceName = "refs/heads/" + branchName;
    }
    else
    {
        fullReferenceName = "refs/remotes/" + branchName;
    }

    return Reference::getReferenceHeadLibGitCommit(fullReferenceName);
}
