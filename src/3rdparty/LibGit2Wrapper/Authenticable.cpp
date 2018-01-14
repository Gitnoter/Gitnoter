#include "Authenticable.h"

using Authenticable = Git::Authenticable;

git_cred_acquire_cb Authenticable::getCredentialsCallbackWrapper(AuthenticationCallback getUserCredentialsCallback
                                                                /* = nullptr */
                                                                ) const
{
    if(nullptr == getUserCredentialsCallback)
    {
        return git_cred_userpass;
    }
    else
    {
        return [](git_cred** credentials
               , const char* url
               , const char *username_from_url
               , unsigned int allowed_types
               , void* payload
               ) -> int
               {
                   (void)url;
                   (void)username_from_url;
                   (void)allowed_types;


                   AuthenticationCallback& applicationCallback =
                       *static_cast<AuthenticationCallback*>(payload);

                   Authenticable::Credentials applicationCredentials = applicationCallback();

                   if(applicationCredentials.isEmpty())
                   {
                       return GIT_EUSER;
                   }

                   std::string username = applicationCredentials.getUsername();
                   std::string password = applicationCredentials.getPassword();

                   return git_cred_userpass_plaintext_new(credentials
                                                         , username.c_str()
                                                         , password.c_str()
                                                         );
               };
    }
}
