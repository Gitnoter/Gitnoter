#include "LibGitInitializer.h"

#include <git2.h>

#include "LibGitException.h"

using LibGitInitializer = Git::LibGitInitializer;
using LibGitException = Git::LibGitException;


int LibGitInitializer::numOfLibGitInitializers = 0;

void LibGitInitializer::checkLibGitError(int errorCode) const
{
    if(0 != errorCode)
    {
        //std::cout << giterr_last()->message;
        if(giterr_last())
        {
            throw LibGitException(*giterr_last(), errorCode);
        }
        else
        {
            if(GIT_EUSER == errorCode)
            {
                throw LibGitException("User interruption", errorCode);
            }
            else
            {
                throw LibGitException("Unknown", errorCode);
            }
        }
    }
}

LibGitInitializer::LibGitInitializer()
{
    init();
}

LibGitInitializer::LibGitInitializer(const LibGitInitializer& toCopy)
{
    (void) toCopy;
    
    init();
}

LibGitInitializer& LibGitInitializer::operator=(const LibGitInitializer& toAssign)
{
    (void) toAssign;
    
    init();
    return *this;
}

LibGitInitializer::~LibGitInitializer()
{
    --LibGitInitializer::numOfLibGitInitializers;
    
    if(0 == LibGitInitializer::numOfLibGitInitializers)
    {
        git_libgit2_shutdown();
    }
}

/*private*/ void LibGitInitializer::init() const
{
    if(0 == LibGitInitializer::numOfLibGitInitializers)
    {
        git_libgit2_init();
    }
    
    ++LibGitInitializer::numOfLibGitInitializers;
}
