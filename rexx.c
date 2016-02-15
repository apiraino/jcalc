/*
 * rexx.c
 *
 *  Created on: Aug 12, 2013
 *      Author: jman
 */

#include "includes/common.h"
#include "includes/zunecommon.h"

#ifndef JLOG
#define JLOG
#include "includes/log.h"
#endif

#include "app.h"
#include "rexx.h"
#include "jcalc.h"

/**
    Open The AREXX library and a port
**/
exit_t openRexxLibPort()
{
    exit_t ret = EXIT_FAILURE;
    portName = (STRPTR)"JCALC";
    RexxSysBase = OpenLibrary((STRPTR)"rexxsyslib.library", 0);
    if (RexxSysBase != NULL)
    {
        port = CreatePort(portName, 1);
        if (port != NULL)
        {
            ret = EXIT_SUCCESS;
            jdebug("AREXX port %s created, waiting for messages", (STRPTR)portName);
        }
        else
        {
            CloseLibrary(RexxSysBase);
        }
    }

    return ret;
}

/**
    Close The AREXX port and library pointer
**/
exit_t closeRexxLibPort()
{
    if (port != NULL)
    {
        DeletePort(port);
    }

    CloseLibrary(RexxSysBase);

    return EXIT_SUCCESS;
}

struct cmdList rexxCommandList[] = {
    {"VERSION", rexxversion},
    {"QUIT", rexxquit},
    {NULL, NULL}
};

/* AREXX callbacks */

/**
    Manage an AREXX request to return the Port version
    @param msg AREXX message to be populated
    @param command (NOT USED) AREXX command
**/
    void rexxversion (struct RexxMsg *msg, struct ArexxCmdParams *command UNUSED)
{
    jdebug("[AREXX] Invoked rexxversion");
    msg->rm_Result1 = RC_OK;
    msg->rm_Result2 = (LONG) CreateArgstring( AREXXPORTVERSION, strlen((char *)AREXXPORTVERSION) );
    ReplyMsg((struct Message *)msg);
    // *res = StrDup(AREXXPORTVERSION);
}

/**
    Manage an AREXX request to close the application
    @param msg AREXX message to be populated
    @param command (NOT USED) AREXX command
**/
void rexxquit (struct RexxMsg *msg, struct ArexxCmdParams *command UNUSED)
{
    jdebug("[AREXX] Invoked rexxquit");

    msg->rm_Result1 = RC_OK;
    STRPTR ret = (STRPTR)"EXITING";
    msg->rm_Result2 = (LONG) CreateArgstring( ret, strlen((char *)ret) );
    ReplyMsg((struct Message *)msg);

    // Send the "close signal"
    DoMethod(app, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
}

/**
    Manage a command string (generic)
    @param msg AREXX message to be populated
    @param command AREXX command
**/
void rexxsendkeys (struct RexxMsg *msg, struct ArexxCmdParams *command)
{
    STRPTR ret = AllocVec(sizeof(STRPTR)*30, MEMF_CLEAR);
    // printf("[rexxsendkeys] Allocated %d at 0x%p\n", sizeof(STRPTR)*30, ret);

    if (EXIT_SUCCESS != parseCmdString(command->cmd, ret))
    {
        jerror("[AREXX] rexxsendkeys: problem?");

        msg->rm_Result1 = RC_WARN;
        msg->rm_Result2 = (LONG) CreateArgstring( ret, strlen((char *)ret) );
        ReplyMsg((struct Message *)msg);
    }
    else
    {
        jdebug("[AREXX] rexxsendkeys returning with ret:'%s'", ret);

        msg->rm_Result1 = RC_OK;
        msg->rm_Result2 = (LONG) CreateArgstring( ret, strlen((char *)ret) );
        ReplyMsg((struct Message *)msg);
    }

    FreeVec(ret); ret = NULL;
    return;
}


/**
    Command string validation
    @param _string command string to be evaluated
    @param _amp AREXX command to be populated
**/
exit_t ValidateArexxCmdParams(STRPTR _string, struct ArexxCmdParams *_amp)
{
    // jdebug("[AREXX] Entering VAMP");
    STRPTR str = StrDup(_string);
    // int str_len = strlen((char *)str);
    struct ArexxCmdParams *amp = _amp;

    amp->cmd = AllocVec(sizeof(STRPTR)*strlen((char *)str), MEMF_CLEAR);
    // printf("[rexxsendkeys] Allocated %d at 0x%p\n", sizeof(STRPTR)*strlen((char *)str), amp->cmd);
    if (amp->cmd == NULL)
    {
        jerror("[AREXX] Failed memory allocation for cmd!");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[AREXX] Failed memory allocation for cmd!\n");
    }
    amp->cmd = StrDup((STRPTR)str);

    // jdebug("[AREXX] VAMP Exiting");
    return EXIT_SUCCESS;
}

/**
    Print out the AREXX command (for debug purposes)
    @param _amp AREXX command to be printed
**/
exit_t PrintArexxCmdParams(struct ArexxCmdParams *amp)
{   
    // jdebug("[AREXX] Array len %d", sizeof(amp->params) / sizeof(STRPTR));
    if (amp)
    {
        // jdebug("[AREXX] PACP Alpha");
        if (amp->cmd)
            jdebug("[AREXX] PACP CMD:%s", amp->cmd);
    }

    // jdebug("[AREXX] PACP Echo");
    return EXIT_SUCCESS;
}

/**
    Free the AREXX command
    @param _amp AREXX command to be freed
**/
exit_t FreeArexxCmdParams(struct ArexxCmdParams *amp)
{
    if (amp)
    {
        if (amp->cmd)
        {
            FreeVec(amp->cmd); amp->cmd = NULL;
        }

        FreeVec(amp); amp = NULL;
    }

    return EXIT_SUCCESS;
}
