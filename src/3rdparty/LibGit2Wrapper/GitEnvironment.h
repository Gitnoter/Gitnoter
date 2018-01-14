#ifndef GIT_ENVIRONMENT_H
#define GIT_ENVIRONMENT_H

#include "LibGitInitializer.h"
#include "Authenticable.h"

#include "Remote.h"

#include <memory>
#include <string>

#include "libgit2wrapper_global.h"


namespace Git
{
    /**
     * @brief A GitEnvironment osztály azon parancsok elérését szolgáltatja, melyek nem köthetőek egy adott
     *        Git repositoryhoz.
     *
     * @note A későbbiekben a global config kezelése is ebben az osztályban lesz megoldva.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT GitEnvironment : public Git::LibGitInitializer, protected Authenticable
    {
    private:
        
    public:
        GitEnvironment() = default;
    
        /**
         * @brief init Egy üres repositoryt inicializál. Azonos a `git init` paranccsal.
         * @param path
         *        Teljes elérési útvonal a Git repositoryként inicializálandó mappához.
         *
         * @note A `bare` repositoryk támogatása jelenleg nem megoldott.
         */
        void init(const std::string& path);

        /**
         * @brief clone Egy távoli repositoryt lemásolva inicializál egy lokális repositoryt. Azonos a `git clone`
         *        paranccsal.
         * @param pathToRepo
         *        Teljes elérési útvonal az Git repoitoryként inicializálandó lokális mappához.
         * @param url
         *        A másolandó távoli repository URL-je.
         * @param getUserCredentialsCallback
         *        Git::Authenticable::Credentials objektumot visszaadó callback függvény, amelyet a művelet a
         *        felhasznáó azonosításához használ. Ha a művelet nem kötött autentikációhoz, akkor a callback nem
         *        hívódik meg.<BR>
         *        Amennyiben nem adunk meg callbacket (default), akkor csak autentikációhoz nem kötött remote
         *        repositoryval tud kommunikálni.<BR>
         *        Amennyiben a callback üres Credentials objektumot ad vissza (`isEmpty` avagy az objektumot a default
         *        konstruktorral hoztuk létre), akkor a művelet megszakad és nullptr-t ad vissza. Ezzel a módszerrel
         *        lehet kezelni, ha a például egy dialógust a felhasználó `cancel` művelettel zár be.
         *
         * @note A C++11 előtti konvenciótól eltérően a callback nem fogad a felhasználó által meghatározott payloadot,
         *       azonban ez kiváltható lambda capture segítségével, ami aztán beágyazható egy std::function objektumba.
         */
        void clone( const std::string& pathToRepo
                  , const std::string& url
                  , AuthenticationCallback getUserCredentialsCallback = nullptr
                  );
        /**
         * @brief isRepository Visszaadja, hogy egy adott elérési útvonalon repository található-e.
         * @param path
         *        Teljes elérési útvonal a repositoryhoz.
         * @return Repository-e?
         */
        bool isRepository(const std::string& path) const;
        
    };
}

#endif //GIT_ENVIRONMENT_H
