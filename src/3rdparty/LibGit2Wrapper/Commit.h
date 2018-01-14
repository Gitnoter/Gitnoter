#ifndef COMMIT_H
#define COMMIT_H

#include "RepositoryDependant.h"

#include <string>
#include <ctime>
#include <vector>

#include <git2.h>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A Commit osztály egy DTO osztály a `git commit` paranccsal vagy
     *        azzal egyenértékűvel létrehozott objektumok reprezentálására.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Commit : public Git::RepositoryDependant
    {
    private:
        struct Signature
        {
            std::string name;
            std::string email;
            std::time_t when;
            
            Signature(std::string _name="", std::string _email="", std::time_t _when=0) : name(_name)
                                                                                        , email(_email)
                                                                                        , when(_when)
            {}
        };

        std::string sha;
        std::string message;
        Signature author;
        Signature committer;
        std::time_t commitDate;
        std::vector<std::string> parentShas;
        
        Commit::Signature getAuthor(const git_commit* commit) const;
        Commit::Signature getCommitter(const git_commit* commit) const;

        std::vector<std::string> getParentShas(const git_commit* commit) const;
        std::unique_ptr< git_signature
                       , void(*)(git_signature*)
                       > getAuthorLibGitSignature(const git_commit* commit) const;
        std::unique_ptr< git_signature
                       , void(*)(git_signature*)
                       > getCommitterLibGitSignature(const git_commit* commit) const;
            
    public:
        /**
         * @brief Commit konstruktor
         * @param _repositoryKey
         *        Teljes elérési út a commitot birtokló repositoryig.
         * @param _sha
         *        A betöltendő commit teljes SHA kódja.
         *
         * Amennyiben a commit nem található a Git repositoryban invalid_argument exception váltódik ki. <BR>
         * Ha a belső reprezentáció olvasása közben hiba történik LibGitException dobódhat.
         */
        Commit(const std::string& _repositoryKey, const std::string& _sha);

        /**
         * @brief getSha Visszaadja a Commit SHA kódját.
         * @return Teljes SHA kód.
         */
        const std::string& getSha() const { return sha; }

        /**
         * @brief getMessage Visszaadja a commit üzenetet.
         * @return Commit üzenet.
         *
         * @note Az utolsó karakter mindig egy új sor karakter.
         */
        const std::string& getMessage() const { return message; }

        /**
         * @brief getAuthorName Visszaadja a commit szerzőjének nevét.
         * @return Commit szerzőjének neve.
         */
        const std::string& getAuthorName() const { return author.name; }

        /**
         * @brief getAuthorEmail Visszaadja a commit szerzőjének e-mail címét.
         * @return Commit szerzőjének e-mail címe.
         */
        const std::string& getAuthorEmail() const { return author.email; }

        /**
         * @brief getCommitterName Visszaadja annak a nevét, aki a commitot hozzáadta a repositoryhoz.
         * @return Commit hozzáadójának neve.
         */
        const std::string& getCommitterName() const { return committer.name; }

        /**
         * @brief getCommitterEmail Visszaadja annak az e-mail címét, aki a commitot hozzáadta a repositoryhoz.
         * @return Commit hozzáadójának e-mail címe.
         */
        const std::string& getCommitterEmail() const { return committer.email; }

        /**
         * @brief getCreationTime Visszaadja a commit utolsó módosításának időpontját.
         * @return Commit utolsó módosításának időpontja.
         */
        const std::time_t& getCreationTime() const { return commitDate; }
    };
}

#endif //COMMIT_H
