#ifndef TAG_H
#define TAG_H

#include "Reference.h"

namespace Git
{
    /**
     * @brief A Tag osztály egy DTO osztály a `git tag` paranccsal vagy azzal egyenértékűvel létrehozott objektumok
     *        reprezentálására.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Tag : public Git::Reference
    {
    private:

        std::unique_ptr<git_commit, void(*)(git_commit*)>
            getReferenceHeadLibGitCommit(const std::string& tagName) const;
        
    public:
        /**
         * @brief Tag konstruktor
         * @param _repositoryKey
         *        A tag-et tartalmazó repository teljes elérési útvonala.
         * @param _name
         *        A tag neve a repositoryban
         *
         * Ha az objektum nem található a repositoryban, akkor std::invalid_argument váltódik ki.<BR>
         * Ha az objektum kezelése közben hiba történik, akkor Git::LibGitException dobódhat.
         */
        Tag(const std::string& _repositoryKey, const std::string& _name);
    };
}

#endif //TAG_H
