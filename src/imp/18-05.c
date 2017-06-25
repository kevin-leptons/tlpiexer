/*
SYNOPSIS

    Solution for exercise 18-05.

REQUIREMENT

    Implement a function that performs the equivalent of getcwd(). A useful
    tip for solving this problem is that you can find the name of the current
    working directory by using opendir() and readdir() to walk through each of
    the entries in the parent directory (..) to find and entry with the same
    i-node and device number as the current working directory 
    (i.e., respectively, the st_ino and st_dev fields in the stat structure
    returned by stat() and lsat()). Thus, it is possible to construct the
    directory path by walking up the directory tree (chdir(..)) one step at a
    time and performing such scans. The walk can be finished when the parent
    directory is the same as the current working directory (recall that /..
    is same as /). The caller should be left in the same directory in which it 
    started, regardless of whether your getcwd() function succeed or fails
    (open() plus fchdir() are handly for this purpose).

SOLUTION

    Same as hints of requirements.

USAGE

    $ ./dest/bin/18-05
    mnt/workspace/package/tlpiexer/
    
AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

int main(int argc, char **argv)
{
    // variables
    char cwd[PATH_MAX];
    char cwdtmp[PATH_MAX];
    char cdir_path[PATH_MAX];
    char udir_path[PATH_MAX];
    DIR *udir;
    struct dirent *udir_ent;
    struct stat cdir_stat, udir_stat;

    // init
    strcpy(cdir_path, ".");
    strcpy(cwd, "");

    // walk though up directory
    for(;;) {

        // status of current dir
        if (stat(cdir_path, &cdir_stat) == -1)
            exit_err("stat");
        
        // assign current dif by up dir
        // open up dir
        strcat(cdir_path, "/..");
        udir = opendir(cdir_path);
        if (udir == NULL)
            exit_err("opendir");
        
        // find currrent dir
        // store information in udir_ent
        errno = 0;
        while ((udir_ent = readdir(udir)) != NULL) {

            // skip for dir link
            if (strcmp(udir_ent->d_name, ".") == 0
                    || strcmp(udir_ent->d_name, "..") == 0)
                continue;

            // stat of sub file
            strcpy(udir_path, cdir_path);
            strcat(udir_path, "/");
            strcat(udir_path, udir_ent->d_name);
            if (stat(udir_path, &udir_stat) == -1)
                exit_err("stat");

            // verify
            if (udir_stat.st_dev == cdir_stat.st_dev && 
                    udir_stat.st_ino == cdir_stat.st_ino)
                break;
        }
        if (errno != 0)
            exit_err("readdir");

        // close up dir
        if (closedir(udir) == -1)
            exit_err("closedir");

        // break condition
        // can not find same sub dir in '/' exclude '..'
        if (udir_ent == NULL)
            break;
        
        // save current dir name
        strcpy(cwdtmp, udir_ent->d_name);
        strcat(cwdtmp, "/");
        strcat(cwdtmp, cwd);
        strcpy(cwd, cwdtmp);
    }

    // show current dir path
    printf("%s\n", cwd);
    
    // success
    return EXIT_SUCCESS;
}
