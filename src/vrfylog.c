#include "includes/common.h"

#ifndef JLOG
#define JLOG
#include "includes/log.c"
#endif

int test(void)
{
    JLEVEL i;
    jinit(J_DEBUG, J_USE_STDERR, NULL);
    for (i = J_ALWAYS; i <= J_EVERYTHING; i++)
    {
        jprintf(i, "logging a message with level %d", i);
    }

    jshutdown();

    return 0;
}

int main(VOID)
{
    STRPTR src = (STRPTR)"the quick brown fox that jumped over the zombie dog";
    DOUBLE dst = .42;
    BYTE op = 5;
    logfile = (CONST_STRPTR)"RAM:jlog.log";

    // test();

    if (EXIT_SUCCESS != jinit(J_DEBUG, J_USE_LOGFILE, logfile))
    {
        fail((CONST_STRPTR)"Err", 101, (CONST_STRPTR)"jinit failed, aborting");
    }

    jconfig("[CONFIG] Config error ...");
    jerror("[ERROR] Error message ...");
    jwarning("[WARNING] Warning you about %s and %f or %d", src, dst, op);
    jconnect("[CONNECT] Connection error");
    jinfo("[INFO] Info message");
    jverbose("[VERBOSE] Verbose output");
    jdebug("[DEBUG] Debug too");

    jshutdown();

    printf("end.\n");

    return EXIT_SUCCESS;
}
