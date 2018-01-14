#include "GitEnvironment.h"

#include <git2.h>

using GitEnvironment = Git::GitEnvironment;

void GitEnvironment::init(const std::string& path)
{
    git_repository* repository = nullptr;
    
    git_repository_init_options options;
    checkLibGitError( git_repository_init_init_options(&options, GIT_REPOSITORY_INIT_OPTIONS_VERSION) );
    
    options.flags |= GIT_REPOSITORY_INIT_MKPATH;
    
    checkLibGitError(git_repository_init_ext(&repository, path.c_str(), &options));
    
    git_repository_free(repository);
}

void GitEnvironment::clone(const std::string& pathToRepo
                          , const std::string& url
                          , Authenticable::AuthenticationCallback getUserCredentialsCallback /* = nullptr */
                          )
{
    git_clone_options options;
    checkLibGitError( git_clone_init_options(&options, GIT_CLONE_OPTIONS_VERSION) );
    options.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    if(nullptr != getUserCredentialsCallback)
    {
        auto credentialCallback = getCredentialsCallbackWrapper(getUserCredentialsCallback);
        options.fetch_opts.callbacks.credentials = credentialCallback;
        options.fetch_opts.callbacks.payload = &getUserCredentialsCallback;
    }
    
    git_repository* repository = nullptr;
    
    checkLibGitError( git_clone(&repository, url.c_str(), pathToRepo.c_str(), &options) );
    
    git_repository_free(repository);
}

bool GitEnvironment::isRepository(const std::string& path) const
{
    git_buf out = { nullptr, 0, 0};
    int error = git_repository_discover(&out, path.c_str(), 1, path.c_str()); // stores the found repository
    std::size_t usedMemory = out.size;
    git_buf_free(&out);

    if(GIT_ENOTFOUND == error)
    {
        return false;
    }

    checkLibGitError(error);

    return usedMemory > 0;
}
