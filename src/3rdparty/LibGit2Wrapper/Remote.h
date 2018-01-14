#ifndef REMOTE_H
#define REMOTE_H

#include "RepositoryDependant.h"

#include <string>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief A Remote osztály egy DTO osztály a lokális repositoryhoz társított távoli repository lekérdezésére.
     *
     * @note Jelenleg lokális repositorynként legfeljebb egy darab remote támogatott.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Remote : public Git::RepositoryDependant
    {
    private:
        std::string name;
        std::string url;
        
    public:
        /**
         * @brief Remote konstruktor
         * @param _repositoryKey
         *        A remote objektumot tartalmazó repository teljes elérési útvonala.
         *
         * @param _name
         *        A remote neve a repository számára (milyen néven szerepel a refs/remotes/ mappában).
         *
         * Amennyiben a Remote nem eleme az adott repositorynak, akkor kivétel váltódik ki.
         */
        Remote(const std::string& _repositoryKey, const std::string& _name);

        /**
         * @brief getName Visszaadja a remote nevét. Létrehozás után a név nem frissül.
         * @return Remote neve a lokális repositoryban.
         */
        const std::string& getName() const { return name; }

        /**
         * @brief getUrl Az URL, amin keresztül a lokális repository kommunikál a remote-tal.
         * @return URL.
         */
        const std::string& getUrl() const { return url; }
        
    };
}

#endif //REMOTE_H
