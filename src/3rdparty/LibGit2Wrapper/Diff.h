#ifndef DIFF_H
#define DIFF_H

#include "RepositoryDependant.h"
#include "Commit.h"

#include <vector>
#include <memory>
#include <string>

#include <git2.h>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A Diff osztály a `git diff` parancs különböző verzióinak megfelelő szolgáltatásokat nyújt.
     *
     * Jelenleg elérhető funkcionalitás: `git diff HEAD`, `git diff <commit>` és `git diff <commit> <commit>`
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Diff : public Git::RepositoryDependant
    {
    public:

        enum class ModificationType {ADDED, DELETED, UNKNOWN};
        using NameChange = std::pair<std::string, std::string>;
        /**
         * Az egyik verzióban nem létező sorokat -1 értékkel jelöljük.
         */
        using LineNumberChange = std::pair<int, int>;
        using Line = std::string;
        using Hunk = std::vector<std::tuple<Diff::ModificationType, LineNumberChange, Line>>;

    private:
        std::string commitSha;
        std::string newCommitSha;

        bool compareRepositories(const Commit& commit1, const Commit& commit2);

        using FileCallbackType = int(*)(const git_diff_delta*, float, void*);
        using HunkCallbackType = int(*)(const git_diff_delta*, const git_diff_hunk*, void*);
        using LineCallbackType = int(*)( const git_diff_delta*
                                       , const git_diff_hunk*
                                       , const git_diff_line*
                                       , void*
                                       );

        //int(*getGetFilesCallback() const)(const git_diff_delta*, float, void*);
        FileCallbackType getGetFilesCallback() const;
        HunkCallbackType getHunkCallback() const;
        LineCallbackType getLineCallback() const;

        void runDiffCallbacks( FileCallbackType fileCallback
                             , HunkCallbackType hunkCallback
                             , LineCallbackType lineCallback
                             , void* payload
                             , std::vector<std::string> fileNameConstraints = {}
                             ) const;

        std::unique_ptr<git_diff, void(*)(git_diff*)> getLibGitDiff( const git_commit* toDiff
                                                                   , const std::vector<std::string>& fileNames = {}
                                                                   ) const;

        std::unique_ptr<git_diff, void(*)(git_diff*)> getLibGitDiff( const git_commit* oldCommit
                                                                   , const git_commit* newCommit
                                                                   , const std::vector<std::string>& fileNames = {}
                                                                   ) const;


    public:

        /**
         * @brief Diff konstruktor a `git diff HEAD` parancs futtatásához.
         * @param _repositoryKey
         *        A vizsgálandó repository teljes elérési útvonala.
         *
         * @note `git diff HEAD`: a checkoutolt commitot kívánjuk összehasonlítani a work directory és az index együttes
         *       tartalmával.
         */
        Diff(const std::string& _repositoryKey);

        /**
         * @brief Diff konstruktor a `git diff <commit>` parancs futtatásához.
         * @param commit
         *        A work directoryban és indexben megtalálható elemekkel összehasonlítandó commit SHA azonosítója.
         *
         * @note `git diff <commit>`: a commitot kívánjuk összehasonlítani a work directory és az index együttes
         *       tartalmával.
         */
        Diff(const Commit& commit);

        /**
         * @brief Diff konstruktor a `git diff <oldCommit> <newCommit>` parancs futtatásához.
         * @param oldCommit
         *        Az összehasonlítás "régebbi" oldalán álló Git::Commit.
         * @param newCommit
         *        Az összehasonlítás "újabb" oldalán álló Git::Commit. A két commitnak azonos repositoryból kell
         *        származniuk különben std::invalid_argument kivétel dobódik.
         */
        Diff(const Commit& oldCommit, const Commit& newCommit);

        /**
         * @brief getAllDifferentFiles Visszaadja az összes olyan fájl relatív elérési útvonalát, amelyet a diff
         *        művelet különbözőnek jelölt meg.
         * @return Relatív útvonal párok (régi név - új név) a work directoryhoz képest.
         *
         * @note Minden lekérdezés esetén újra fut a diff művelet, ezért a művelet számításigényes.
         */
        std::vector<Diff::NameChange> getAllDifferentFiles();


        /**
         * @brief getAllHunksForFile Visszaadja egy fájlra vonatkozóan az összes filetöredéket, amelyben a diff művelet
         *        változást érzékelt.
         * @param fileName
         *        Relatív útvonal a work directoryhoz képest.
         * @return Az összes különböző hunk, a változást leíró információkkal együtt. A sorok a fájltartalomtól függően
         *         tartalmazhatnak sorvég karaktereket is.
         *
         *
         * @note Minden lekérdezés esetén újra fut a diff művelet, ezért a művelet számításigényes.
         * @note Az eredeti `git diff` művelethez hasonlóan fájlvégi újsor karakter hiányát egy önálló diff sorral
         *       jelzi a függvény.
         */
        std::vector<Hunk> getAllHunksForFile(const std::string& fileName);
    };
}

#endif // DIFF_H
