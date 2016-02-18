/*
 * main.c
 *
 *  Created on: Aug 1, 2012
 *      Author: jman
 *
 *  Copyright © 2012-2013, Antonio Piraino
 *  Distributed under the terms of the AROS Public License 1.1
 *  http://aros.sourceforge.net/license.html
 *
 *  Desc: a simple calculator
 *  Language: English
 *
 */

#include "includes/common.h"

/* Logging */

#ifndef JLOG
    #define JLOG
    #include "includes/log.c"
#endif

#include "includes/zunecommon.h"

#define ENABLE_RT 0
#include <aros/rt.h>

#define ENABLE_MEMDBG 0
#include "includes/mem_debug.h"

#include "app.h"
#include "jcalc.h"
#include "about.h"
#include "rexx.h"

#include <aros/debug.h>

/**
    Set comma according to locale settings
**/
static void getLocale(void)
{
    struct Locale *loc;

    comma = (STRPTR)".";

    if ((loc = OpenLocale(0)))
    {
        comma = (STRPTR)loc->loc_DecimalPoint;
        CloseLocale(loc);
    }
}

/**
    Add a node to the input commands list
    @param list Destination list
    @param val the new line
**/
exit_t addNodeToList(struct inputList *list, STRPTR val)
{
    // jdebug("[addNodeToList] List starts @ 0x%p (-> 0x%p)", list, list->next);
    struct inputList *l = list;

    if (l->line == NULL)
    {
        l->line = StrDup(val);
        return EXIT_SUCCESS;
    }

    while (NULL != l->next)
    {
        // jdebug("[addNodeToList] List item: (0x%p) = %s -> 0x%p", l, l->line, l->next);
        l = l->next;
    }
    // jdebug("[addNodeToList] New element (%s) position: 0x%p", val, l);

    struct inputList *nuovo = AllocVec(sizeof(struct inputList), MEMF_CLEAR);
    // printf("[addNodeToList] Allocated %d at 0x%p\n", sizeof(struct inputList), nuovo);
    nuovo->line = StrDup(val);
    nuovo->next    = NULL;
    l->next = nuovo;

    // jdebug("[addNodeToList] List now starts @ 0x%p (-> 0x%p)", l, l->next);

    return EXIT_SUCCESS;
}

/**
    Print the whole input commands file (for debug purposes)
    @param list Command list received
**/
exit_t printList(struct inputList *list)
{
    struct inputList *l = list;
    while (NULL != l)
    {
        jdebug("[printList] List item: (0x%p) = %s -> 0x%p", l, l->line, l->next);
        l = l->next;
    }
    return EXIT_SUCCESS;
}

/**
    Parse the input file, populate a command list
    @param fp Input file pointer
    @param inputFile Command list to be populated
**/
exit_t parseInputFile(APTR *fp, struct inputList *inputFile)
{
    int buf_len    = 127+1;
    char buf[buf_len]; buf[0] = '\0';
    int i = 0;
    while(NULL != FGets(fp, (STRPTR)buf, buf_len))
    {
        // skip comments
        if (strchr((char *)buf, ';'))
        {
            i++;
            continue;
        }

        // skip empty non printable chars
        if (strlen((char *)buf) == 1 && buf[0] < 32)
        {
            i++;
            continue;
        }

        char *c;
        // likely the last char is a LINEFEED (10)
        if (NULL != (c = strchr((char *)buf, 10)))
            *c = '\0';
        else
            buf[buf_len-1] = '\0';

        addNodeToList(inputFile, (STRPTR)buf);

        i++;
    }

    return EXIT_SUCCESS;
}

