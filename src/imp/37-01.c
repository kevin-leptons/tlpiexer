/*
SYNOPSIS

    Solution for exercise 37-01.

REQUIRE

    Write a program (similar to logger(1)) that uses syslog(3) to write 
    arbitrary messages to the system log file. As well as accepting a single 
    command-line argument containing the message to be logged, the program
    permit an option to specify the level of message.

SOLUTION

    - verify options
    - verify arguments
    - open system log by openlog()
    - write message and log level by syslog()
    - close system log 

USAGE

    $ ./dest/bin/37-01 "LOG_USER to system"
    $ ./dest/bin/37-01 -l CRIT "LOG_USER with LOG_CRIT"
    $ sudo grep -rni LOG_USER /var/log/user.log
    9141:Jun 21 17:25:06 dev ./dest/bin/37-01: LOG_USER to system
    9258:Jun 21 17:45:34 dev ./dest/bin/37-01: LOG_USER with LOG_CRIT

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : convert log level string to number
// arg      : 
//  - str: log level string, it is macro of log level of syslog(3)
// ret      : 
//  - log level number on success
//  - -1 on fails
int loglevel(char *str);

int  main(int argc, char *argv[])
{
    // variables
    int opt;
    int level;

    // init
    level = LOG_NOTICE;

    // parse options
    while ((opt = getopt(argc, argv, "hl:")) != -1) {
        switch (opt) {
            case 'l':
                if ((level = loglevel(optarg)) == -1) {
                    fprintf(stderr, "invalid log level\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }
    // verify arguments
    if (argc < 2 || argc > 4) {
        showhelp();
        return EXIT_FAILURE;
    }

    // write message to system log file
    openlog(argv[0], 0, LOG_USER); 
    syslog(level, "%s", argv[optind]); 
    closelog();

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "36-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s message\n", exename);
    printf("\t%s -l level message\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("message: string to write to system log file\n\n");

    printf("OPTIONS:\n\n");
    printf("\t-l: log level, it must be string below\n");
    printf("\t    EMERG, ALERT, CRIT, ERR, WARNING, NOTICE, INFO, DEBUG\n");
    printf("\t    default is NOTICE\n\n");
}

int loglevel(char *str)
{
    if (strcmp(str, "EMERG") == 0)
        return LOG_EMERG;
    else if (strcmp(str, "ALERT") == 0)
        return LOG_ALERT;
    else if (strcmp(str, "CRIT") == 0)
        return LOG_CRIT;
    else if (strcmp(str, "ERR") == 0)
        return LOG_ERR;
    else if (strcmp(str, "WARNING") == 0)
        return LOG_WARNING;
    else if (strcmp(str, "NOTICE") == 0)
        return LOG_NOTICE;
    else if (strcmp(str, "INFO") == 0)
        return LOG_INFO;
    else if (strcmp(str, "DEBUG") == 0)
        return LOG_DEBUG;
    else 
        return RET_FAILURE;
}
