#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/fsuid.h>
#include <pwd.h>
#include <shadow.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

int signin(const char *usrname, const char *pass)
{
    struct passwd *pwd;
    struct spwd *spwd;
    char *crypt_pwd;
    char *p;

    // retrieve user information such as uid, gid, password
    pwd = getpwnam(usrname);
    if (pwd == NULL)
        return FN_ER;

    // retrieve user password if it protected by shadow
    spwd = getspnam(usrname);
    if (spwd != NULL)
        pwd->pw_passwd = spwd->sp_pwdp;

    // verify password
    crypt_pwd = crypt(pass, pwd->pw_passwd);
    for (p = (char*) pass; *p != 0; p++)
        *p = 0;
    if (crypt_pwd == NULL)
        return FN_ER;
    if (strcmp(crypt_pwd, pwd->pw_passwd) != 0)
        return FN_ER;

    // change user, group, file id
    if (setresuid(pwd->pw_uid, pwd->pw_uid, pwd->pw_uid) == FN_ER)
        return FN_ER;

    // BUGS: can't detect error here
    setfsuid(pwd->pw_uid);
    setfsgid(pwd->pw_gid);

    return FN_OK;
}
