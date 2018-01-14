#include "Remote.h"

#include "SmartLibGitHeapPointerTemplate.hpp"

using Remote = Git::Remote;

Remote::Remote(const std::string& _repositoryKey, const std::string& _name) : RepositoryDependant(_repositoryKey)
                                                                            , name(_name)
{
    uniquePtr<git_remote> smartLibGitRemote = getLibGitRemote(name);
    url = git_remote_url(smartLibGitRemote.get());
}
