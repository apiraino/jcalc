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
void rexxversion (struct RexxMsg *msg, struct ArexxCmdParams *command UNUSED)
{
    jdebug("[AREXX] Invoked rexxversion");
    msg->rm_Result1 = RC_OK;
    msg->rm_Result2 = (LONG) CreateArgstring( AREXXPORTVERSION, strlen((char *)AREXXPORTVERSION) );
    ReplyMsg((struct Message *)msg);
    // *res = StrDup(AREXXPORTVERSION);
}

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

void rexxsendkeys (struct RexxMsg *msg, struct ArexxCmdParams *command)
{
    STRPTR ret = AllocVec(sizeof(STRPTR)*30, MEMF_CLEAR);

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









    if (command->cmd)
    {
        jdebug("[AREXX] rexxsendkeys cmd is '%s'", command->cmd);

        char *numbers   = "0123456789P"; // P = Pi
        char *others    = "+-/*%=.,IYSQ";

        int i = 0, num = 0;
        while(command->cmd[i])
        {
            if (command->cmd[i] == ' ')
            {
                i++;
                continue;
            }

            jdebug("[AREXX] rexxsendkeys currchar is '%c'", command->cmd[i]);
            if (NULL != strchr(numbers, command->cmd[i]))
            {
                if ('P' == command->cmd[i])
                {
                    DoMethod(jcalc_win, MUIM_addConstant, 'P');
                    // break;
                }

                num = command->cmd[i]-'0';
                jdebug("[AREXX] rexxsendkeys iterating numbers:'%d'", num);
                DoMethod(jcalc_win, MUIM_addNumber, num);
            }
            else if (NULL != strchr(others, command->cmd[i]))
            {
                switch(command->cmd[i])
                {
                    case '+':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_PLUS);
                        break;
                    case '-':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_MINUS);
                        break;
                    case '*':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_MULTIPLY);
                        break;
                    case '/':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_DIVIDE);
                        break;
                    case '%':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_PERCENT);
                        DoMethod(jcalc_win, MUIM_getLastResult, ret);
                        break;
                    case '.':
                        DoMethod(jcalc_win, MUIM_addDot);
                        break;
                    case 'I':
                        DoMethod(jcalc_win, MUIM_setOperatorImmediate, EVT_BTN_INVERSE);
                        DoMethod(jcalc_win, MUIM_getLastResult, ret);
                        break;
                    case 'Q':
                        DoMethod(jcalc_win, MUIM_setOperatorImmediate, EVT_BTN_SQUARE);
                        DoMethod(jcalc_win, MUIM_getLastResult, ret);
                        break;
                    case 'S':
                        DoMethod(jcalc_win, MUIM_setOperatorImmediate, EVT_BTN_SQRT);
                        DoMethod(jcalc_win, MUIM_getLastResult, ret);
                        break;
                    case 'Y':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_POWERY);
                        break;
                    case '=':
                        // Perform a calculation, clean the results
                        DoMethod(jcalc_win, MUIM_doCalc, TRUE);
                        DoMethod(jcalc_win, MUIM_getLastResult, ret);
                        break;
                }
            }
            else
            {
                jerror("[AREXX] Could not manage char:'%c'", command->cmd[i]);
                msg->rm_Result1 = RC_WARN;
                // memset(ret, 0, sizeof(STRPTR)*strlen((char *)ret));
                ret = StrDup((STRPTR)"Could not fully manage command");
                msg->rm_Result2 = (LONG) CreateArgstring( ret, strlen((char *)ret) );
                ReplyMsg((struct Message *)msg);
                return;
            }

            i++;
        }
    }
    jdebug("[AREXX] rexxsendkeys finished parsing (everything is OK)");
    msg->rm_Result1 = RC_OK;
    msg->rm_Result2 = (LONG) CreateArgstring( ret, strlen((char *)ret) );
    ReplyMsg((struct Message *)msg);
    FreeVec(ret); ret = NULL;
}

