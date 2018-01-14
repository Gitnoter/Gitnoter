#ifndef LIB_GIT_EXCEPTION_H
#define LIB_GIT_EXCEPTION_H

#include "Exception.h"

#include <git2.h>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A LibGitException kivételosztály annak jelzésére szolgál, hogy a LibGit2 könyvtár jelzett hibát.
     *        A hibát ilyenkor az eredeti hibaüzenettel és hibakóddal dobjuk tovább. Gyakorlatilag bármely függvény
     *        hívásakor felmerülhet.
     *
     * @note A hibakódok és a hibaosztályok az <út a LibGit2 könyvtárhoz>/include/git2/errors.h headerben találhatóak
     *       meg.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT LibGitException : public Git::Exception
    {
    private:
        int errorClass;
        int errorCode;
        
    public:
        /**
         * @brief LibGitException konstruktor. Akkor használjuk, ha egy művelet során hibakódot kaptunk, de a
         *        a LibGit2 könyvtár mégsem állította be a hibaállapotát. (Valószínűleg a felhasználó szakította meg a
         *        műveletet.)
         * @param message
         *        A hiba üzenete
         * @param code
         *        A runtime hibakód. Különbözik a hibaosztálytól.
         */
        LibGitException(std::string message, int code) : Git::Exception(std::string(message))
                                                                       , errorClass(GITERR_NONE)
                                                                       , errorCode(code)
        {
        }

        /**
         * @brief LibGitException konstruktor.
         * @param error
         *        LibGit2 hiba, ami alapján kivételt generálunk.
         * @param code
         *        A runtime hibakód. Különbözik a hibaosztálytól.
         */
        LibGitException(const git_error& error, int code) : Git::Exception(std::string(error.message))
                                                          , errorClass(error.klass)
                                                          , errorCode(code)
        {
        }
        
        /**
         * @brief getErrorClass Hibaosztály lekérdezése.
         * @return Hibaosztály.
         */
        int getErrorClass() const noexcept { return errorClass; }

        /**
         * @brief getErrorCode Hibakód lekérdezése.
         * @return Hibakód.
         */
        int getErrorCode() const noexcept { return errorCode; }
    };
}

#endif //LIB_GIT_EXCEPTION_H