/**
    Command string parser, returns the clean command
    @param line Input command string
    @param ret Output command
**/
exit_t parseCmdString (STRPTR line, STRPTR ret)
{
    if (line)
    {
        char *numbers   = "0123456789P"; // P = Pi
        char *others    = "+-/*%=.,IYSQ";

        int i = 0, num = 0;

        // ignore comments and non printable
        if (';' == line[0] || line[0] < 32)
            return EXIT_SUCCESS;

        jdebug("[parseCmdString] line is '%s'", line);
        while(line[i])
        {
            if (line[i] == ' ' || line[i] < 32)
            {
                i++;
                continue;
            }

            // jdebug("[parseCmdString] currchar is (%d) '%c'", line[i], line[i]);
            if (NULL != strchr(numbers, line[i]))
            {
                if ('P' == line[i])
                {
                    DoMethod(jcalc_win, MUIM_addConstant, 'P');
                    break;
                }

                num = line[i]-'0';
                // jdebug("[parseCmdString] iterating numbers:'%d'", num);
                DoMethod(jcalc_win, MUIM_addNumber, num);
            }
            else if (NULL != strchr(others, line[i]))
            {
                switch(line[i])
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
                        jdebug("[parseCmdString] sending ret as 0x%p='%s'", ret, ret);
                        DoMethod(jcalc_win, MUIM_getLastResult, ret);
                        jdebug("[parseCmdString] receiving ret as 0x%p='%s'", ret, ret);
                        break;
                }
            }
            else
            {
                jerror("[parseCmdString] Could not manage char:'%c'", line[i]);
                ret = StrDup((STRPTR)"NAN");
                return EXIT_FAILURE;
            }
            i++;
        }
    }
    jdebug("[parseCmdString] OK finished parsing, ret:'%s'", ret);
    return EXIT_SUCCESS;
}

/* Callbacks */

/**
    Open a window
    @param data Pointer to parameter(s)
**/
HOOKPROTONHNO(hookVal, void, APTR *data)
{
    Object *win = *data;
    SetAttrs(win, MUIA_Window_Open, TRUE, TAG_DONE);
}
MakeStaticHook(menuHook, hookVal);

HOOKPROTONHNO(rexxHookClick, void, APTR *data UNUSED)
{
    printf("Got rexx hook\n");
}
MakeStaticHook(rexxHook, rexxHookClick);

int main(int argc, char const *argv[])
{
    t = FindTask(NULL);
    t->tc_Node.ln_Name = "JCALC";
    struct RexxMsg *msg;

    // STRPTR tskName = (STRPTR)t->tc_Node.ln_Name;
    // MEMDBG_start_tracing(TRUE);

    // RT_Init();

    BOOL toggleSaveAsCsv = FALSE;
    STRPTR inputFileName = 0;

    /* Init logging */
    logfile = (CONST_STRPTR)"RAM:jcalc.log";
#if DEBUG
    if (EXIT_SUCCESS != jinit(J_DEBUG, J_USE_LOGFILE, logfile))
#else
    if (EXIT_SUCCESS != jinit(J_CONNECT, J_USE_STDOUT, logfile))
#endif
    {
        fail((CONST_STRPTR)"JCALC", 101, (CONST_STRPTR)"jinit failed, aborting");
    }
    else
        jinfo("logging successfully initialized");

#ifdef __amigaos4__

    MUIMasterBase = OpenLibrary("muimaster.library", 0);
    if (!MUIMasterBase)
    {
        jerror("Error opening muimaster.library\n");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"Failed opening muimaster.library!\n");
    };

    IMUIMaster = (struct MUIMasterIFace *) GetInterface(MUIMasterBase, "main", 1, NULL);
    if (!MUIMasterBase && !IMUIMaster)
    {
        jerror("Error opening muimaster library\n");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"Failed opening muimaster.library!\n");
    };

