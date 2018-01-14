#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "RepositoryDependant.h"
#include "Authenticable.h"

#include "Commit.h"
#include "Branch.h"
#include "Tag.h"
#include "Remote.h"

#include <git2.h>

#include <memory>
#include <vector>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A Repository osztály felelős azon lekérdező és módosító műveletekért, melyek egy adott Git repositoryra
     *        vonatkoznak.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Repository : public Git::RepositoryDependant, protected Authenticable
    {
        
    private:
        enum class State { NORMAL
                         , MERGE
                         , REBASE
                         , UNKNOWN
                         };

        bool isFileInRepository(const std::string& relativePath) const;
        bool doesGitIgnoreApply(const std::string& relativePath);
        State getState() const;
        bool isStaged(const std::string& relativePath) const;
        bool isReferenceOwnedByRepo(const std::string& fullReferenceName) const;
        std::string getConfigEntry(const std::string& key) const;
        std::size_t getRemoteCount() const;

        std::string messagePrettify(std::string message) const;
        void forceableCheckout(const Git::Commit& toCheckout, bool force);
        void forceableCheckout(const Git::Branch& toCheckout, bool force);

        void startRebase(std::unique_ptr<git_annotated_commit, void(*)(git_annotated_commit*)> headToRebaseTo);

    public:
        /**
         * @brief Repository konstruktor.
         * @param _pathToRepo Teljes elérési útvonal a git repositoryhoz.
         */
        Repository(const std::string& _pathToRepo);

        /**
         * @brief isHeadDetached Visszaadja a HEAD referencia állapotát.
         * @return A HEAD referencia `detached HEAD` állapotban van-e?
         *
         * @note Kiválthat LibGitExceptiont.
         */
        bool isHeadDetached() const;
        /**
         * @brief getPath Visszaadja a teljes elérési útvonalat a repositoryhoz. Ekvivalens a getRepositoryPath()
         *        örökölt metódussal.
         * @return Teljes elérési útvonal a repositoryhoz.
         */
        const std::string& getPath() const { return pathToRepository; }
        /**
         * @brief hasConflicts Visszaadja, hogy conflict állapotban van-e a repository.
         * @return Conflict állapotban van-e a repository?
         *
         * @note Conflict állapotú repository merge, rebase etc. esetén fordulhat elő. Ilyenkor abortáljunk, ha
         *       lehetséges vagy oldjuk fel a conflictokat a `getConflictedFiles()` által visszaadott fájlokban, majd
         *       folytassuk a műveletet.
         *
         * Belső hiba esetén kiválthat LibGitExceptiont.
         */
        bool hasConflicts() const;
        /**
         * @brief isInNormalState Visszaadja, hogy a repository nincs-e semmilyen különleges (merge, rebase,
         *        cherry-pick etc.) állapotban.
         * @return Normál állapotú-e a repository?
         *
         * Belső hiba esetén kiválthat LibGitExceptiont.
         */
        bool isInNormalState() const { return State::NORMAL == getState(); }
        /**
         * @brief isInNormalState Visszaadja, hogy a repositoryban éppen egy merge folyik-e. Akkor is igazra
         *        értékelődik ki, ha a merge conflictokat már feloldottuk, de még nem commitoltunk.
         * @return Merge állapotban van-e a repository?
         *
         * @note A `git reset --merge` és a `git reset --hard` parancsok atására a repository állapota visszavált
         *       normálisra. Egyelőre csak a `git reset --hard` paranccsal van egyenértékű szolgáltatás a könyvtárban.
         *
         * Belső hiba esetén kiválthat LibGitExceptiont.
         */
        bool isInMergeState() const { return State::MERGE == getState(); }
        /**
         * @brief isInRebaseState Visszaadja, hogy a repositoryban éppen egy rebase folyik-e. Akkor is igazra
         *        értékelődik ki, ha az összes rebase patch-et alkalmaztuk, illetve a conflictokat feloldottuk már, de
         *        még nem commitoltunk.
         * @return Rebase állapotban van-e a repository?
         *
         * @note Rebase esetében nem a `commit()` függvény, hanem a `commitCurrentRebasePatch()` függvény segítségével
         *       írjuk az aktuális patch változtatásait az addatbázisba és az `abortRebase()` segítségével állítjuk le.
         *
         * Belső hiba esetén kiválthat LibGitExceptiont.
         */
        bool isInRebaseState() const { return State::REBASE == getState(); }
        /**
         * @brief commit Ekvivalens a `git commit -m <message>` paranccsal. Az index alapján egy commitot ad a
         *        repositoryhoz, szükség esetén frissítve a megfelelő referenciákat. <BR>
         *        A committer és a szerző egyaránt az alapértelmezett értékre lesz beállítva.
         * @param message A commit üzenete.
         *
         * Ha a repository állapota nem megfelelő (azaz nincs se merge se normál állapotban), akkor logic_error
         * váltódik ki.<BR>
         * Amennyiben belső hiba történik LibGitExeption váltódik ki.
         */
        void commit(const std::string& message);
        /**
         * @brief stage A megadott elérési útvonalú fájlokat hozzáadja az indexhez. Ekvivalens a `git add <file>`
         *        paranccsal.
         * @param relativePaths A work dir-hez viszonyított útvonala a fájloknak.
         *
         * Hiba esetén LiGitExceptiont válthat ki.
         */
        void stage(const std::vector<std::string>& relativePaths);
        /**
         * @brief stage A megadott elérési útvonalú fájlokat eltávolítja az indexből. Ekvivalens a `git reset <file>`
         *        paranccsal.
         * @param relativePaths A work directoryhoz viszonyított útvonala a fájloknak.
         *
         * Ha valamely átadott fájlra gitignore szabály vonatkozik vagy nem található a repositoryban, akkor
         * std::invalid_argument kivétel váltódik ki. <BR>
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        void unstage(const std::vector<std::string>& relativePaths);
        /**
         * @brief getStagedFiles Visszaadja a work directoryhoz viszonyított teljes útvonalát (fájlnévvel együtt) az
         *        indexben lévő fájloknak.
         * @return Indexben lévő fájlok listája.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::vector<std::string> getStagedFiles() const;
        /**
         * @brief getNotStagedButModifiedFiles Visszaadja a work directoryhoz viszonyított teljes útvonalát (fájlnévvel
         *        együtt) az indexben NEM lévő, azonban módosult fájloknak. A conflictokat nem tartalmazza.
         * @return Dirty fájlok listája.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::vector<std::string> getDirtyFiles() const;
        /**
         * @brief getConflictedFiles Visszaadja a work directoryhoz viszonyított teljes útvonalát (fájlnévvel együtt) a
         *        conflictoknak.
         * @return Conflictok listája.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::vector<std::string> getConflictedFiles() const;
        /**
         * @brief hardReset Ekvivalens a `git reset --hard` paranccsal. A repository állapotát - akár fájlok felülírása
         *        árán is - visszaállítja a HEAD-nek megfelelő állapotba.
         *
         * @note Merge és rebase műveletek abortálására is használható, mivel ezen állapotokat is grantáltan
         *       visszaállítja.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        void hardReset();
        /**
         * @brief abortRebase Egy folyamatban lévő rebase műveletet megszakít. Az esetlegesen véglegesített patch-eket
         *        ez nem vonja vissza.
         *
         * Ha a repository nincs rebase állapotban, akkor logic_error váltódik ki.<BR>
         * Belső hiba esetén LibGitException dobódik.
         */
        void abortRebase();
        /**
         * @brief tag Ekvivalens a `git tag <name>` paranccsal.
         * @param name A címke szövege.
         *
         * Belső hiba esetén kiválthat LibGitExceptiont.
         */
        void tag(const std::string& name);
        /**
         * @brief tag Ekvivalens a `git tag <commit> <name>` paranccsal.
         * @param name A címke szövege.
         * @param toTag A commit, amit fel akarunk címkézni.
         *
         * Belső hiba esetén kiválthat LibGitExceptiont.
         */
        void tag(const Git::Commit& toTag, const std::string& name);
        /**
         * @brief tag Ekvivalens a `git branch <name>` paranccsal.
         * @param name A branch neve.
         *
         * Egy új branch-et hoz létre a HEAD által mutatott objektumból leágazva.<BR>
         * Belső hiba esetén LibGitException dobódik.
         */
        void branch(const std::string& name);

        /**
         * @brief tag Ekvivalens a `git branch <name> <commit>` paranccsal.
         * @param toStartBranchFrom A commit, amin létre szeretnénk hozni az új branch-et.
         * @param name A branch neve.
         *
         * Egy új branch-et hoz létre a HEAD által mutatott objektumból leágazva.<BR>
         * Amennyiben a argumentum nem a adott repository része, akkor invalid_excepiton áltódik ki.<BR>
         * Belső hiba esetén LibGitException dobódik.
         */
        void branch(const Commit& toStartBranchFrom, const std::string& name);
        
        /**
         * @brief push Ekvivalens a `git push` paranccsal, azzal a megkötéssel, hogy csak a config fájl push
         *        refspec-je alapján frissíti a remote-ot, annak hiánya esetén nem veszi figyelembe a push.default
         *        értéket.
         * @param getUserCredentialsCallback
         *        Git::Authenticable::Credentials objektumot visszaadó callback függvény, amelyet a művelet a
         *        felhasználó azonosításához használ. Ha a művelet nem kötött autentikációhoz, akkor a callback nem
         *        hívódik meg.<BR>
         *        Amennyiben nem adunk meg callbacket (default), akkor csak autentikációhoz nem kötött remote
         *        repositoryval tud kommunikálni.<BR>
         *        Amennyiben a callback üres Credentials objektumot ad vissza (`isEmpty` avagy az objektumot a default
         *        konstruktorral hoztuk létre), akkor a művelet megszakad és nullptr-t ad vissza. Ezzel a módszerrel
         *        lehet kezelni, ha a például egy dialógust a felhasználó `cancel` művelettel zár be.
         *
         * A függvény logic_errort vált ki, ha a repository nem rendelkezik remote-tal.<BR>
         * Ha belső hiba történik, akkor LibGitException váltódik ki.
         *
         * @note A C++11 előtti konvenciótól eltérően a callback nem fogad a felhasználó által meghatározott payloadot,
         *       azonban ez kiváltható lambda capture segítségével, ami aztán beágyazható egy std::function objektumba.
         * @warning Felhasználóként jellemzően a push.default alapján push-olunk az upstream branch-re. Mivel ezt a
         *          függvény nem támogatja, ezért explicit módon meg kell adjunk egy push refspec-et a config fájlban.
         * @warning Ne felejtsük el, hogy a push refspec destination változója is egy lokális branch-et ír le
         *          (csak a remote oldalán), pl.: refs/heads/master:refs/heads/master
         */
        void push(AuthenticationCallback getUserCredentialsCallback = nullptr) const;
        /**
         * @brief pull Ekvivalens a `git pull` paranccsal.
         * @param getUserCredentialsCallback
         *        Git::Authenticable::Credentials objektumot visszaadó callback függvény, amelyet a művelet a
         *        felhasználó azonosításához használ. Ha a művelet nem kötött autentikációhoz, akkor a callback nem
         *        hívódik meg.<BR>
         *        Amennyiben nem adunk meg callbacket (default), akkor csak autentikációhoz nem kötött remote
         *        repositoryval tud kommunikálni.<BR>
         *        Amennyiben a callback üres Credentials objektumot ad vissza (`isEmpty` avagy az objektumot a default
         *        konstruktorral hoztuk létre), akkor a művelet megszakad és nullptr-t ad vissza. Ezzel a módszerrel
         *        lehet kezelni, ha a például egy dialógust a felhasználó `cancel` művelettel zár be.
         *
         * A függvény logic_errort vált ki, ha a repository állapota gátolja a műveletet, a repository nem rendelkezik
         * remote-tal vagy ha nincs upstream branch beállítva a HEAD által mutatott branch-hez.<BR>
         * Ha a HEAD detached állapotban van, akkor domain_error váltódik ki.<BR>
         * Ha belső hiba történik, akkor LibGitException dobódik.
         *
         * @note Detached HEAD vagy nemlétező upstream esetében használjuk a `fetch` függvényt.
         * @note A C++11 előtti konvenciótól eltérően a callback nem fogad a felhasználó által meghatározott payloadot,
         *       azonban ez kiváltható lambda capture segítségével, ami aztán beágyazható egy std::function objektumba.
         */
        void pull(AuthenticationCallback getUserCredentialsCallback = nullptr);
        /**
         * @brief fetch Ekvivalens a `git fetch` paranccsal.
         * @param A reflogba írandó üzenet.
         * @param getUserCredentialsCallback
         *        Git::Authenticable::Credentials objektumot visszaadó callback függvény, amelyet a művelet a
         *        felhasználó azonosításához használ. Ha a művelet nem kötött autentikációhoz, akkor a callback nem
         *        hívódik meg.<BR>
         *        Amennyiben nem adunk meg callbacket (default), akkor csak autentikációhoz nem kötött remote
         *        repositoryval tud kommunikálni.<BR>
         *        Amennyiben a callback üres Credentials objektumot ad vissza (`isEmpty` avagy az objektumot a default
         *        konstruktorral hoztuk létre), akkor a művelet megszakad és nullptr-t ad vissza. Ezzel a módszerrel
         *        lehet kezelni, ha a például egy dialógust a felhasználó `cancel` művelettel zár be.
         *
         * A függvény logic_errort vált ki, ha a repository állapota gátolja a műveletet vagy a repository nem
         * rendelkezik remote-tal.<BR>
         * Ha belső hiba történik, akkor LibGitException dobódik.
         *
         * @note A C++11 előtti konvenciótól eltérően a callback nem fogad a felhasználó által meghatározott payloadot,
         *       azonban ez kiváltható lambda capture segítségével, ami aztán beágyazható egy std::function objektumba.
         */
        void fetch(AuthenticationCallback getUserCredentialsCallback = nullptr, std::string reflog = "fetch");

        /**
         * @brief isRemote Visszaadja, hogy egy adott néven szerepel-e távoli repository a config fájlban.
         * @param name A név, amit ellenőrizni szeretnénk.
         */
        bool isRemote(const std::string& name) const;
        /**
         * @brief hasRemote Visszaadja, hogy be van-e állítva távoli repository.
         * @return Van-e távoli repository?
         *
         * Kiválthat LibGitExceptiont, ha belső hiba történik.
         *
         * @note Jelenleg legfeljebb egy távoli repository támogatott.
         */
        bool hasRemote() const;
        /**
         * @brief addRemote Hozzáad a lokális repository config fájljához egy remote-ot és a fetch refspec-et beállítja
         *        az alapértelmezett értékre.
         * @param url A remote URL elérési útvonala.
         * @param name A remote neve a lokális repository számára.
         *
         * Kiválthat LibGitExceptiont belső hiba esetén. <BR>
         * Amennyiben már van remote hozzárendelve a repositoryhoz, akkor domain_error váltódik ki.
         *
         * @note Jelenleg legfeljebb egy távoli repository támogatott.
         */
        void addRemote(const std::string& url, const std::string& name = "origin");
        /**
         * @brief getRemote Visszaadja a repositoryhoz rendelt remote reprezentációját.
         * @return A repositoryhoz rendelt remote reprezentációját vagy nullptr, ha nincs ilyen.
         *
         * Kiválthat LibGitExceptiont belső hiba esetén. <BR>
         * Amennyiben több remote is hozzá van rendelve a repositoryhoz domain_error váltódik ki.
         *
         * @note Jelenleg legfeljebb egy távoli repository támogatott.
         */
        std::unique_ptr<Git::Remote> getRemote() const;
        
        /**
         * @brief stash A work directory és az index tartalmát egy címkézett stash-be menti. Ekvivalens a
         *        `git stash save [<message>]` paranccsal.
         * @param message A stash címkéjének szövege.
         */
        void stash(const std::string& message = "");
        
        /**
         * @brief merge Ekvivalens a `git merge --no-commit -Srecursive -Xno-renames` paranccsal, azzal a két
         *        megszorítással, hogy a HEAD nincs detached állapotban és a merge-elendő objektum egy branch.
         * @param toMergeIntoHead
         *
         * A HEAD leszármazottjaként egy új commitot állít össze létre (de bejegyzést nem hoz létr), melyben a két
         * branch bármelyikén elérhető minden commit tartalma megtalálható (az esetleges conflictok feloldása után).<BR>
         * Ha a  repository állapota gátolja a műveletet vagy ha nem fastforward merge esetén fájlok vannak az indexben,
         * akkor logic_error váltódik ki.<BR>
         * Amennyiben az argumentum nem jelöl objektumot a repositoryban, akkor invalid_argument exception váltódik
         * ki.<BR>
         * Ha `detached HEAD` állapot gátolja a műveletet vagy bare rapository-n alkalmaznánk a műveletet,
         * akkor domain_error váltódik ki.<BR>
         * Egyéb hiba esetén LibGitException dobódik.
         */
        void merge(const Git::Branch& toMergeIntoHead); //one can only merge when a branch is checked out (not a commit)
        /**
         * @brief numOfRemainingRebasePatches Visszaadja a még nem alkalmazott rebase patch-ek számát.
         * @return Hátralévő rebase patch-ek száma.
         *
         * Ha a repository állapota gátolja a művelet elvégzését, conflictok találhatóak a repositoryban vagy nincs mit
         * commitolni (üres index esetén), akkor logic_error váltódik ki.<BR>
         * Ha az argumentum nem az adott repositoryból származik, vagy megyegyezik a HEAD által mutatottal, akkor
         * invalid_argument kivétel generálódik.<BR>
         * Ha a repository bare vagy a HEAD detached állapotban van, illetve ha kettőnél több ágat szeretnénk
         * merge-elni, akkor domain_error váltódik ki.<BR>
         * Egyéb belső hiba esetén LibGitException dobódik.
         *
         * @warning A hátralévő rebase patch-ek száma nem tartja számon, hogy az adott patch véglegesítve lett-e már.
         * @note Jelenleg LibGit2 korlátozások miatt nem lehetséges a Git applikációból indított rebase megnyitása.
         */
        std::size_t numOfRemainingRebasePatches() const;
        /**
         * @brief startRebase A repositoryt és a belső állapotot az elkövetkező rebase-nek megfelelő állapotba állítja.
         *        A `startRebase`, `applyNextRebasePatch` és `commitCurrentRebasePatch` függvénycsaláddal
         *        a `git rebase --force-rebase --preserve-merges --no-ff -Xno-renames <branch>` paranccsal azonos
         *        hatást érhetünk el (az adott függvényeknél említett megszorításoktól eltekintve).
         * @param toRebaseTo A branch, amire rebase-elni szeretnénk.
         *
         * A repository állapota megakadályozhatja a rebase indítását. Ebben az esetben logic_error váltódik ki.<BR>
         * Ha az argumentum egy másik repository eleme, akkor invalid_argument váltódik ki.<BR>
         * Egyéb esetben LiBGitException váltódhat ki.
         *
         * @note A függvény nem alkalmazza az első patch-et.
         * @note Jelenleg nem lehetséges commitra rebase-elni.
         * @note Jelenleg LibGit2 korlátozások miatt nem lehetséges a Git applikációból indított rebase lekérdezése
         *       (LibGItException kiváltódásához vezet).
         */
        void startRebase(const Git::Branch& toRebaseTo);
        /**
         * @brief applyNextRebasePatch Alkalmazza a következő rebase patch-et.
         *
         * A repository állapota megakadályozhatja a patch alkalmazását. Ebben az esetben logic_error váltódik ki.<BR>
         * Ha az argumentum nem az adott repositoryból származik, vagy megyegyezik a HEAD által jelenleg mutatottal,
         * akkor invalid_argument exception váltódik ki.<BR>
         * Amennyiben a HEAD detached állapotban van, akkor domain_error váltódik ki.<BR>
         * Egyéb belső hiba esetén LibGitException váltódik ki.
         *
         * @note Jelenleg LibGit2 korlátozások miatt nem lehetséges a Git applikációból indított rebase folytatása
         *       (LibGItException kiváltódásához vezet).
         */
        void applyNextRebasePatch();
        /**
         * @brief commitCurrentRebasePatch Véglegesíti (commitként hozzáadja a repositoryhoz) az utolsóként alkalmazott
         *        rebase patch-et, amennyiben minden conflict rendezésre került.
         *
         * Amennyiben a repository nem rebase állapotban van, akkor logic_error váltódik ki.<BR>
         * Ha vannak rendezetlen conflict fájlok, akkor LibGitException váltódik ki GIT_EUNMERGED hibakóddal. <BR>
         * Ha a jelenlegi patch már véglegesítve lett, akkor LibGitException váltódik ki GIT_EAPPLIED hibakóddal. <BR>
         * Egyéb belső hiba esetén LibGitException váltódik ki (a fentiektől különböző hibakóddal).
         *
         * @note Jelenleg LibGit2 korlátozások miatt nem lehetséges a Git applikációból indított rebase folytatása
         *       (LibGItException kiváltódásához vezet).
         */
        void commitCurrentRebasePatch();
        /**
         * @brief deleteTag Törli a meghatározott taget a repositoryból. Ekvivalens a `git tag -d` paranccsal.
         * @param toDelete A törölni szánt tag objektum reprezentációja.
         *
         * Kiválthat invalid_argument exceptiont - ha egy másik repository által birtokolt taget törölnénk -, illetve
         * LibGitExceptiont - belső hiba esetén.
         */
        void deleteTag(const Git::Tag& toDelete);
        /**
         * @brief deleteBranch Törli a meghatározott branch-et a repositoryból. Ekvivalens a `git branch -d` paranccsal.
         * @param toDelete A törölni szánt branch objektum reprezentációja.
         *
         * Amennyiben az argumentumnak nem a repository a tulajdonosa, vagy az remote tracking branch-et jelöl, akkor
         * invalid_argument váltódik ki.<BR>
         * Egyéb belső hiba esetén (ld. például a megjegyzésben) LibGitException dobódik.
         *
         * @note Nem törölhető azon branch, amelyre jelenleg a mutat a HEAD refererencia.
         */
        void deleteBranch(const Git::Branch& toDelete);
        /**
         * @brief checkout A work directoryt a szolgáltatott objektummal megegyező állapotba hozza, majd - amennyiben ez
         *        sikeres volt - a HEAD referenciát is ráállítja, ezzel `detached HEAD` állapotba hozva a
         *        HEAD referenciát.
         *        Megegyezik a `git checkout <commit sha>` paranccsal.
         * @param toCheckout A Git::Commit, ami alapján be szeretnék állítani a repositoryt.
         *
         * Kiválthat LibGitExceptiont.
         */
        void checkout(const Git::Commit& toCheckout);
        /**
         * @brief checkout A work directoryt a szolgáltatott objektummal megegyező állapotba hozza, majd - amennyiben ez
         *        sikeres volt - a HEAD referenciát is ráállítja.
         *        Megegyezik a `git checkout <branch név>` paranccsal.
         * @param toCheckout A Git::Branch, ami alapján be szeretnék állítani a repositoryt.
         *
         * Kiválthat LibGitExceptiont.
         */
        void checkout(const Git::Branch& toCheckout);
        /**
         * @brief getParentCommits Visszaadja a paraméterként kapott Git::Commit objektum szüleinek reprezentációját.
         * @param commit Az objektum, amelynek a szüleit akarjuk lekérdezni.
         * @return A vizsgált objektum szülei.
         *
         * Kiválthat LibGitExceptiont belső hiba esetén, invalid_argument kivételt, ha másik repositoryból kérdeznénk
         * le, vagy domain_errort, ha kettőnél több szülője van egy adott Git::Commitnak (ez jelenleg nem támogatott).
         */
        std::vector<std::unique_ptr<Git::Commit>> getParentCommits(const Git::Commit& commit) const;
        /**
         * @brief getTags Visszaadja a repositoryban jelenleg létező tagek listáját.
         * @return Lokálisan elérhető tagekkel feltöltött vektor.
         *
         * @note Ez rendszeres pull és push műveletek esetén is eltérhet a remote-ban jelenlévőktől.
         *
         * Ha a commit nem az adott repository által felügyelt objektum, akkor invalid_argument exception dobódik.
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::vector<std::unique_ptr<Git::Tag>> getTags() const;

        /**
         * @brief getBranches Visszaadja a repositoryban jelenleg létező (lokális vagy távoli repositoryt követő)
         *        branch-ek listáját.
         * @return Lokálisan és remote branch-ekkel feltöltött vektor.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::vector<std::unique_ptr<Git::Branch>> getBranches() const;

        /**
         * @brief isCheckedOut Visszaadja, hogy a HEAD referencia az adott objektumon áll-e.
         * @param toCheck Objektum, amire az ellenőrzést lefuttatjuk.
         * @return A HEAD az adott objektumra mutat-e?
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isCheckedOut(const Git::Commit& toCheck) const;
        /**
         * @brief isCheckedOut Visszaadja, hogy a HEAD referencia az adott objektumon áll-e.
         * @param toCheck Objektum, amire az ellenőrzést lefuttatjuk.
         * @return A HEAD az adott objektumra mutat-e?
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isCheckedOut(const Git::Branch& toCheck) const;
        /**
         * @brief isABranchCheckedOut Visszaadja, hogy a HEAD referencia egy branch-re mutat-e.
         * @return A HEAD egy branch-re mutat-e?
         *
         * @note Ez pontosan abban az esetben igaz, ha a HEAD nincs detached állapotban.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isABranchCheckedOut() const;
        /**
         * @brief getHeadBranch Visszaad egy unique_ptr-t, ami a HEAD referencia által mutatott lokális branch-et leíró
         *        Git::Branch objektumra mutat. Amennyiben nincs ilyen (azaz `detached HEAD` állapotban van a
         *        repository), akkor nullptr-t tartalmazó unique_ptr-t ad vissza.
         * @return A HEAD referencia által mutatott branch objektum vagy nullptr, ha nincs ilyen.
         *
         * @note Ha a mutatott branch egy remote lokálisan követett verziója (bár ez jelenleg nem történhet meg a Git
         *       egyik implementációjában sem), akkor domain_errort vált ki.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::unique_ptr<Git::Branch> getHeadBranch() const;

        /**
         * @brief getHead Visszaadja a HEAD által aktuálisan mutatott commitot. A metódus mindenképp egy valid
         *        Git::Commit objektumot ad vissza, akár`detached a HEAD állapota, akár nem.
         * @return A HEAD által mutatott commit objektum reprezentációját.
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        std::unique_ptr<Git::Commit> getHead() const;

        /**
         * @brief isOwnedByRepo Visszaadja, hogy egy létező objektum a lekérdezés időpontjában eleme-e az adott
         *        repositorynak.
         * @param toCheck Azon elem, amit ellenőrizni akarunk.
         * @return Tartalmazza-e a vizsgált elemet a repository?
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isOwnedByRepo(const Git::Commit& toCheck) const;
        /**
         * @brief isOwnedByRepo Visszaadja, hogy egy létező objektum a lekérdezés időpontjában eleme-e az adott
         *        repositorynak.
         * @param toCheck Azon elem, amit ellenőrizni akarunk.
         * @return Tartalmazza-e a vizsgált elemet a repository?
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isOwnedByRepo(const Git::Branch& toCheck) const;
        /**
         * @brief isOwnedByRepo Visszaadja, hogy egy létező objektum a lekérdezés időpontjában eleme-e az adott
         *        repositorynak.
         * @param toCheck Azon elem, amit ellenőrizni akarunk.
         * @return Tartalmazza-e a vizsgált elemet a repository?
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isOwnedByRepo(const Git::Tag& toCheck) const;
        /**
         * @brief isOwnedByRepo Visszaadja, hogy egy létező objektum a lekérdezés időpontjában eleme-e az adott
         *        repositorynak.
         * @param toCheck Azon elem, amit ellenőrizni akarunk.
         * @return Tartalmazza-e a vizsgált elemet a repository?
         *
         * Belső hiba esetén LiGitExceptiont válthat ki.
         */
        bool isOwnedByRepo(const Git::Remote& toCheck) const;
    };
}

#endif //REPOSITORY_H
