#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <exception>

#include "libgit2wrapper_global.h"

namespace Git
{
    /**
     * @brief Az Exception kivételosztály a LibGitException ősosztálya. Csak azon keresztül példányosítható.
     */
    class LIBGIT2WRAPPERSHARED_EXPORT Exception : public std::exception
    {
    private:
        std::string message;

    protected:
        /**
         * @brief Exception konstruktor
         * @param _message A kivétel által tartalmazott üzenet.
         */
        Exception(const std::string& _message = "") : message(_message)
        {
        }
        
    public:

        /**
         * @brief what Lekérdező metódus kivételosztályokhoz.
         * @return A kivétel üzenete.
         */
        virtual const char* what() const noexcept
        {
            return ( message.c_str() );
        }
    };
}

#endif //EXCEPTION_H
