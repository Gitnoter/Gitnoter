#ifndef LIB_GIT_USER_H
#define LIB_GIT_USER_H

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A LibGitInitializer osztály felel a LibGit2 környezet automatikus elindításáért és leállításáért.
     *        Minden osztály, amelyiknek szüksége lehet a fent említett környezetre, leszármazottja kell legyen.
     *
     * @note A LibGit2 környezet egyéb módon történő indítása/leállítása a könyvtárat inkonzisztens állapotba hozhatja
     *      és valószínűleg kivételhez vezet.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT LibGitInitializer
    {
    private:
        static int numOfLibGitInitializers;
    
        void init() const;
    
    protected:
        LibGitInitializer();
        LibGitInitializer(const LibGitInitializer& toCopy);
        LibGitInitializer& operator=(const LibGitInitializer& toAssign);

        /**
         * @brief checkLibGitError A LibGit2 függvények által visszaadott hibakódot ellenőrzi, szükség esetén
         *        LibGitException hibát dobva.
         * @param errorCode
         *        A LibGit2 függvény által visszaadott hibakód.
         *
         * @note Nem minden LibGit2 függvény ad vissza hibakódot. Minden esetben ellenőrizzük ezt a referenciában vagy
         *       a kódban.
         */
        void checkLibGitError(int errorCode) const;

    public:
        ~LibGitInitializer();
    };
}

#endif //LIB_GIT_USER_H