#endif

    /* Open AREXX port */
    if (EXIT_SUCCESS != openRexxLibPort())
    {
        jerror("Error opening muimaster rexxsyslib.library\n");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"Failed opening rexxsyslib.library!\n");
    }

    /* parse command line args (if any) */
    if (argc != 1)
    {
        int i;
        for (i = 0; i < argc; i++)
        {
            jdebug("parameter[%d]:%s", i, argv[i]);
            if (strncmp(argv[i], "--csv", strlen(argv[i])) == 0)
            {
                toggleSaveAsCsv = TRUE;
            }
            else if (strncmp(argv[i], "--input", strlen(argv[i])) == 0)
            {
                if (argv[i+1])
                {
                    int len = strlen(argv[i+1]);
                    inputFileName = AllocVec(sizeof(STRPTR)*len, MEMF_CLEAR);
                    // printf("[main] Allocated %d at 0x%p\n", sizeof(STRPTR)*len, inputFileName);
                    if (!inputFileName)
                    {
                        jerror("Error allocating memory for inputFileName\n");
                        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"Error allocating memory for inputFileName!\n");
                    }
                    inputFileName = StrDup((STRPTR)argv[i+1]);
                }
                else
                {
                    jerror("--input parameter expects a filename");
                }
            }
        }
    }
    jdebug(">>> Logging as %d", toggleSaveAsCsv);
    if (inputFileName)
        printf("Parsing input file: '%s'\n", inputFileName);

    // set comma character according to Locale
    getLocale();

    struct MUI_CustomClass *mcc = NULL, *aboutClass = NULL;

    if (!(mcc = initCalcClass()))
        jerror("Could not create Jcalc custom class.");

    // Create GUI
    app = ApplicationObject,
        MUIA_Application_RexxHook, &rexxHook, // 20130812 Not implemented yet in AROS

        MUIA_Application_Menustrip,
            MenuObject,
            MUIA_Family_Child, MenuObject,
            MUIA_Menuitem_Title, "Project",
              MUIA_Family_Child, menu_about = MenuObject, MUIA_Menuitem_Title,"About",MUIA_Menuitem_Shortcut,"A",End,
              MUIA_Family_Child, menu_quit = MenuObject, MUIA_Menuitem_Title,"Quit",MUIA_Menuitem_Shortcut,"Q",End,
            End,

            MUIA_Family_Child, MenuObject,
            MUIA_Menuitem_Title, "Settings",
              MUIA_Family_Child, menu_history = MenuObject,
                  MUIA_Menuitem_Title, "Enable history panel",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, FALSE,
                  MUIA_Menuitem_Shortcut, "H",
              End,
              MUIA_Family_Child, menu_csv = MenuObject,
                  MUIA_Menuitem_Title, "Save history as CSV",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, FALSE,
                  MUIA_Menuitem_Shortcut, "S",
                  End,
            End,

            MUIA_Family_Child, menu_mode = MenuObject,
            MUIA_Menuitem_Title, "Mode",
              MUIA_Family_Child, menu_basic = MenuObject,
                  MUIA_Menuitem_Title, "Basic",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, TRUE, /* default mode */
                MUIA_Menuitem_Exclude, 2,
                  End,
              MUIA_Family_Child, menu_prg = MenuObject,
                  MUIA_Menuitem_Title, "Scientific",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, FALSE,
                MUIA_Menuitem_Exclude, 1,
                  End,
               End,

            MUIA_Family_Child, menu_base = MenuObject,
            MUIA_Menu_Enabled, FALSE,
            MUIA_Menuitem_Title, "Base",
              MUIA_Family_Child, menu_bin = MenuObject,
                  MUIA_Menuitem_Title, "Binary",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, FALSE,
                MUIA_Menuitem_Exclude, 2+4+8,
                  MUIA_Menuitem_Shortcut, "B",
                  End,
              MUIA_Family_Child, menu_dec = MenuObject,
                  MUIA_Menuitem_Title, "Decimal",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, TRUE, /* default mode */
                MUIA_Menuitem_Exclude, 1+4+8,
                  MUIA_Menuitem_Shortcut, "D",
                  End,
              MUIA_Family_Child, menu_hex = MenuObject,
                  MUIA_Menuitem_Title, "Hexadecimal",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, FALSE,
                MUIA_Menuitem_Exclude, 1+2+8,
                MUIA_Menuitem_Shortcut, "H",
                End,
              MUIA_Family_Child, menu_oct = MenuObject,
                  MUIA_Menu_Title, "Octal",
                  MUIA_Menuitem_Checkit, TRUE,
                MUIA_Menuitem_Checked, FALSE,
                MUIA_Menuitem_Exclude, 1+2+4,
                MUIA_Menuitem_Shortcut, "O",
                End,
            End,

        End, /* End MUIA_Application_Menustrip */

        SubWindow, window = WindowObject,
        MUIA_Window_Title, "jCalc",
        MUIA_Application_Version, VERSTAG,
        // MUIA_Window_Height, MUIV_Window_Height_Visible(55),
        // MUIA_Window_Width, MUIV_Window_Width_Visible(30),

        WindowContents, GroupObject,
            MUIA_Window_ID, MAKE_ID('M','A','I','N'),
            // MUIA_Window_Height, MUIV_Window_Height_Visible(85), /* % of available window */
            // MUIA_Window_Height, APPHEIGHT,

            Child, GroupObject,

                Child, jcalc_win = NewObject(mcc->mcc_Class, NULL,
//                    MUIA_FrameTitle, "CustomClass group",
//                    GroupFrame,
                    MUIA_JCALC_toggleHistoryPanel, FALSE,
                    MUIA_JCALC_SaveAsCSV, toggleSaveAsCsv,
                    MUIA_JCALC_SetMode, DECBASE,
                    TAG_DONE
                ),

            End,

        End, /* Close Group */

        End, /* Close WindowObject */
        End;

    /* Create about window */
    if(!(aboutClass = initAboutClass()))
    {
        jerror("Could not create About MCC object");
        return EXIT_FAILURE;
    }

    Object *winAbout = NewObject(aboutClass->mcc_Class, NULL,
            MUIA_Window_Title, (IPTR) "About jCalc",
        TAG_DONE);

    if (winAbout)
    {
        DoMethod(app, OM_ADDMEMBER, winAbout);

        DoMethod(winAbout, MUIM_Notify,
             MUIA_About_Close, MUIV_EveryTime,
             winAbout,
             3,
             MUIM_Set,
             MUIA_Window_Open,
             MUIV_NotTriggerValue);
    }
    else
    {
        jerror("Could not create About Window object");
    }

    /* Menu event management */

    /*
        FIXME

        CHECK CAREFULLY WHAT I'M DOING HERE!
        Shall I notify the attribute MUIA_Menuitem_Trigger OR MUIA_Menuitem_Checked???
        If I check MUIA_Menuitem_Checked strange things happen: like the same event
        triggered twice!

    */

    DoMethod(menu_about, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        app, 3,
        MUIM_CallHook, &menuHook, winAbout);

    DoMethod(menu_history, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 1,
        MUIM_toggleHistoryPanel);

    DoMethod(menu_csv, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 1,
        MUIM_setSaveAs);

    DoMethod(menu_bin, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 2,
        MUIM_setBase, BINBASE);

    DoMethod(menu_dec, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 2,
        MUIM_setBase, DECBASE);

    DoMethod(menu_hex, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 2,
        MUIM_setBase, HEXBASE);

    DoMethod(menu_oct, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 2,
        MUIM_setBase, OCTBASE);

    DoMethod(menu_basic, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 2,
        MUIM_setMode, BASICMODE);

    DoMethod(menu_prg, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        jcalc_win, 2,
        MUIM_setMode, PRGMODE);

    DoMethod(menu_quit, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        app, 2,
        MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        app, 2,
        MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    if (app != NULL)
    {
        ULONG sigs, evt = 0, port_mask = ( 1L << port->mp_SigBit );
        int msg_answered = 0;
        struct ArexxCmdParams *command = NULL;

        // manage input file
        int killMe = 0; /* <-- once finished parsing the file, exit gracefully */
        if (inputFileName)
        {
            APTR *fp;
            struct inputList *inputFile = AllocVec(sizeof(struct inputList), MEMF_CLEAR);
            // printf("[main] Allocated %d at 0x%p\n", sizeof(struct inputList), inputFile);

            if (NULL == (fp = Open(inputFileName, MODE_OLDFILE)))
            {
                jerror("[INPUTFILE] Could not open file '%s'", inputFileName);
            }
            else
            {
                // jdebug("[INPUTFILE] Will parse file '%s'", inputFileName);
                parseInputFile(fp, inputFile);
                // jdebug("[INPUTFILE] Finished parsing file");
                Close(fp);
            }

            // printList(inputFile);

            int i = 0;
            killMe = 1;
            struct inputList *l = inputFile;
            STRPTR ret = AllocVec(sizeof(STRPTR)*30, MEMF_CLEAR);
            // printf("[main] Allocated %d at 0x%p\n", sizeof(STRPTR)*30, ret);
            while(l != NULL)
            {
                // jdebug("[%d] parseCmdString Ready to iterate?", i);
                jdebug("[%d] [INPUTFILE] Will send to parseCmdString line:'%s'", i, l->line);
                if (EXIT_SUCCESS != parseCmdString((STRPTR)l->line, ret))
                {
                    jerror("Error running parseCmdString on string:'%s'", l->line);
                }
                jdebug("[%d] [INPUTFILE] parseCmdString returned with (0x%p) '%s'", i, ret, ret);

                // if ret is inf o un errore, break
                if (0 == strcmp((char *)ret, "+inf"))
                {
                    jdebug("[%d] [INPUTFILE] Line %s provoked an error, bail out now!", i, l->line);
                    printf("ERROR: Line [%s] cannot be processed.\n", l->line);
                    printf("Exiting!\n");
                    break;
                }

                l = l->next;
                i++;
            }

            jdebug("[INPUTFILE] parseCmdString finished with (0x%p) '%s'", ret, ret);
            if (0 == strlen((char *)ret))
            {
                // jdebug("ret is empty (%s)\n", ret);
                strncpy((char *)ret, "NaN", 3+1);
            }
            // else
            // {
            //     jdebug("ret is not empty (%s)\n", ret);
            // }

            printf("RIS: %s\n", ret);
            if (ret) { FreeVec(ret); ret = NULL; }
        }

        // running jcalc in interactive (ZUNE) mode
        if (!killMe)
        {
            set(window, MUIA_Window_Open, TRUE);
        }

        /* Opens the main window and start main application loop */
        if (XGET(window, MUIA_Window_Open))
        {
            // 20130812 Had to revert to the older loop implementation
            // to make room to the AREXX commands management
            // DoMethod(app, MUIM_Application_Execute);

            while ((evt = (LONG)DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)) != MUIV_Application_ReturnID_Quit)
            {
                if (sigs)
                {
                    /* AREXX msg management */
                    while( ( msg = (struct RexxMsg *) GetMsg( port ) ) != NULL )
                    {
                        jdebug("[AREXX] Received a message: '%s'", (STRPTR)msg->rm_Args[0] );

                        if (!IsRexxMsg(msg))
                        {
                            jerror("[AREXX] Apparently not an AREXX message...");
                            msg->rm_Result1 = RC_ERROR;
                            ReplyMsg((struct Message *)msg);
                            continue;
                        }

                        if (!CheckRexxMsg(msg))
                        {
                            jerror("[AREXX] Message is not from an AREXX interpreter");
                            msg->rm_Result1 = RC_ERROR;
                            ReplyMsg( (struct Message *) msg );
                            continue;
                        }

                        command = AllocVec(
                            sizeof(struct ArexxCmdParams) +     // memory for struct itself
                            sizeof(STRPTR),                        // memory for "command"
                            MEMF_CLEAR);
                        // printf("[main] Allocated %d at 0x%p\n", sizeof(struct ArexxCmdParams) + sizeof(STRPTR), command);

                        ValidateArexxCmdParams((STRPTR)msg->rm_Args[0], command);
                        // PrintArexxCmdParams(command);

                        // manage reply to Msg.
                        // ReplyMsg() is performed by the callback
                        int j;
                        msg_answered = 0;
                        for (j = 0; rexxCommandList[j].name; ++j)
                        {
                            if (0 == (strcasecmp(rexxCommandList[j].name, (char *)command->cmd)))
                            {
                                jdebug("[AREXX] cmd requested: '%s'", (STRPTR)command->cmd);
                                rexxCommandList[j].f(msg, command);
                                msg_answered = 1;
                                break;
                            }
                        }

                        if (msg_answered == 0)
                        {
                            // managing commands extra list (parsing char-by-char)
                            jdebug("[AREXX] don't know cmd: '%s'", (STRPTR)command->cmd);
                            rexxsendkeys(msg, command);

                            // msg->rm_Result1     = RC_WARN;
                            // msg->rm_Result2     = (LONG) CreateArgstring( REPLY_STR, strlen((char *)REPLY_STR) );
                            // ReplyMsg((struct Message *)msg);
                        }

                        if (command) { FreeArexxCmdParams(command); }
                    }
                    sigs = Wait(sigs | port_mask | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D);
                }
            }
        }

        // Clean up everything
        MUI_DisposeObject(app);
        MUI_DeleteCustomClass(mcc);
        closeRexxLibPort();
    }

    jshutdown();

    if (inputFileName) { FreeVec(inputFileName); inputFileName = NULL; }

    // RT_Exit();

    // MEMDBG_stop_tracing();
    // MEMDBG_report_tracing();

    return EXIT_SUCCESS;
}
