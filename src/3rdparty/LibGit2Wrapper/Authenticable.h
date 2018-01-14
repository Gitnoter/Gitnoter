#ifndef Authenticable_H
#define Authenticable_H

#include <git2.h>

#include "libgit2wrapper_global.h"

#include <functional>
#include <string>

namespace Git
{
    /**
     * @brief A Authenticable osztály minden olyan osztály őse, amely autentikációs műveleteket végezhet.
     *        Önmagában nem példányosítható.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Authenticable
    {
    public:
        /**
         * @brief A Authenticable::Credentials osztály egy (jellemzően callback által visszaadott) DTO objektum, amely
         *        a hálózati autentikációhoz szükséges adatokat tartalmazza.
         *
         * Amennyiben az objektum üres (`isEmpty`, avagy default konstruktorral inicializált) az jelzés arra, hogy a
         * felhasználó megszakította az autentikációs műveletet.
         */
        class Credentials
        {
        private:
            std::string username;
            std::string password;
            bool empty;

        public:
            Credentials() : username(""), password(""), empty(true) {}
            Credentials(const std::string& _username, const std::string& _password) : username(_username)
                                                                                    , password(_password)
                                                                                    , empty(false)
            {}

            bool isEmpty() const { return empty; }
            const std::string& getUsername() const { return username; }
            const std::string& getPassword() const { return password; }
        };


        using AuthenticationCallback = std::function<Git::Authenticable::Credentials()>;

    protected:
        Authenticable() = default;

        /**
         * @brief getCredentialsCallbackWrapper Visszaad egy LibGit2 autentikációs művelet során használható callbacket.
         * @param getUserCredentialsCallback Felhasználó által szolgáltatott callback, melyet az autentikáció során
         *        a könyvtár meghív a jelszó és a felhasználónév bekérésére.
         * @return Amennyiben paraméterként egy callbacket adunk meg, akkor ezt csomagolja be, egyébként egy olyat ad
         *         vissza, amely autentikációs lépést nem végez, de jól formált.
         *
         * @note Az autentikáció megszakítható, amennyiben a paraméterként megadott callback üres Credential objektumot
         *       ad vissza.
         * @note A callback csak akkor fog lefutni, ha az autentikáció ténylegesen szükséges.
         */
        git_cred_acquire_cb getCredentialsCallbackWrapper(AuthenticationCallback getUserCredentialsCallback
                                                         = nullptr
                                                         ) const;
    };
}

#endif // Authenticable_H