#if 0
void _rexxsendkeys (struct ArexxCmdParams *command, STRPTR *res)
{
    jdebug("[AREXX] Invoked rexxsendkeys");

    if (command->params)
    {
        int i = 0;
        while (command->params[i])
        {
            jdebug("[AREXX] param is '%s'", command->params[i]);

            char *numbers = "0123456789"; // TODO abcdefABCDEF";
            char *operators = "+-/*%=";
            char *comma = ".,";
            if (strspn((char *)command->params[i], comma) > 0)
            {
                // received a comma, either one is OK
                DoMethod(jcalc_win, MUIM_addDot);
            }
            else if (strspn((char *)command->params[i], numbers) > 0)
            {
                // received a number
                int num = 0;
                int len = 0;
                int base = 10; // TODO base BIN,HEX,OCT
                if (strspn((char *)command->params[i], numbers) > 1)
                {
                    // We have a number with multiple chars, ALLOWED.
                    // Will iterate through the string and shoot all the numbers to jcalc
                    len = strlen((char *)command->params[i]);
                    int j;
                    for (j = 0; j < len; ++j)
                    {
                        num = command->params[i][j]-'0';
                        jdebug("[AREXX] rexxsendkeys iterating numbers:'%d'", num);
                        DoMethod(jcalc_win, MUIM_addNumber, num);
                        // command->params[i]++;
                    }
                }
                else
                {
                    if (base == 10)
                    {
                        // num = strtod((char *)command->params[i], NULL);
                        num = command->params[i][0]-'0';
                    }
                    // TODO base BIN,HEX,OCT
                    // else
                    // {
                    //     unsigned int ui;
                    //     ui = strtoul((char *)command->params[i], NULL, base);
                    //     // jwarning("[doGetDisplayWrapper] testing against overflow: %d", i);
                    //     if ((int)ui == -1)
                    //         num = -1;
                    //     else
                    //         num = (DOUBLE)strtol((char *)command->params[i], NULL, base);
                    // }
                    jdebug("[AREXX] rexxsendkeys num is:'%d'", num);
                    DoMethod(jcalc_win, MUIM_addNumber, num);
                }
            }
            else if (strspn((char *)command->params[i], operators) > 0)
            {
                // received an operator
                char op;
                int len = 0;
                if (strspn((char *)command->params[i], operators) > 1)
                {
                    // We have an operator with multiple chars, NOT ALLOWED.
                    // Will pick the last one and discard the others
                    len = strlen((char *)command->params[i]);
                    // op = command->params[i][len-1];
                }
                else
                    len = 1;

                op = command->params[i][len-1];
                jdebug("[AREXX] rexxsendkeys op is:'%c'", op);

                switch(op)
                {
                    case '+':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_PLUS);
                        break;
                    case '-':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_MINUS);
                        break;
                    case '*':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_MULTIPLY);
                        break;
                    case '/':
                        DoMethod(jcalc_win, MUIM_setOperator, EVT_BTN_DIVIDE);
                        break;
                    case '=':
                        // Requested to perform a calculation
                        DoMethod(jcalc_win, MUIM_doCalc, TRUE);
                        // jdebug("[AREXX] rexxsendkeys sending ptr (0x%p) '%s'", res, *res);
                        DoMethod(jcalc_win, MUIM_getLastResult, *res);
                        // jdebug("[AREXX] rexxsendkeys received ptr (0x%p) is '%s'", res, *res);
                        break;
                }
            }
            else
            {
                jwarning("[AREXX] Did not manage param:'%s'", command->params[i]);
            }

            i++;
        }
    }

    // msg->rm_Result1 = RC_OK;
}
#endif

exit_t ValidateArexxCmdParams(STRPTR _string, struct ArexxCmdParams *_amp)
{
    // jdebug("[AREXX] Entering VAMP");
    STRPTR str = StrDup(_string);
    // int str_len = strlen((char *)str);
    struct ArexxCmdParams *amp = _amp;

    amp->cmd = AllocVec(sizeof(STRPTR)*strlen((char *)str), MEMF_CLEAR);
    if (amp->cmd == NULL)
    {
        jerror("[AREXX] Failed memory allocation for cmd!");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[AREXX] Failed memory allocation for cmd!\n");
    }
    amp->cmd = StrDup((STRPTR)str);


    /*
        --> Param parsing not needed anymore

    char buf[str_len]; buf[0] = '\0';
    strncpy(buf, (char *)str, str_len);
    buf[str_len] = '\0';

    char *tok = strtok(buf, " ");
    int j = 0, num_param = 0;
    while(tok != NULL)
    {
        if (j == 0) // "CMD"
        {
            amp->cmd = AllocVec(sizeof(STRPTR)*strlen(tok), MEMF_CLEAR);
            if (amp->cmd == NULL)
            {
                jerror("[AREXX] Failed memory allocation for cmd!");
                fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[AREXX] Failed memory allocation for cmd!\n");
            }
            amp->cmd = StrDup((STRPTR)tok);
        }
        else // params
        {
            // jdebug("[AREXX] VAMP Adding param%d:'%s'", num_param, tok);
            amp->params[num_param] = StrDup((STRPTR)tok);
            num_param++;
        }

        // array terminator
        amp->params[num_param] = NULL;

        tok = strtok(NULL," ");
        j++;
        // jdebug("[AREXX] VAMP End iteration");
    }

    */

    // jdebug("[AREXX] VAMP Exiting");
    return EXIT_SUCCESS;
}

exit_t PrintArexxCmdParams(struct ArexxCmdParams *amp)
{   
    // jdebug("[AREXX] Array len %d", sizeof(amp->params) / sizeof(STRPTR));
    if (amp)
    {
        // jdebug("[AREXX] PACP Alpha");
        if (amp->cmd)
            jdebug("[AREXX] PACP CMD:%s", amp->cmd);

        // if (amp->params)
        // {
        //     // jdebug("[AREXX] PACP Will print params");
        //     int i = 0;
        //     while (amp->params[i])
        //     {
        //         // jdebug("[AREXX] PACP Charlie%d", i);
        //         jdebug("[AREXX] PACP PARAM%d:%s", i, amp->params[i]);
        //         i++;
        //     }
        // }
        // else
        // {
        //     jdebug("[AREXX] PACP NOPARAMS");
        // }
        // jdebug("[AREXX] PACP Delta");
    }

    // jdebug("[AREXX] PACP Echo");
    return EXIT_SUCCESS;
}

exit_t FreeArexxCmdParams(struct ArexxCmdParams *amp)
{
    if (amp)
    {
        if (amp->cmd)
        {
            FreeVec(amp->cmd); amp->cmd = NULL;
        }

        // if (amp->params)
        // {
        //     int i = 0;
        //     while (amp->params[i])
        //     {
        //         FreeVec(amp->params[i]); amp->params[i] = NULL;
        //         i++;
        //     }
        // }

        FreeVec(amp); amp = NULL;
    }

    return EXIT_SUCCESS;
}
