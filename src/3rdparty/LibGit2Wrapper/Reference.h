#ifndef REFERENCE_H
#define REFERENCE_H

#include "RepositoryDependant.h"

#include <string>
#include <memory>

namespace Git
{
    class Commit;
}

namespace Git
{
    /**
     * @brief A Reference osztály a Branch és a Tag DTO osztályok őse és csak azokon keresztül példányosítható.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Reference : public Git::RepositoryDependant
    {
    private:
        std::string name;

    protected:
        /**
         * @brief Reference konstruktor
         * @param _repositoryKey A repository elérési útvonala.
         * @param _name
         *        A referencia rövid neve (NEM tartalmazza a refs/heads/..., refs/remotes/... vagy refs/tags/... részt).
         */
        Reference(const std::string& _repositoryKey, const std::string& _name);

        virtual std::unique_ptr<git_commit, void(*)(git_commit*)>
            getReferenceHeadLibGitCommit(const std::string& fullReferenceName) const;
            
    public:        

        /**
         * @brief getName Visszaadja a referencia rövid nevét (amivel inicializáltuk).
         * @return Rövid név (NEM tartalmazza a refs/heads/..., refs/remotes/... vagy refs/tags/.. részt)
         */
        const std::string& getName() const { return name; }

        /**
         * @brief getHead Visszaadja a Commitot, amire a referencia aktuálisan mutat.
         *
         * Nem tárol állapotokat, mindig lekérdezi Commitot. <BR>
         * Kiválthat LibGitException-t.
         *
         * @return
         */
        std::unique_ptr<Git::Commit> getHead() const;
    };
}

#endif //REFERENCE_H
