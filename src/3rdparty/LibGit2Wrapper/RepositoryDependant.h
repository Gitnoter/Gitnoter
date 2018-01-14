#ifndef RepositoryDependant_H
#define RepositoryDependant_H

#include "LibGitInitializer.h"

#include <memory>
#include <map>
#include <string>

#include <git2.h>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A RepositoryDependant osztály ősként funkcionál az egy adott repositoryhoz köthető objektumok számára.
     *        Csak ezeken az osztályokon keresztül példányosítható.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT RepositoryDependant : public LibGitInitializer
    {
    private:
        static std::map<std::string, std::shared_ptr<git_repository>> instances;

        std::shared_ptr<git_repository> openRepository();
        void setLibGitRepository();

    protected:
        std::string pathToRepository;
        std::shared_ptr<git_repository> smartLibGitRepository;

        RepositoryDependant(std::string _pathToRepo);
        RepositoryDependant(const RepositoryDependant&) = default;
        RepositoryDependant& operator=(const RepositoryDependant&) = default;

        std::unique_ptr<git_commit, void(*)(git_commit*)> getHeadLibGitCommit() const;
        std::unique_ptr<git_commit, void(*)(git_commit*)> getLibGitCommit(const std::string& sha) const;
        std::string getLibGitCommitSha(const git_commit* commit) const;
        git_oid shaToLibGitOid(const std::string& sha) const;
        std::string libGitOidToSha(const git_oid* oid) const;

        std::vector<std::string> getParentShas(const git_commit* commit) const;
        std::unique_ptr<git_tree, void(*)(git_tree*)> getLibGitTreeForCommit(const git_commit* commit) const;

        std::unique_ptr<git_remote, void(*)(git_remote*)> getLibGitRemote(const std::string& name) const;

    public:
        /**
         * @brief getRepositoryPath Visszaadja az objektumot karbantartó repository elérési útját.
         * @return Teljes elérési útvonal a repository work directory részéig bezárólag.
         */
        const std::string& getRepositoryPath() const { return pathToRepository; }

        virtual ~RepositoryDependant();
};
}

#endif // RepositoryDependant_H
