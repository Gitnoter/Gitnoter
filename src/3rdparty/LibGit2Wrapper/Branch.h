#ifndef BRANCH_H
#define BRANCH_H

#include "Reference.h"

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A Branch osztály egy DTO osztály a `git branch` paranccsal vagy
     *        azzal egyenértékűvel létrehozott objektumok reprezentálására.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Branch : public Git::Reference
    {
    private:
        bool local; /**<A local repository tulajdona-e az objektum vagy egy remote branch-et követ. */

        std::unique_ptr<git_commit, void(*)(git_commit*)>
            getReferenceHeadLibGitCommit(const std::string& branchName) const;
        
    public:
        /**
         * @brief Branch konstruktor
         * @param _repositoryKey
         *        A branch-et tartalmazó repository teljes elérési útvonala.
         * @param _name
         *        A branch neve a repositoryban.
         * @param _isLocal
         *        Visszaadja, hogy a Branch objektum egy lokális branch-et jelöl vagy egy olyat,
         *        amely a távoli repository egy branch-ét követi-e.
         *
         * Ha az objektum nem található a repositoryban, akkor std::invalid_argument dobódik.<BR>
         * Ha az objektum kezelése közben hiba történik, akkor Git::LibGitException dobódhat.
         */
        Branch(const std::string& _repositoryKey, const std::string& _name, bool _isLocal = true);

        /**
         * @brief isLocal Visszaadja, hogy a Branch objektum egy lokális branch-et jelöl vagy egy olyat,
         *        amely a távoli repository egy branch-ét követi.
         * @return Lokális vagy remote branch.
         */
        bool isLocal() const { return local; }
    };
}

#endif //BRANCH_H
