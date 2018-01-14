#include "Tag.h"

#include "Repository.h"
#include "SmartLibGitHeapPointerTemplate.hpp"

#include <stdexcept>

using Reference = Git::Reference;
using Tag = Git::Tag;
using Repository = Git::Repository;

Tag::Tag(const std::string& _repositoryKey, const std::string& _name) : Reference(_repositoryKey, _name)
{
    Repository repository(_repositoryKey);
    
    if(!repository.isOwnedByRepo(*this))
        throw std::invalid_argument("Not found in repository.");
}

/* private */ uniquePtr<git_commit> Tag::getReferenceHeadLibGitCommit(const std::string& tagName) const
{
    std::string fullReferenceName = "refs/tags/" + tagName;

    return Reference::getReferenceHeadLibGitCommit(fullReferenceName);
}
