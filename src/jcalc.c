/*
 * jcalc.c
 *
 *  Created on: Mar 11, 2013
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

#ifndef JLOG
#define JLOG
#include "includes/log.h"
#endif

// #define jdebug(...) {}
// #define jwarning(...) {}
// #define jwarning(x) printf(x)
// #define jwarning(...) printf("--\n");
// #define jerror(x) printf(x)

#include "includes/zunecommon.h"
#include "jcalc.h"
#include "about.h"
#include "display.h"

#include <aros/debug.h>

char char_operators[11] = {
    '+',
    '-',
    '/',
    '*',
    '=',
    '.',
    '%',
    '.',
    '.',
    '.',
    '.',
};

/**
    Remove the comma (if that's the last char)
    @param str the number
**/
exit_t removeComma(STRPTR *str)
{
    int len = strlen((char *)*str) + 1;
    char buf[len]; buf[0] = '\0';
    snprintf(buf, len, "%s", *str);
    buf[len-1] = '\0';

    if (buf[len-2] == '.' || buf[len-2] == ',')
        buf[len-2] = '\0';
    *str = (STRPTR)StrDup((STRPTR)buf);

    return EXIT_SUCCESS;
}

/**
    Remove leading zeroes in a number
    @param str the number
**/
exit_t removeLeadingZeroes(STRPTR *str)
{
    int len = strlen((char *)*str);
    char *b = NULL;
    b = strdup((char *)*str);
    D(bug("[removeLeadingZeroes] Starting with %s, [%d]\n", b, strlen(b)));

    int i;
    int zeroes = 0;
    for (i = 0; i < len; i++)
    {
        if (b[i] != '0' || b[i] == '\0')
            break;

        zeroes++;
    }
    D(bug("[removeLeadingZeroes] Will remove %d zero(es) from %s\n", zeroes, b));

    char buf[len-1]; buf[0] = '\0';
    int j = 0;
    for(i = zeroes, j = 0; i < len; i++, j++)
        buf[j] = b[i];

    buf[j] = '\0';
    b  = strdup(buf);
    *str = (STRPTR)strdup(b);

    return EXIT_SUCCESS;
}

/**
    Set MAX_DECIMALS decimals for number
    @param str the number
**/
exit_t max_decimals(STRPTR *str)
{
    return EXIT_SUCCESS;

    /*
        TODO
        (must not be higher than MAX_DISPLAY_FIGURES anyway)
    */

    int len = strlen((char *)*str);
    char *b = NULL;
    b = strdup((char *)*str);
    int pos;
    for (pos = 0; pos < len; pos++)
        if (b[pos] == '.')
            break;

    char buf[len]; buf[0] = '\0';
    int i;
    for (i = 0; i < pos + MAX_DECIMALS; i++)
    {
        buf[i] = b[i];
    }

    buf[i+1] = '\0';

    *str = (STRPTR)strdup(buf);
    return EXIT_SUCCESS;
}

/**
    Remove trailing zeroes from a number
    @param str the number
**/
exit_t removeTrailingZeroes(STRPTR *str)
{
    // FIXME removeTrailingZeroes should only remove zeroes! Check the code

    // D(bug("[removeTrailingZeroes] TAG MEMORY check: '%s' 0x%p\n", *str, *str));
    jdebug("[removeTrailingZeroes] starts with: '%s' (comma is %s)", *str, comma);

    /* Is there really a comma? */
    /* If there's no comma, there's no zeroes to trim */
    char *p;
    if (NULL == (p = strstr((char *)*str, ",")))
        if (NULL == (p = strstr((char *)*str, ".")))
            return EXIT_SUCCESS;

    int len = strlen((char *)*str);
    char *b = NULL;
    b = strdup((char *)*str);
    // char b[len+1]; b[0] = '\0';
    // snprintf(b, len, "%s", *str);
    // jdebug("[removeTrailingZeroes] XXX b: '%s' (0x%p)", b, b);

    int i;
    int zeroes = 0;
    for (i = len; i > 0; i--)
    {
        if (b[i] == '\0')
            continue;

        // Stop counting at the char that is either a
        // comma or not a 0
        if ( b[i] == '.' || b[i] == ',' || b[i] != '0')
            break;

        zeroes++;
    }

    char buf[len+1]; buf[0] = '\0';
    // jdebug("[removeTrailingZeroes] buf is %d long", len+1);
    for(i = 0; i < (len-zeroes); i++)
    {
        // if (b[i] == '.')
        //     break;
        buf[i] = b[i];
    }

    // removing the comma is not your responsability anymore.
    // It will be performed in specific parts of the code
    // and please for the love of [gG]od, fix this mess...
    buf[i] = '\0';
    // jdebug("[removeTrailingZeroes] new buffer is '%s'", buf);

    snprintf((char *)*str, len+1, "%s", buf);
    // jdebug("[removeTrailingZeroes] after buf: '%s', *str:'%s'", buf, *str);
    // jdebug("[removeTrailingZeroes] *str: %s (0x%p) (removed %d zeroes)", *str, *str, zeroes);

    return EXIT_SUCCESS;
}

/**
    Convert back comma (if any) according to locale
    @param str the number
**/
exit_t convertComma(STRPTR *str)
{
    // int len = strlen((char *)*str);
    // char *b = NULL;
    // b = strdup((char *)*str);
    jdebug("[convertComma] Starting with %s (0x%p)", *str, *str);

    // 1 - check if a translation of the comma is required (by locale settings)
        // 2 - check if any decimal is present
            // 3 - do the translation '.' => ','
    char *p;
    char buffer[strlen((char *)str)+1];
    buffer[0] = '\0';

    // (1)
    if (0 == strcmp(",", (char *)comma))
    {
        // (2)
        if (NULL != (p = strstr((char *)*str, (char *)".")))
        {
            // (3)
            jdebug("[convertComma] Locale comma is ',': must convert back the string");
            jdebug("[convertComma] before tmpstr=%s", *str);

            strncpy(buffer, (char *)*str, p-(char *)*str);
            buffer[p-(char *)*str] = '\0';
            sprintf(buffer+(p-(char *)*str), "%s%s", comma, p+strlen((char *)"."));
            *str    = StrDup((STRPTR)buffer);
            jdebug("After conversion: %s", *str);
        }
        else
        {
            jdebug("[convertComma] There is no comma in the string");
        }
    }
    else
    {
        jdebug("[convertComma] Comma is not ',': no translation is needed");
    }

    return EXIT_SUCCESS;
}

/**
    Convert BASE of the display
    @param str the number
    @param old_base the old base
    @param new_base the new base
**/
exit_t convertDisplay(STRPTR *str, BYTE old_base, BYTE *new_base)
{
    /*
        This function is needed to turn back what I have in calcStack, according
        to the base.
        Ex. 124 becomes 7c if base=16
    */

    int buf    = 100;
    // char tmpstr[buf];
    // tmpstr[0] = '\0';
    long unsigned lu;

    jdebug("[convertDisplay] Put value from %s", *str);

    switch(*new_base)
    {
        case 2: // converting to a string representing the binary
        {
            lu    = strtol((char *)*str, NULL, old_base);
            jdebug("[convertDisplay] %s now is %lu", *str, lu);
            int precision = 32;
            int n    = 0;
            int exp    = precision - 1;
            char _str[exp+2];
            _str[0] = '\0';

            for (; exp >= 0; n++, exp--)
            {
                if (0 <= (lu - pow(2,exp)))
                {
                    _str[n] = '1';
                    lu -= pow(2,exp);
                }
                else
                {
                    _str[n] = '0';
                }
            }

            _str[sizeof(_str)-1] = '\0';
            jdebug("_str=%s", _str);
            // 20120825 no strndup function in string.h for AROS yet
            *str    = (STRPTR)strdup(_str);
            // Remove leading zeroes
            // ex. "00000110" -> "110"
            removeLeadingZeroes(str);
            jdebug("str=%s", *str);
        }; break;
        case 8: // converting to a string representing the octal
            lu    = strtol((char *)*str, NULL, old_base);
            snprintf((char *)*str, buf, "%o", (unsigned int)lu);
            break;
        case 10: // converting to a string representing the decimal
            lu    = strtol((char *)*str, NULL, old_base);
            // I'm converting from baseX to base10
            // %d is ok as I don't expect decimals
            // I'm using %lu since it's a LONG
            snprintf((char *)*str, buf, "%lu", lu);
            break;
        case 16: // converting to a string representing the hexadecimal
            lu    = strtol((char *)*str, NULL, old_base);
            snprintf((char *)*str, buf, "%X", (unsigned int)lu);
            break;
        default:
            jdebug("[convertDisplay] Base was not recognized: %d", old_base);
            break;

    }
    jdebug("[convertDisplay] ... to %s[lu=%lu] from base=%d to base=%d", *str, lu, old_base, *new_base);

    return EXIT_SUCCESS;
}

/**
    Perform real calculation
    @param myData calculation parameters (operators, numbers)
**/
exit_t doCalc(struct eData *myData)
{
    // FIXME why this doesn't get printed correctly??? Stupid D(bug()) macro.
    // D(bug("[doCalc] [%.2f] %d [%.2f]\n", myData->cs->n1, myData->cs->op, myData->cs->n2));
    // printf("[doCalc] [%.2f] %d [%.2f]\n", cs->n1, cs->op, cs->n2);
    jdebug("[doCalc] [%.2f] %d [%.2f]", myData->cs->n1, myData->cs->op, myData->cs->n2);

    switch(myData->cs->op)
    {
        case EVT_BTN_PLUS:
            myData->cs->res    = myData->cs->n1 + myData->cs->n2;
            break;
        case EVT_BTN_MINUS:
            myData->cs->res    = myData->cs->n1 - myData->cs->n2;
            break;
        case EVT_BTN_DIVIDE:
            myData->cs->res    = myData->cs->n1 / myData->cs->n2;
            break;
        case EVT_BTN_MULTIPLY:
            myData->cs->res    = myData->cs->n1 * myData->cs->n2;
            break;
        case EVT_BTN_POWERY:
            myData->cs->res    = pow(myData->cs->n1, myData->cs->n2);
            break;
    }

    return EXIT_SUCCESS;
}

/**
    Calculation interface
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doCalcWrapper(struct IClass *cl, Object *obj, struct MUIP_JCALC_doCalcMsg *msg)
{
    struct eData *myData = INST_DATA(cl,obj);

    jdebug("[doCalcWrapper] with clickedReturnBtn=%d", msg->clickedReturnBtn);

    STRPTR res = AllocVec(sizeof(STRPTR)*50, MEMF_CLEAR);
    // printf("[doCalcWrapper] Allocated %d at 0x%p\n", sizeof(STRPTR)*50, res);
    if (res == NULL)
    {
        jerror("[doOpWrapper] Failed memory allocation!");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doOpWrapper] Failed memory allocation!\n");
    }

    // STRPTR historyLine = AllocVec(sizeof(STRPTR)*HISTORY_LINE_LENGTH, MEMF_ANY);
    // if (historyLine == NULL)
    // {
    //     jerror("[doCalcWrapper] Failed allocation!");
    //     fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper] Failed allocation!\n");
    // }

    jdebug("[doCalcWrapper] mode is %d", myData->mode);

    if (isInsertMode(myData->mode))
    {
#warning FIXME This code should be removed
        /*
            This method should only perform calculations, not get the display
            Unfortunately here is used when I press "=" and changing this has
            a huge impact on the core code

            Recovering the string from the display top be put into cs->n1 or cs->n2
            is done in doCalcWrapper or doOpWrapper. Unfortunately when I click "="
            I'm triggering directly doCalcWrapper and if cs->n2 is not yet populated
            there must be some code duplicated to populate it (only in INSERTMODE).

        */

        // This method should simply get the display and put into cs->n1 or cs->n2
        // This decision is taken here
// #warning replaced with displayStr
        // STRPTR currdisplay    = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
        // STRPTR currdisplay    = StrDup(myData->displayStr);
        // jwarning("[doCalcWrapper,isInsertMode] Invoking MUIM_getDisplay with src=%s with base=%d",
        //         currdisplay, myData->cs->base);

        // FIXME Based on what do I decide to populate myData->cs->n2?
        if (TRUE != DoMethod(obj, MUIM_getDisplay, myData->displayStr, &myData->cs->n2, myData->cs->base))
        {
            jdebug("[doCalcWrapper,isInsertMode] Error retrieving display '%s'", myData->displayStr);
        }
        else
        {
            // FIXME the string "9876543210" is converted to 9876543209.999999
            jdebug("[doCalcWrapper,isInsertMode] getDisplay returned %f into cs->n2", myData->cs->n2);
        }

        // if (currdisplay)
        // {
        //     FreeVec(currdisplay); currdisplay = NULL;
        // }

        if (EXIT_SUCCESS == doCalc(myData))
        {
            jdebug("[doCalcWrapper,isInsertMode] doCalc returned %f", myData->cs->res);

            /*
                I need to set the mode NOW because when I send a message to MUIM_setDisplay
                there's a flag that removes the comma if we are in CALCMODE. Which is the case
                but I set this flag only at the end of this method.
                I cannot set it BEFORE this if..then..else choice because I need to arrive at
                this point with the correct old mode (INSMODE)
            */
            myData->mode = CALCMODE;

            snprintf((char *)res, 100, "%f", myData->cs->res);
            jdebug("[doCalcWrapper,isInsertMode] Invoking MUIM_setDisplay: TRUE");
            if (TRUE != DoMethod(obj, MUIM_setDisplay, res, TRUE))
            {
                jerror("[doCalcWrapper,isInsertMode] Write into display failed");
                fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper,isInsertMode] Write into display failed\n");
            }

            if (msg->clickedReturnBtn == TRUE)
            {
                // Print this line in history reel only if user clicked ENTER
                DoMethod(obj, MUIM_historyListInsert, myData->cs->n2, myData->cs->op, MUIV_List_Insert_Bottom);
                jdebug("[doCalcWrapper,isInsertMode] Will insert a linebreak!");

                DoMethod(obj, MUIM_historyListInsert, LINE_SEPARATOR, -1, MUIV_List_Insert_Bottom);
                DoMethod(obj, MUIM_historyListInsert, myData->cs->res, EVT_BTN_EQUALS, MUIV_List_Insert_Bottom);
            }

            if (msg->isImmediateCalc == TRUE)
            {
                DoMethod(obj, MUIM_historyListInsert, LINE_SEPARATOR, -1, MUIV_List_Insert_Bottom);
                DoMethod(obj, MUIM_historyListInsert, myData->cs->res, EVT_BTN_EQUALS, MUIV_List_Insert_Bottom);
            }
        }
        else
        {
            jerror("[doCalcWrapper,isInsertMode] Error in doCalc!");
            fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper,isInsertMode] Error in doCalc!");
        }
    }
    else if (isOpMode(myData->mode))
    {
        /*
            If I press "=" after an operator I assume that:

            [1+===...] => 1+1+1+1+1+1+...
            [1+2=+===...] => (1+2)+2+2+2+2+...

        */

        myData->cs->op    = myData->operator;
        myData->cs->n2    = myData->cs->n1;
        myData->cs->res    = 0;

        if (EXIT_SUCCESS == doCalc(myData))
        {
            snprintf((char *)res, 100, "%f", myData->cs->res);
            jdebug("[doCalcWrapper,isOpMode] Result is %s", res);
            if (TRUE != DoMethod(obj, MUIM_setDisplay, res, FALSE))
            {
                jerror("[doCalcWrapper,isOpMode] Write into display failed");
                fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper,isOpMode] Write into display failed");
            }
        }
        else
        {
            jerror("[doCalcWrapper,isOpMode] doCalc returned FAILURE");
            fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper,isOpMode] doCalc returned FAILURE");
        }

    }
    else if (isCalcMode(myData->mode))
    {
        myData->cs->n1    = myData->cs->res;
        if (EXIT_SUCCESS == doCalc(myData))
        {
            snprintf((char *)res, 100, "%f", myData->cs->res);
            jdebug("[doCalcWrapper,isCalcMode] Invoking MUIM_setDisplay: TRUE");
            if (TRUE != DoMethod(obj, MUIM_setDisplay, res, TRUE))
            {
                jerror("[doCalcWrapper,isCalcMode] Write into display failed");
                fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper,isCalcMode] Write into display failed");
            }
            else
            {
                DoMethod(obj, MUIM_historyListInsert, myData->cs->n2, myData->cs->op, MUIV_List_Insert_Bottom);
                DoMethod(obj, MUIM_historyListInsert, LINE_SEPARATOR, -1, MUIV_List_Insert_Bottom);
                DoMethod(obj, MUIM_historyListInsert, myData->cs->res, EVT_BTN_EQUALS, MUIV_List_Insert_Bottom);
            }
        }
        else
        {
            jerror("[doCalcWrapper,isOpMode] doCalc returned FAILURE");
            fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doCalcWrapper,isOpMode] doCalc returned FAILURE");
        }

    }

    if (res)
    {
        FreeVec(res); res = NULL;
    }

    myData->mode = CALCMODE;

    return (IPTR) TRUE;
}

BOOL getDisplay(STRPTR src, DOUBLE *dest, BYTE *base)
{
    /*
        This function serves the sole purpose of populating FLOATs of the calcStack
        struct, so I have to convert what I recover from the display (a string) to
        a decimal number so to have a "working" number for the operators.
        Ex. "12a" becomes "298" if base=16
    */

    // Need to temporarily convert the comma to a dot
    char c = 0;
    STRPTR sp = (STRPTR)strstr((char *)src, (char *)comma);
    if (sp)
    {
        c = *sp;
        *sp = '.';
    }

    /*
        Since 0 can legitimately be returned on both success and failure, the calling program should
        set errno to 0 before the call, and then determine if an error occurred by checking whether
        errno has a nonzero value after the call.
        (man 3 strtod)
    */

    // Convert the string from the display to a float is base10, else in a long.
    // Then store into the calcStack struct.
    errno    = 0;
    if (10 == *base)
    {
        *dest    = strtod((char *)src, NULL);
        // printf("[getDisplay] Put value %f from %s base %d (errno=%d)\n", *dest, src, *base, errno);
    }
    else
    {
        *dest    = (DOUBLE)strtol((char *)src, NULL, *base);
        // printf("[getDisplay] Put value %f from %s base %d (errno=%d)\n", f, src, *base, errno);
    }

    // FIXME This D(bug()) triggers a crash. Investigate the AROS macro, it's bugged!
    // D(bug("[getDisplay] Got value %f from %s base %d (errno=%d)\n", *dest, src, *base, errno));

    if (sp) *sp = c;

    if ((errno == ERANGE && ((int)dest == LONG_MAX || (int)dest == LONG_MIN))
        || (errno != 0 && dest == 0))
    {
        jerror("[getDisplay] Error converting string: %d", errno);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
    Get the display content, convert according to current BASE
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doGetDisplayWrapper(struct IClass *cl UNUSED, Object *obj UNUSED, struct MUIP_JCALC_getDisplayMsg *msg)
{
    char c = 0;

    jdebug("[doGetDisplayWrapper] Entered with '%s'", msg->src);

    STRPTR sp = (STRPTR)strstr((char *)msg->src, (char *)comma);
    if (sp)
    {
        c = *sp;
        *sp = '.';
    }

    errno    = 0;
    if (10 == msg->base)
    {
        // FIXME due to the 32bit nature of AROS there are issues converting numbers to DOUBLE
        // Please verify this assumption, there's probably something else behind
        *msg->dst    = strtod((char *)msg->src, NULL);
    }
    else
    {
        // Converting "FFFFFFFF" to DEC overflows: http://goo.gl/2IXax
        // Converting "11111111" to DEC too.

        unsigned int i;
        i = strtoul((char *)msg->src, NULL, msg->base);
        jdebug("[doGetDisplayWrapper] testing against overflow: %d", i);
        if ((int)i == -1)
            *msg->dst = -1;
        else
            *msg->dst = (DOUBLE)strtol((char *)msg->src, NULL, msg->base);
    }

    jdebug("[doGetDisplayWrapper] conversion produced %f", *msg->dst);

    if (sp) *sp = c;

    if ((errno == ERANGE && ((int)msg->dst == LONG_MAX || (int)msg->dst == LONG_MIN))
        || (errno != 0 && msg->dst == 0))
    {
        jdebug("[getDisplay] Error converting string: %d", errno);
        return EXIT_FAILURE;
    }

    jdebug("[doGetDisplayWrapper] src=0x%p, dst=0x%p (%2f), base=%d",
            &msg->src, &msg->dst, *msg->dst, msg->base);

    return (IPTR) TRUE;
}

/**
    Set display according to current BASE
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doSetDisplayWrapper(struct IClass *cl, Object *obj, struct MUIP_JCALC_setDisplayMsg *msg)
{
    // FIXME doSetDisplayWrapper should be reorganized

    struct eData *myData = INST_DATA(cl, obj);

    jdebug("[doSetDisplayWrapper] Received msg->val '%s'", msg->val);

    STRPTR tmpstr = StrDup(msg->val);

    // int len = strlen((char *)msg->val);
    // STRPTR tmpstr = AllocVec(sizeof(STRPTR) * 100, MEMF_CLEAR);
    // if (!tmpstr)
    // {
    //     jerror("[doSetDisplayWrapper] Failed memory allocation!");
    //     fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doSetDisplayWrapper] Failed memory allocation!\n");
    // }
    // snprintf((char *)tmpstr, strlen((char *)msg->val), "%s", msg->val);

    jdebug("[doSetDisplayWrapper] Received msg->val '%s'", msg->val);
    jdebug("[doSetDisplayWrapper] [A] tmpstr is '%s' 0x%p", tmpstr, tmpstr);

    if (msg->doProcess)
    {
         // snprintf((char *)tmpstr, 100, "%f", msg->val);
        // jdebug("[doSetDisplayWrapper] Will doProcess %s", tmpstr);
        max_decimals(&tmpstr);
        // jwarning("[doSetDisplayWrapper] after max_decimals %s", tmpstr);
        // Only convert display is new base required is different from old one
        if (10 != myData->cs->base)
            convertDisplay(&tmpstr, 10, &myData->cs->base);
        convertComma(&tmpstr);
        removeTrailingZeroes(&tmpstr);
    }
    jdebug("[doSetDisplayWrapper] [B] tmpstr is '%s' 0x%p", tmpstr, tmpstr);

    /*
        What's behind this?
        If I'm inserting numbers
            I don't want to remove the comma
        If I'm NOT inserting numbers (e.g. displaying something that could be "1234.00")
            I want to remove the comma
    */
    if (!isInsertMode(myData->mode))
    {
        removeTrailingZeroes(&tmpstr);
        // jdebug("[doSetDisplayWrapper] and removing the comma from '%s'!", tmpstr);
        removeComma(&tmpstr);
    }
    else
    {
        jdebug("[doSetDisplayWrapper] Won't remove commas from %s (myData->mode=%d)", tmpstr, myData->mode);
    }

    jdebug("[doSetDisplayWrapper] Writing on display '%s' (0x%p)", tmpstr, tmpstr);
    // set(myData->display, MUIA_Text_Contents, tmpstr);
// #warning added displayStr

    // myData->displayStr = tmpstr;
    snprintf((char *)myData->displayStr, strlen((char *)tmpstr)+1, "%s", tmpstr);
    // sprintf((char *)myData->displayStr, tmpstr);
    SET(myData->displayDraw, MUIA_CalcDisplay_Input, myData->displayStr);

    // FIXME: leave a memory leak or a crash?
    // if (tmpstr) { FreeVec(tmpstr); tmpstr = NULL; }

    jdebug("[doSetDisplayWrapper] displayStr is '%s' 0x%p", myData->displayStr, myData->displayStr);

    return (IPTR) TRUE;
}

static APTR MakeButtonImmediate(UBYTE *Label, STRPTR _id, UBYTE Key)
{
    if (0 == Key)
    {
        return(TextObject, ButtonFrame, MUIA_Text_Contents, Label,
            MUIA_Text_SetVMax, FALSE,
            MUIA_ObjectID, _id,
            MUIA_Text_PreParse, "\33c",
            MUIA_InputMode, MUIV_InputMode_Immediate,
            MUIA_Background, MUII_ButtonBack,
        End);
    }
    else
    {
        return(TextObject, ButtonFrame, MUIA_Text_Contents, Label,
            MUIA_Text_SetVMax, FALSE,
            MUIA_ObjectID, _id,
            MUIA_Text_PreParse, "\33c",
            MUIA_ControlChar, Key,
            MUIA_InputMode, MUIV_InputMode_Immediate,
            MUIA_Background, MUII_ButtonBack,
        End);
    }

}

static APTR MakeButton(UBYTE *Label, UBYTE Key)
{
    /*
    VA_LIST argp;
    VA_START(argp, Label);

        warning: "UBYTE" is promoted to "int" when passed through "..."
        note: (so you should pass "int" not "UBYTE" to "va_arg")
        note: if this code is reached, the program will abort
        UBYTE Key    = va_arg(argp, UBYTE);

    UBYTE Key        = (UBYTE)va_arg(argp++, int);
    int _InputMode    = va_arg(argp, int);
    VA_END(argp);
    D(bug("Key=%c, Label=%s\n", Key, Label));
    */

    char *preparse = "\33c";
    /*
    if (btnProp_Bold)
    {
        preparse = "\33b\33c";
    }
    else
    {
        preparse = "\33c";
    }
    */

    if (0 == Key)
    {
        return(TextObject, ButtonFrame, MUIA_Text_Contents, Label,
            MUIA_ObjectID, "generic_btn",
            MUIA_Text_SetVMax, FALSE,
            MUIA_Text_PreParse, preparse,
            MUIA_InputMode, MUIV_InputMode_RelVerify,
            MUIA_Background, MUII_ButtonBack,
        End);
    }
    else
    {
        return(TextObject, ButtonFrame, MUIA_Text_Contents, Label,
            MUIA_ObjectID, "generic_btn",
            MUIA_Text_SetVMax, FALSE,
            MUIA_Text_PreParse, preparse,
            MUIA_ControlChar, Key,
            MUIA_InputMode, MUIV_InputMode_RelVerify,
            MUIA_Background, MUII_ButtonBack,
        End);
    }
}

/**
    Add a number to the calc stack
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doAddNumber(struct IClass *cl, Object *obj, struct MUIP_JCALC_numberMsg *msg)
{
    struct eData *myData    = INST_DATA(cl,obj);
// #warning replaced with displayStr
    // STRPTR currdisplay        = (STRPTR)XGET(myData->display, MUIA_Text_Contents);

    STRPTR currdisplay = (STRPTR)StrDup(myData->displayStr);
    // jdebug("[doAddNumber] Got myData->displayStr: '%s' 0x%p", myData->displayStr, myData->displayStr);
    jdebug("[doAddNumber] Got display: '%s' from myData->displayStr (0x%p)", currdisplay, myData->displayStr);

    char tmpval[2]; tmpval[0] = '\0';

    if (msg->value >= 'A' && msg->value <= 'F') // 65 to 70 (included)
    {
        // Convert to char (show the hex digit as it is)
        sprintf((char *)tmpval, "%c", msg->value);
    }
    else
    {
        // Convert to string
        sprintf((char *)tmpval, "%d", msg->value);
    }

    /*
        if isInsertMode                = I've just put a number, will add one more
        if isOpMode                    = adding other operand
        if isCalcMode or undefined    = Pressed a number after the last "=". I want to start from scratch
    */

    if (isInsertMode(myData->mode))
    {
        int max_fig = 0;

        jdebug("[doAddNumber] myData->cs->base=%d", myData->cs->base);
        switch(myData->cs->base)
        {
            case BINBASE:
                max_fig = MAX_DISPLAY_BIN_FIGURES;
                break;
            case DECBASE:
                max_fig = MAX_DISPLAY_DEC_FIGURES;
                break;
            case HEXBASE:
                max_fig = MAX_DISPLAY_HEX_FIGURES;
                break;
            case OCTBASE:
                max_fig = MAX_DISPLAY_OCT_FIGURES;
                break;
        }

        if ((int)strlen((char *)currdisplay) <= max_fig)
        {
            int newlen = strlen((char *)currdisplay) + strlen(tmpval);
            STRPTR tmpstr = AllocVec(sizeof(STRPTR) * newlen, MEMF_CLEAR);
            // printf("[doAddNumber] Allocated %d at 0x%p\n", sizeof(STRPTR)*newlen, tmpstr);

            /* if the first and only char of the display is a zero, replace with the number */
            if ( strlen((char *)currdisplay) == 1 && *currdisplay == '0' )
            {
                jdebug("[doAddNumber] replace char");
                strncat((char *)tmpstr, tmpval, strlen(tmpval));
            }
            else
            {
                jdebug("[doAddNumber] concat display %s+%s", currdisplay, tmpval);
                strncat((char *)tmpstr, (char *)currdisplay, strlen((char *)currdisplay));
                strncat((char *)tmpstr, tmpval, strlen(tmpval));
            }

            jdebug("[doAddNumber-InsertMode] Setting display to %s", tmpstr);
            DoMethod(obj, MUIM_setDisplay, tmpstr, FALSE);
            FreeVec(tmpstr); tmpstr = NULL;
        }
        else
            jdebug("[doAddNumber] Cannot allow more than %d figures", max_fig);
    }
    else if (isOpMode(myData->mode))
    {
        jdebug("[doAddNumber-OpMode] Setting display to %s", tmpval);
        myData->cs->op    = myData->operator;
        DoMethod(obj, MUIM_setDisplay, tmpval, FALSE);
    }
    else
    {
        jdebug("[doAddNumber-%d mode] Setting display to %s", myData->mode, tmpval);
        // either isCalcMode or undefined
        myData->cs->op    = -1;
        myData->cs->n1    = .0f;
        myData->cs->n2    = .0f;
        myData->cs->res    = .0f;
        DoMethod(obj, MUIM_setDisplay, tmpval, FALSE);
    }

    // D(bug("[btnNumClick] Setting mode to %d with op=%d\n", INSMODE, myData->cs->op));
    myData->mode    = INSMODE;

    if (currdisplay)
    {
        FreeVec(currdisplay); currdisplay = NULL;
    }

    return (IPTR) TRUE;
}

/**
    Add a constant to the calc stack
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doAddConstant(struct IClass *cl, Object *obj, struct MUIP_JCALC_numberMsg *msg)
{
    struct eData *myData    = INST_DATA(cl, obj);
    int _val                = msg->value;

    switch(_val)
    {
        case 'P':
            DoMethod(obj, MUIM_setDisplay, PI, FALSE);
            break;
    }

    myData->cs->op    = myData->operator;
    myData->mode    = INSMODE;

    return (IPTR) TRUE;
}

/**
    Operator interface
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doOpWrapper(struct IClass *cl, Object *obj, struct MUIP_JCALC_doOpMsg *msg)
{
    jdebug("[doOpWrapper]");
    struct eData *myData    = INST_DATA(cl,obj);
    BYTE value                = msg->value;

// #warning replaced with displayStr
    STRPTR tmpstr = StrDup(myData->displayStr);
    jdebug("[doOpWrapper] myData->displayStr='%s', tmpstr='%s'", myData->displayStr, tmpstr);

    STRPTR res = AllocVec(sizeof(STRPTR)*50, MEMF_CLEAR);
    // printf("[doOpWrapper] Allocated %d at 0x%p\n", sizeof(STRPTR)*50, res);
    if (res == NULL)
    {
        jerror("[doOpWrapper] Failed memory allocation!");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doOpWrapper] Failed memory allocation!\n");
    }

    jdebug("[doOpWrapper] *** TAG MEMORY ALLOC ***  0x%p (\"%s\", %zu bytes)",
        res, res, strlen((char *)res));

    jdebug("[doOpWrapper] Operator is %d, mode is %d", value, myData->mode);

    if (isInsertMode(myData->mode))
    {
        /*
            The user has just put an operator: what should I do if I was in
            insertMode (that is, he put an operator after having put numbers)?

            If cs->n1 is 0, I can't perform a calulation:
            - Save the display in cs->n1

            If cs->n2 is 0, I will shortly be able to perform a calculation:
            - Save the display in cs->n2
            - do the calculation

            If cs->n2 is *not* empty, I can do a calculation:
            - Move cs->res in cs->n1
            - Save display in cs->n2
            - do the calculation (I should already have cs->op populated)

        */

        // printf("[btnOpClick,isInsertMode]\n"); //  Prima: cs->n1=%f, cs->n2=%f, cs->res=%f\n",
        //            myData->cs->n1, myData->cs->n2, myData->cs->res);

        if (0 == myData->cs->n1)
        {
            jdebug("[doOpWrapper] cs->n1 is 0, will put something in it");
            jdebug("[doOpWrapper] Display is '%s', base is %d", tmpstr, myData->cs->base);

            if (TRUE != DoMethod(obj, MUIM_getDisplay, tmpstr, &myData->cs->n1, myData->cs->base))
            {
                jerror("[doOpWrapper] Err converting %s", tmpstr);
            }
            else
            {
                jdebug("[doOpWrapper] getDisplay returned %f into cs->n1 from '%s' and base:%d",
                    myData->cs->n1, tmpstr, myData->cs->base);
            }

            // Write to history reel
            DoMethod(obj, MUIM_historyListInsert, myData->cs->n1, value, MUIV_List_Insert_Bottom);
        }
        else if (0 == myData->cs->n2)
        {
            jdebug("[doOpWrapper] cs->n2 is 0, will put something in it, then will doCalc");
            // printf("[btnOpClick] cs->n2 is 0 (%f)?\n", myData->cs->n2);
            // tmpstr = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
            jdebug("[doOpWrapper] Display is %s", tmpstr);

            if (TRUE != DoMethod(obj, MUIM_getDisplay, tmpstr, &myData->cs->n2, myData->cs->base))
            {
                jdebug("Err converting %s", tmpstr);
            }
            else
            {
                jdebug("[doOpWrapper] Successfully got display into myData->cs->n2: %f", myData->cs->n2);
            }

            if (value == EVT_BTN_PERCENT)
            {
                /* Special case, the % operator
                 *
                 * Will trick the calculation into using a different operator
                 * Example:
                 *   n1 | op | n2 | value
                 *     "72    +   5    %"
                 * must be converted into
                 *     72 [op] (72 * 5 / 100) = result
                 */

                jdebug("[doOpWrapper] Op is %d", myData->cs->op);
                switch(myData->cs->op)
                {
                    case EVT_BTN_DIVIDE:
                    case EVT_BTN_MULTIPLY:
                    case EVT_BTN_POWERY:
                        myData->cs->n2    = myData->cs->n2 / 100;
                        snprintf((char *)tmpstr, 100, "%f", myData->cs->n2);
                        removeTrailingZeroes(&tmpstr);
                        break;
                    case EVT_BTN_PLUS:
                    case EVT_BTN_MINUS:
                        jdebug("[doOpWrapper] tmpstr is '%s' (0x%p)", tmpstr, tmpstr);
                        myData->cs->n2    = myData->cs->n1 * ( myData->cs->n2 / 100);
                        snprintf((char *)tmpstr, 100, "%f", myData->cs->n2);
                        jdebug("[doOpWrapper] tmpstr is '%s' (0x%p)", tmpstr, tmpstr);
                        removeTrailingZeroes(&tmpstr);
                        jdebug("[doOpWrapper] tmpstr is '%s' (0x%p)", tmpstr, tmpstr);
                        break;
                }
            }

            // Write to history reel
            DoMethod(obj, MUIM_historyListInsert, myData->cs->n2, myData->cs->op, MUIV_List_Insert_Bottom);

            if (TRUE == DoMethod(obj, MUIM_doCalc, FALSE, TRUE))
            {
                /*
                // %2.f rounds too much
                // TODO unit-tests
                snprintf((char *)tmpstr, 100, "%f", myData->cs->res);
                // removeTrailingZeroes(&tmpstr);

                // Only convert display if new base required is different from old one
                if (10 != myData->cs->base)
                    convertDisplay(&tmpstr, 10, &myData->cs->base);

                convertComma(&tmpstr);
                set(myData->display, MUIA_Text_Contents, (STRPTR)tmpstr);
                */

                snprintf((char *)res, 100, "%f", myData->cs->res);
                if (TRUE != DoMethod(obj, MUIM_setDisplay, res, FALSE))
                {
                    jerror("[doOpWrapper] Failed display write");
                    fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doOpWrapper] Failed display write");
                }
                else
                    jdebug("[doOpWrapper] Successfully wrote into display");
            }
            else
            {
                jerror("[doOpWrapper] MUIM_doCalc returned FAILURE");
                fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[doOpWrapper] MUIM_doCalc returned FAILURE");
            }

            /*
             * Here, too, I must assume that the user is putting a series of numbers
             * Example: if he's performing 3 * 5 *2, the steps are as follows:
             *
             * cs1        cs2        res        op
             * 0        0        0        -
             * 3        0        0        -
             * 3        0        0        *
             * 3        5        0        -
             * 3        5        0        *
             * 3        5        15        - <-- ok so far
             * 3        5        15        *
             * 15        0        0        -
             * ... and so on
             *
             */

            // printf("[btnOpClick,isInsertMode] Am I sure? cs->n1=%f, cs->n2=%f, cs->res=%f\n",
            //             myData->cs->n1, myData->cs->n2, myData->cs->res);

            // Write to history reel ?

            myData->cs->n1    = myData->cs->res;
            myData->cs->n2    = 0;
            myData->cs->res    = 0;
        }
        else // cs->n1 and cs->n2 are empty
        {
            jdebug("[doOpWrapper] both cs->n2 and cs->n1 are empty, I assume someone is willing to perform another calculation");
            /*
                Neither cs->n1 nor cs->n2 are 0, so I assume the user
                is inserting many numbers for the operation:
                2+54+6+321-45....

                I have to perform the calculation right away after I
                moved the result in cs->n1 and having emptied cs->n2

                The right operator is already in place.
            */

            // printf("[EVT] n1=%f, n2=%f, op=%d, res=%f\n",
            //         myData->cs->n1, myData->cs->n2, myData->cs->op, myData->cs->res);

            // Shall put into n1 what I have on the display because it can be tampered
            // (f.e. changed sign)
            // tmpstr = (STRPTR)XGET(myData->display, MUIA_Text_Contents);

            if (TRUE != DoMethod(obj, MUIM_getDisplay, tmpstr, &myData->cs->n1, myData->cs->base))
            {
                D(bug("Err converting %s\n", tmpstr));
            }

            // myData->cs->n1    = myData->cs->res;
            // jdebug("n1=%f!", myData->cs->n1);
            myData->cs->n2    = 0;
            myData->cs->res    = 0;

            // tmpstr = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
            jdebug("[doOpWrapper] Display is %s", tmpstr);

            if (TRUE != DoMethod(obj, MUIM_getDisplay, tmpstr, &myData->cs->n2, myData->cs->base))
            {
                D(bug("Err converting %s\n", tmpstr));
            }

            if (TRUE != DoMethod(obj, MUIM_doCalc, FALSE))
            {
                /*
                snprintf((char *)tmpstr, 100, "%f", myData->cs->res);
                // removeTrailingZeroes(&tmpstr);

                convertComma(&tmpstr);

                D(bug("[btnOpClick] Will display %s because res=%f and base=%d\n",
                        tmpstr, myData->cs->res, myData->cs->base));
                set(myData->display, MUIA_Text_Contents, (STRPTR)tmpstr);
                */
                snprintf((char *)res, 100, "%f", myData->cs->res);
                if (TRUE != DoMethod(obj, MUIM_setDisplay, res, FALSE))
                {
                    jerror("[doOpWrapper] Failed display write");
                }
            }
            else
            {
                jerror("[doOpWrapper] Error in doCalc");
            }
        }
    }
    else if (isCalcMode(myData->mode))
    {
        /*
            If I press an operator after "=", I assume the user
            wants to start a new calculation with the last result as
            first number of the new calculation.
            So I place the result in cs->n1 and reset everything else.
            Now I'm prepared to receive more numbers.
        */
        myData->cs->op    = -1;

        jdebug("[doOpWrapper] isCalcMode");

        // Shall put into n1 what I have on the display because it can be tampered
        // (f.e. changed sign)
        if (TRUE != DoMethod(obj, MUIM_getDisplay, tmpstr, &myData->cs->n1, myData->cs->base))
        {
            jdebug("Err converting %s", tmpstr);
        }

        // myData->cs->n1    = myData->cs->res;
        myData->cs->n2    = 0;
        myData->cs->res    = 0;
    }
    else
    {
        jdebug("[doOpWrapper, else: %d]", myData->mode);
    }

    if (res)
    {
        jdebug("[doOpWrapper] *** TAG MEMORY FREE ***  0x%p (\"%s\", %zu bytes)",
            res, res, strlen((char *)res));
        FreeVec(res); res = NULL;
    }

    if(tmpstr)
    {
        jdebug("[doOpWrapper] *** TAG MEMORY FREE ***  0x%p (\"%s\", %zu bytes)",
            tmpstr, tmpstr, strlen((char *)tmpstr));
        FreeVec(tmpstr); tmpstr = NULL;
    }

    /*
        Operator update will be done here, at the end
    */
    myData->operator    = value;
    jdebug("[doOpWrapper] Setting mode to %d with op=%d", OPMODE, myData->operator);
    myData->mode        = OPMODE;

    return (IPTR) TRUE;
}

/**
    Immediate operator interface
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doOpWrapperImmediate(struct IClass *cl, Object *obj, struct MUIP_JCALC_doOpMsg *msg)
{
    struct eData *myData    = INST_DATA(cl, obj);
    BYTE value                = msg->value;
    STRPTR _display            = myData->displayStr;
    DOUBLE dest                = 0;

    if (10 == myData->cs->base)
    {
        if (TRUE != DoMethod(obj, MUIM_getDisplay, _display, &dest, myData->cs->base))
        {
            jerror("[doOpWrapperImmediate] Error converting string");
        }
        else
        {
            switch(value)
            {
                case EVT_BTN_SQRT:
                    myData->cs->res    = sqrt(dest);
                    break;
                case EVT_BTN_SQUARE:
                    myData->cs->res    = pow(dest, 2);
                    break;
                case EVT_BTN_INVERSE:
                    myData->cs->res    = 1 / dest;
                    break;
            }

            STRPTR res = AllocVec(50*sizeof(STRPTR), MEMF_CLEAR);
            // printf("[doOpWrapperImmediate] Allocated %d at 0x%p\n", sizeof(STRPTR)*50, res);
            snprintf((char *)res, 100, "%f", myData->cs->res);
            removeTrailingZeroes(&res);
            removeComma(&res);
            jdebug("[doOpWrapperImmediate] Invoking MUIM_setDisplay: TRUE");
            DoMethod(obj, MUIM_setDisplay, res, TRUE);

            if (res)
            {
                FreeVec(res); res = NULL;
            }

            DoMethod(obj, MUIM_historyListInsert, dest, value, MUIV_List_Insert_Bottom);
            DoMethod(obj, MUIM_historyListInsert, LINE_SEPARATOR, -1, MUIV_List_Insert_Bottom);
            DoMethod(obj, MUIM_historyListInsert, myData->cs->res, EVT_BTN_EQUALS, MUIV_List_Insert_Bottom);
        }
    }

    return (IPTR) TRUE;
}

/**
    Add a dot
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR doAddDot(struct IClass *cl, Object *obj)
{
    struct eData *myData     = INST_DATA(cl, obj);
    jdebug("[doAddDot] Mode is %d", myData->mode);
    STRPTR tmpstr; // = (STRPTR)"";
    int size;

    if (isOpMode(myData->mode))
    {
        jdebug("[doAddDot] Will erase the display");
        /* erase the display and start writing a new number beginning with a dot */
        myData->cs->op    = myData->operator;
        // tmpstr = (STRPTR)strcat((char *)tmpstr, (char *)"0.");
        // tmpstr = (STRPTR)strncpy((char *)tmpstr, (char *)"0.", 2);
        // tmpstr = AllocVec(5, MEMF_ANY);

        size = sizeof(STRPTR)*2;
        tmpstr = AllocVec(size, MEMF_CLEAR);
        // printf("[doAddDot] Allocated %d at 0x%p\n", size, tmpstr);
        // memset(tmpstr, 0, size);
        tmpstr = StrDup((STRPTR)"0.");
    }
    else
    {
        jdebug("[doAddDot] Will concat the display");
// #warning replaced with displayStr
        // tmpstr = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
        // tmpstr = myData->displayStr;

        // jdebug("[doAddDot] before tmpstr[%d]:'%s'", strlen((char *)tmpstr), tmpstr);
        if (NULL == strstr((char *)myData->displayStr, (char *)comma))
        {
            size = sizeof(STRPTR) * strlen((char *)myData->displayStr) +
                    sizeof(STRPTR) * strlen((char *)comma);

            tmpstr = AllocVec(size, MEMF_CLEAR);
            // printf("[doAddDot] Allocated %d at 0x%p\n", size, tmpstr);
            // memset(tmpstr, 0, size);
            jdebug("[doAddDot] after allocvec tmpstr (0x%p):'%s'", tmpstr, tmpstr);
            jdebug("[doAddDot] myData->displayStr (0x%p):'%s'", myData->displayStr, myData->displayStr);

            strcat((char *)tmpstr, (char *)myData->displayStr);
            strcat((char *)tmpstr, (char *)comma);
        }
        else
        {
                    tmpstr = StrDup((STRPTR)myData->displayStr);
        }

        jdebug("[doAddDot] after tmpstr[%zu]:'%s'", strlen((char *)tmpstr), tmpstr);
    }

    /* Set INSMODE now in order to prevent problems in MUIM_setDisplay */
    myData->mode = INSMODE;

    STRPTR toDisplay = (STRPTR)StrDup(tmpstr);

    jdebug("[doAddDot] Will set display to '%s'", toDisplay);
    DoMethod(obj, MUIM_setDisplay, toDisplay, FALSE);

    if (tmpstr) { FreeVec(tmpstr); tmpstr = NULL; }

    return (IPTR) TRUE;
}

/**
    Clear the display
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doClearDisplay(struct IClass *cl, Object *obj, struct MUIP_JCALC_ClearMsg *msg)
{
    struct eData *myData = INST_DATA(cl, obj);
    BYTE flag    = msg->flag;
    jdebug("[doClearDisplay] Will set display to 0 and reset everything");

    if (flag == DISPLAY_CA)
    {
        myData->cs->op    = -1;
        myData->cs->n1    = .0f;
        myData->cs->n2    = .0f;
        myData->cs->res    = .0f;
        myData->mode    = 0;
    }

    char zero[2] = {'0', '\0'};
    DoMethod(obj, MUIM_setDisplay, zero, FALSE);

    return (IPTR) TRUE;
}

/**
    Clear the rightmost char from the display
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doClearOneChar(struct IClass *cl, Object *obj)
{
    struct eData *myData = INST_DATA(cl, obj);

// #warning replaced with displayStr
    // char *str = (char *)XGET(myData->display, MUIA_Text_Contents);
    // char *str = (char *)myData->displayStr;

    // FIXME won't print this line!
    jdebug("[doClearOneChar] Current myData->displayStr is '%s'", myData->displayStr );

    // If I'm showing a result, don't touch the display
    if (isCalcMode(myData->mode))
    {
        return (IPTR) TRUE;
    }

    int len = strlen((char *)myData->displayStr);
    if (1 == len)
    {
        jdebug("[doClearOneChar] len:%d so setting display to 0", len);
        char zero[2] = {'0', '\0'};
        DoMethod(obj, MUIM_setDisplay, zero, FALSE);
    }
    else
    {
        // eat back one char
        char str[len+1]; str[0] = '\0';
        sprintf(str, "%s", (char *)myData->displayStr);
        jdebug("[doClearOneChar] Current str is:%s", str);

        str[len-1]    = '\0';
        DoMethod(obj, MUIM_setDisplay, str, FALSE);
    }

    return (IPTR) TRUE;
}

/**
    Save history reel
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR setSaveAs(struct IClass *cl, Object *obj)
{
    // Have GCC just shut up!
    jdebug("[setSaveAs] Unused params cl=0x%p", cl);

    if (XGET(obj, MUIA_JCALC_SaveAsCSV) == 0)
    {
        SetAttrs(obj, MUIA_JCALC_SaveAsCSV, TRUE, TAG_DONE);
        SetAttrs(menu_csv, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
    }
    else
    {
        SetAttrs(obj, MUIA_JCALC_SaveAsCSV, FALSE, TAG_DONE);
        SetAttrs(menu_csv, MUIA_Menuitem_Checked, FALSE, TAG_DONE);
    }

    return (IPTR) TRUE;
}

/**
    History panel toggle
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR toggleHistoryPanel(struct IClass *cl, Object *obj)
{
    jdebug("[toggleHistoryPanel] [start] MUIA_JCALC_toggleHistoryPanel %d", (int)XGET(obj, MUIA_JCALC_toggleHistoryPanel));
    struct eData *myData = INST_DATA(cl, obj);

    jdebug("[toggleHistoryPanel] Will actually make the panel appear/disappear");
    DoMethod(obj, MUIM_ManageGroup, obj, myData->historyPanel);

    jdebug("[toggleHistoryPanel] Will set attr and menu checks");
    if (XGET(obj, MUIA_JCALC_toggleHistoryPanel) == FALSE)
    {
        SetAttrs(obj, MUIA_JCALC_toggleHistoryPanel, TRUE, TAG_DONE);
        SetAttrs(menu_history, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
    }
    else
    {
        SetAttrs(obj, MUIA_JCALC_toggleHistoryPanel, FALSE, TAG_DONE);
        SetAttrs(menu_history, MUIA_Menuitem_Checked, FALSE, TAG_DONE);
    }

    jdebug("[toggleHistoryPanel] [end] MUIA_JCALC_toggleHistoryPanel %d", (int)XGET(obj, MUIA_JCALC_toggleHistoryPanel));

    return (IPTR) TRUE;
}

/**
    Set BASE
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR setBase(struct IClass *cl, Object *obj, struct MUIP_JCALC_setBaseMsg *msg)
{
    struct eData *myData = INST_DATA(cl, obj);

    jdebug("setBase: %d", msg->base);
    switch (msg->base)
    {
        case BINBASE:
        {
            if (BINBASE != myData->baseMode)
            {
                myData->baseMode = BINBASE;
                DoMethod(obj, MUIM_setBaseBin);
                SetAttrs(menu_bin, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
            }
        }; break;
        case DECBASE:
        {
            if (DECBASE != myData->baseMode)
            {
                myData->baseMode = DECBASE;
                DoMethod(obj, MUIM_setBaseDec);
                SetAttrs(menu_dec, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
            }
        }; break;
        case HEXBASE:
        {
            if (HEXBASE != myData->baseMode)
            {
                myData->baseMode = HEXBASE;
                DoMethod(obj, MUIM_setBaseHex);
                SetAttrs(menu_hex, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
            }
        }; break;
        case OCTBASE:
        {
            if (OCTBASE != myData->baseMode)
            {
                myData->baseMode = OCTBASE;
                DoMethod(obj, MUIM_setBaseOct);
                SetAttrs(menu_oct, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
            }
        }; break;
    }

    return (IPTR) TRUE;
}

/**
    Set calculator MODE
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR setMode(struct IClass *cl, Object *obj, struct MUIP_JCALC_setModeMsg *msg)
{
    jdebug("setMode: %d", msg->mode);
    struct eData *myData = INST_DATA(cl, obj);
    switch (msg->mode)
    {
        case BASICMODE:
        {
            /* We dont want to re-set the same mode if already set! */
            if (BASICMODE != myData->interfaceMode)
            {
                myData->interfaceMode = BASICMODE;
                DoMethod(obj, MUIM_setBaseDec);
                SetAttrs(menu_basic, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
                SetAttrs(menu_base, MUIA_Menu_Enabled, FALSE, TAG_DONE);
                SetAttrs(menu_dec, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
                DoMethod(obj, MUIM_ManageGroup, myData->grp_main, myData->grp_scientific);
            }
        }; break;
        case PRGMODE:
        {
            if (PRGMODE != myData->interfaceMode)
            {
                // DoMethod(obj, MUIM_setModePrg);
                myData->interfaceMode = PRGMODE;
                SetAttrs(menu_prg, MUIA_Menuitem_Checked, TRUE, TAG_DONE);
                SetAttrs(menu_base, MUIA_Menu_Enabled, TRUE, TAG_DONE);
                DoMethod(obj, MUIM_ManageGroup, myData->grp_main, myData->grp_scientific);
            }
        }; break;
    }

    return (IPTR) TRUE;
}

/**
    Generic group add/remove manager
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mManageGroup(struct IClass *cl, Object *obj, struct MUIP_JCALC_manageGroup *msg)
{
    jdebug("[mManageGroup] obj=0x%p parent=0x%p, child=0x%p", obj, msg->_mainGrp, msg->_subGrp);
    Object *parent            = msg->_mainGrp;
    Object *_subgrp            = msg->_subGrp;

    struct eData *myData     = INST_DATA(cl, obj);

    STRPTR _subgrp_id        = (STRPTR)XGET(_subgrp, MUIA_ObjectID);
    // Object *parent        = (Object *)XGET(_subgrp, MUIA_Parent);
    // STRPTR _parent_id    = (STRPTR)XGET(parent, MUIA_ObjectID);
    // jwarning("[mManageGroup] parent of _subgrp (%s) is 0x%p (%s)", _subgrp_id, parent, _parent_id);

    if (parent == NULL)
    {
        jerror("[mManageGroup] Can't find a parent (parent is @ 0x%p)!!!!!", parent);
        return (IPTR)EXIT_FAILURE;
    }
    else
        jdebug("[mManageGroup] parent is %s @0x%p", (STRPTR)XGET(parent, MUIA_ObjectID), parent);

    Object *_obj;
    BOOL exists                = FALSE;
    struct List *children    = (struct List *)XGET(parent, MUIA_Group_ChildList);
    APTR pObj                = GetHead(children);
    while(NULL != (_obj = NextObject((Object **)&pObj)) && !exists)
    {
        STRPTR _id = (STRPTR)XGET(_obj,MUIA_ObjectID);
        // if (_subgrp_id == (STRPTR)XGET(_obj,MUIA_ObjectID))
        if (_id != NULL)
        {
            if (0 == strncmp((char *)_subgrp_id, (char *)_id, strlen((char *)_id)))
            {
                exists    = TRUE;
            }
        }
    }

    jdebug("[mManageGroup] Subgroup is in maingrp? %d", exists);

    // add or remove the subgroup
    if(DoMethod(parent, MUIM_Group_InitChange))
    {
        jdebug("[mManageGroup] InitChange");

        if (FALSE == exists)
        {
            jdebug("[mManageGroup] Adding group %s!", _subgrp_id);
            DoMethod(parent, OM_ADDMEMBER, _subgrp);

            /* Order a different set of objects according to what I'm adding and where */
            if (0 == strcmp("IdGrpSci", (char *)_subgrp_id))
            {
                DoMethod(myData->grp_main, MUIM_Group_Sort, myData->grp_scientific, myData->grp_base, NULL);
            }
            else
            {
                // ok if I'm activating grp_historyPanel
// #warning replaced with displayStr
                // DoMethod(obj, MUIM_Group_Sort, myData->historyPanel, myData->display, myData->grp_main, NULL);
                DoMethod(obj, MUIM_Group_Sort, myData->historyPanel, myData->displayDraw, myData->grp_main, NULL);
            }
        }
        else
        {
            jdebug("[mManageGroup] Removing group %s!", _subgrp_id);
            DoMethod(parent, OM_REMMEMBER, _subgrp);
            jdebug("[mManageGroup] Group removed!");
        }

        DoMethod(parent, MUIM_Group_ExitChange);
    }
    else
    {
        jerror("[mManageGroup] Failed MUIM_Group_InitChange!");
    }

    return (IPTR) TRUE;
}

/**
    Set calc BASE toi BIN
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR setBaseBin(struct IClass *cl, Object *obj)
{
    // - disable all the buttons I don't need: 2-9, a-f
    // - set base mode to 2
    // - convert the display

    struct eData *myData = INST_DATA(cl, obj);

    /*
     * How do I get and modify *any* widget of my UI that is *not* declared in the private
     * data struct of my class? Should I overload OM_GET/OM_SET?
     *
     * The private data struct is a "bin" for whatever data the class needs!
     */

    // update the display, convert whatever it is on screen to "bin"
    // store the current base mode in order to to the conversion
    BYTE curr_base        = myData->cs->base;
    myData->cs->base    = 2;
// #warning replaced with displayStr
    // STRPTR str            = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
    STRPTR str    = myData->displayStr;

    convertDisplay(&str, curr_base, &myData->cs->base);
    DoMethod(obj, MUIM_setDisplay, str, FALSE);

    // Enable all the 'scientific' buttons
    Object *_obj;
    struct List *children    = (struct List *)XGET(myData->grp_mode, MUIA_Group_ChildList);
    APTR pObj                = GetHead(children);
    while(NULL != (_obj = NextObject((Object **)&pObj)))
    {
        STRPTR _id = (STRPTR)XGET(_obj, MUIA_ObjectID);
        if (0 != strncmp((char *)"IdBtnBin", (char *)_id, strlen((char *)"IdBtnBin")))
            set(_obj, MUIA_Selected, FALSE);
    }

    // ... but disable the HEX digits
    set(myData->grp_hex, MUIA_Disabled, TRUE);

    set(myData->btn_two, MUIA_Disabled, TRUE);
    set(myData->btn_three, MUIA_Disabled, TRUE);
    set(myData->btn_four, MUIA_Disabled, TRUE);
    set(myData->btn_five, MUIA_Disabled, TRUE);
    set(myData->btn_six, MUIA_Disabled, TRUE);
    set(myData->btn_seven, MUIA_Disabled, TRUE);
    set(myData->btn_eight, MUIA_Disabled, TRUE);
    set(myData->btn_nine, MUIA_Disabled, TRUE);

    /*
        Ensure selected button is selected (hint: when selecting a menu item).
        Setting MUIA_Selected automatically triggers another chain of events.
        We don't want to end up in a notification loop so instead of set() I use
        nnset() (http://goo.gl/bwsqU)
    */
    nnset(myData->btn_bin, MUIA_Selected, TRUE);

    /* Buttons and functions that should be disabled */
    set(myData->btn_dot, MUIA_Disabled, TRUE);
    set(myData->btn_sqrt, MUIA_Disabled, TRUE);
    set(myData->btn_power_two, MUIA_Disabled, TRUE);
    set(myData->btn_power_y, MUIA_Disabled, TRUE);
    set(myData->btn_percent, MUIA_Disabled, TRUE);
    set(myData->btn_inverse, MUIA_Disabled, TRUE);
    set(myData->btn_invert_sign, MUIA_Disabled, TRUE);
    set(myData->btn_pi, MUIA_Disabled, TRUE);

    return (IPTR) TRUE;
}

/**
    Set calc BASE to DEC
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR setBaseDec(struct IClass *cl, Object *obj)
{
    // disable all the buttons I don't need:
    // a-f, set base mode to 10
    struct eData *myData = INST_DATA(cl, obj);

    // update the display, convert whatever it is on screen to "dec"
    BYTE curr_base    = myData->cs->base;
    myData->cs->base    = 10;
    // printf("[setDecBase] Set base mode to %d\n", myData->cs->base);

// #warning replaced with displayStr
    // str    = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
    STRPTR str = StrDup(myData->displayStr);

    jdebug("[setBaseDec] displayStr at '%s' 0x%p", myData->displayStr, myData->displayStr);
    convertDisplay(&str, curr_base, &myData->cs->base);
    DoMethod(obj, MUIM_setDisplay, str, FALSE);
    jdebug("[setBaseDec] displayStr at '%s' 0x%p", myData->displayStr, myData->displayStr);

    // Enable all the 'scientific' buttons
    Object *_obj;
    struct List *children    = (struct List *)XGET(myData->grp_mode, MUIA_Group_ChildList);
    APTR pObj                = GetHead(children);
    while(NULL != (_obj = NextObject((Object **)&pObj)))
    {
        STRPTR _id = (STRPTR)XGET(_obj, MUIA_ObjectID);
        if (0 != strncmp((char *)"IdBtnDec", (char *)_id, strlen((char *)"IdBtnDec")))
            set(_obj, MUIA_Selected, FALSE);
    }

    set(myData->grp_hex, MUIA_Disabled, TRUE);

    set(myData->btn_two, MUIA_Disabled, FALSE);
    set(myData->btn_three, MUIA_Disabled, FALSE);
    set(myData->btn_four, MUIA_Disabled, FALSE);
    set(myData->btn_five, MUIA_Disabled, FALSE);
    set(myData->btn_six, MUIA_Disabled, FALSE);
    set(myData->btn_seven, MUIA_Disabled, FALSE);
    set(myData->btn_eight, MUIA_Disabled, FALSE);
    set(myData->btn_nine, MUIA_Disabled, FALSE);

    /* See comment in setBaseBin */
    nnset(myData->btn_dec, MUIA_Selected, TRUE);

    /* Buttons and functions that should be enabled */
    set(myData->btn_dot, MUIA_Disabled, FALSE);
    set(myData->btn_sqrt, MUIA_Disabled, FALSE);
    set(myData->btn_power_two, MUIA_Disabled, FALSE);
    set(myData->btn_power_y, MUIA_Disabled, FALSE);
    set(myData->btn_percent, MUIA_Disabled, FALSE);
    set(myData->btn_inverse, MUIA_Disabled, FALSE);
    set(myData->btn_invert_sign, MUIA_Disabled, FALSE);
    set(myData->btn_pi, MUIA_Disabled, FALSE);

    if (str)
    {
        FreeVec(str); str = NULL;
    }

    return (IPTR) TRUE;
}

/**
    Set calc BASE to HEX
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR setBaseHex(struct IClass *cl, Object *obj)
{
    // Enable all the buttons I need:
    // 0-9, a-f, set base mode to 16
    struct eData *myData    = INST_DATA(cl ,obj);

    // update the display, convert whatever it is on screen to "hex"
    BYTE curr_base    = myData->cs->base;
    myData->cs->base    = 16;
    // printf("[setHexBase] Set base mode to %d\n", myData->cs->base);
    STRPTR str;
// #warning replaced with displayStr
    // str    = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
    str    = myData->displayStr;
    convertDisplay(&str, curr_base, &myData->cs->base);
    DoMethod(obj, MUIM_setDisplay, str, FALSE);

    // Enable all the 'scientific' buttons
    Object *_obj;
    struct List *children    = (struct List *)XGET(myData->grp_mode, MUIA_Group_ChildList);
    APTR pObj                = GetHead(children);
    while(NULL != (_obj = NextObject((Object **)&pObj)))
    {
        STRPTR _id = (STRPTR)XGET(_obj, MUIA_ObjectID);
        if (0 != strncmp((char *)"IdBtnHex", (char *)_id, strlen((char *)"IdBtnHex")))
            set(_obj, MUIA_Selected, FALSE);
    }

    set(myData->grp_scientific, MUIA_Disabled, FALSE);

    set(myData->btn_two, MUIA_Disabled, FALSE);
    set(myData->btn_three, MUIA_Disabled, FALSE);
    set(myData->btn_four, MUIA_Disabled, FALSE);
    set(myData->btn_five, MUIA_Disabled, FALSE);
    set(myData->btn_six, MUIA_Disabled, FALSE);
    set(myData->btn_seven, MUIA_Disabled, FALSE);
    set(myData->btn_eight, MUIA_Disabled, FALSE);
    set(myData->btn_nine, MUIA_Disabled, FALSE);

    /* See comment in setBaseBin */
    nnset(myData->btn_hex, MUIA_Selected, TRUE);

    /* Buttons and functions that should be disabled */
    set(myData->btn_dot, MUIA_Disabled, TRUE);
    set(myData->btn_sqrt, MUIA_Disabled, TRUE);
    set(myData->btn_power_two, MUIA_Disabled, TRUE);
    set(myData->btn_power_y, MUIA_Disabled, TRUE);
    set(myData->btn_percent, MUIA_Disabled, TRUE);
    set(myData->btn_inverse, MUIA_Disabled, TRUE);
    set(myData->btn_invert_sign, MUIA_Disabled, TRUE);
    set(myData->btn_pi, MUIA_Disabled, TRUE);

    return (IPTR) TRUE;
}

/**
    Set calc BASE to OCT
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR setBaseOct(struct IClass *cl, Object *obj)
{
    // disable all the buttons I don't need:
    // 8-9, a-f, set base mode to 8
    struct eData *myData    = INST_DATA(cl, obj);

    // update the display, convert whatever it is on screen to "oct"
    BYTE curr_base    = myData->cs->base;
    myData->cs->base    = 8;
    // printf("[setOctBase] Set base mode to %d\n", myData->cs->base);
    STRPTR str;
// #warning replaced with displayStr
    // str    = (STRPTR)XGET(myData->display, MUIA_Text_Contents);
    str    = myData->displayStr;
    convertDisplay(&str, curr_base, &myData->cs->base);
    DoMethod(obj, MUIM_setDisplay, str, FALSE);

    // Enable all the 'scientific' buttons
    Object *_obj;
    struct List *children    = (struct List *)XGET(myData->grp_mode, MUIA_Group_ChildList);
    APTR pObj                = GetHead(children);
    while(NULL != (_obj = NextObject((Object **)&pObj)))
    {
        STRPTR _id = (STRPTR)XGET(_obj, MUIA_ObjectID);
        if (0 != strncmp((char *)"IdBtnOct", (char *)_id, strlen((char *)"IdBtnOct")))
            set(_obj, MUIA_Selected, FALSE);
    }

    set(myData->grp_hex, MUIA_Disabled, TRUE);

    set(myData->btn_two, MUIA_Disabled, FALSE);
    set(myData->btn_three, MUIA_Disabled, FALSE);
    set(myData->btn_four, MUIA_Disabled, FALSE);
    set(myData->btn_five, MUIA_Disabled, FALSE);
    set(myData->btn_six, MUIA_Disabled, FALSE);
    set(myData->btn_seven, MUIA_Disabled, FALSE);
    set(myData->btn_eight, MUIA_Disabled, TRUE);
    set(myData->btn_nine, MUIA_Disabled, TRUE);

    /* See comment in setBaseBin */
    nnset(myData->btn_oct, MUIA_Selected, TRUE);

    /* Buttons and functions that should be disabled */
    set(myData->btn_dot, MUIA_Disabled, TRUE);
    set(myData->btn_sqrt, MUIA_Disabled, TRUE);
    set(myData->btn_power_two, MUIA_Disabled, TRUE);
    set(myData->btn_power_y, MUIA_Disabled, TRUE);
    set(myData->btn_percent, MUIA_Disabled, TRUE);
    set(myData->btn_inverse, MUIA_Disabled, TRUE);
    set(myData->btn_invert_sign, MUIA_Disabled, TRUE);
    set(myData->btn_pi, MUIA_Disabled, TRUE);

    return (IPTR) TRUE;
}

/**
    Change sign
    @param [cl, obj] pointer to the class private data struct
**/
static IPTR changeSign(struct IClass *cl, Object *obj)
{
    struct eData *myData = INST_DATA(cl, obj);
    STRPTR _display    = myData->displayStr;

    if (10 == myData->cs->base)
    {
        DOUBLE dest = 0;
        if (TRUE != DoMethod(obj, MUIM_getDisplay, _display, &dest, myData->cs->base))
        {
            jerror("[changeSign] Error converting string!");
        }
        jdebug("[changeSign] Changing %f to %f", dest, dest*-1);
        dest = dest * -1;
        STRPTR tmpstr = AllocVec(50*sizeof(STRPTR), MEMF_CLEAR);
        // printf("[changeSign] Allocated %d at 0x%p\n", sizeof(STRPTR)*50, tmpstr);
        snprintf((char *)tmpstr, 100, "%f", dest);

        removeTrailingZeroes(&tmpstr);
        /*
            FIXME
            Unfortunately this the ugly exception to the MUIM_setDisplay rule (centralize
            text processing in that method right before printing) on display.

            The reason is that a) I'm not printing a result (i.e. isInsertMode is TRUE)
            and I can't either set the doProcess flag to TRUE because that's reserved to
            final results.
        */

        jdebug("[changeSign] and removing the comma!");
        removeComma(&tmpstr);
        // jdebug("[changeSign] Will write %s", tmpstr);

        DoMethod(obj, MUIM_setDisplay, tmpstr, FALSE);

        if (tmpstr)
        {
            FreeVec(tmpstr); tmpstr = NULL;
        }
    }

    return (IPTR) TRUE;
}

/**
    Calc memory management
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR memMgmt(struct IClass *cl, Object *obj, struct MUIP_JCALC_memMsg *msg)
{
    struct eData *myData    = INST_DATA(cl, obj);
    DOUBLE dest                = 0;
// #warning replaced with displayStr
    // STRPTR tmpstr            = (STRPTR)XGET(myData->display, MUIA_Text_Contents);

    // Possible memory issue here? Isn't necessary to allocate memory for tmpstr?
    STRPTR tmpstr    = myData->displayStr;

    if (TRUE != DoMethod(obj, MUIM_getDisplay, tmpstr, &dest, myData->cs->base))
    {
        jerror("[memMgmt] Error parsing display!");
    }

    switch(msg->operator)
    {
        case EVT_BTN_MEMADD:
            myData->mem    += dest;
            break;
        case EVT_BTN_MEMSUB:
            myData->mem    -= dest;
            break;
        case EVT_BTN_MEMRECALL:
            snprintf((char *)tmpstr, 100, "%f", myData->mem);
            jdebug("[memMgmt] Invoking MUIM_setDisplay: TRUE");
            DoMethod(obj, MUIM_setDisplay, tmpstr, TRUE);

            // setting to insert mode because I'm actually writing a number
            // to the display.
            myData->mode    = INSMODE;

            // Also saving the operator (otherwise lost)
            myData->cs->op    = myData->operator;

            break;
        case EVT_BTN_MEMCLEAR:
            myData->mem    = 0;
            break;
        case EVT_BTN_MEMSAVE:
            myData->mem    = dest;
            break;
    }

    return (IPTR) TRUE;
}

/* HOOKS (Private methods) */

/**
    Allocate for the list
    @param entry new list entry
    @param p new list content
**/
HOOKPROTONHNO(listConstructor, struct entry *, struct entry *p)
{
    // jwarning("[listConstructor] with str=0x%p", str);
    struct entry *e;
    if ((e = AllocVec(sizeof(struct entry), MEMF_CLEAR)))
    {
        // printf("[listConstructor A] Allocated %d at 0x%p\n", sizeof(struct entry), e);
        if (NULL != (e->line = AllocVec(sizeof(char)*HISTORY_LINE_LENGTH, MEMF_CLEAR)))
        {
            // printf("[listConstructor B] Allocated %d at 0x%p\n", sizeof(char)*HISTORY_LINE_LENGTH, e->line);
            if (NULL != (e->op = AllocVec(sizeof(char)*2, MEMF_CLEAR)))
            {
                // printf("[listConstructor C] Allocated %d at 0x%p\n", sizeof(char)*2, e->op);
                snprintf((char *)e->line, 100, "%s", p->line);
                snprintf((char *)e->op, 2, "%s", p->op);
                return e;
            }
        }
        // FIXME why this???
        // I should free "e"!
        // FreeVec(p);
        FreeVec(e); e = NULL;
    }
    else
        jerror("Allocation failed!");


    return NULL;

}
MakeHook(listConstructorHook, listConstructor);

/**
    Destroy the list entry
    @param entry list entry to be destroyed
**/
HOOKPROTONHNO(listDestructor, void, struct entry *e)
{
//    jwarning("[listDestructor]");
    if(e->line)
    {
        FreeVec(e->line); e->line = NULL;
    }

    if(e->op)
    {
        FreeVec(e->op); e->op = NULL;
    }

    if (e)
    {
        FreeVec(e); e = NULL;
    }
}
MakeHook(listDestructorHook, listDestructor);

/**
    Add an item to the list
    @param array list items
    @param e list entry
**/
HOOKPROTONH(listDisplay, void, STRPTR *array, struct entry *e)
{
// #warning CRASH! If any kind of print is invoked here. Any (printf, j*)

    // jwarning("[listDisplay] Got in with '%s' and '%s' for 0x%p!", e->line, e->op, array);
     *array++    = (STRPTR)e->line;
     *array        = (STRPTR)e->op;

    /* MUI does not allow a single column right-aligned */
    /* Padding the column doesn't solve because if I resize the window I cannot
     * listview content as well.
     */
#if 0
    /* Trying to work around a MUI mischief when alignining to the right */
    int len = HISTORY_LINE_LENGTH - strlen((char *)e->line);
    printf(">>> %d = (HLL)%d - %d\n", len, HISTORY_LINE_LENGTH, strlen((char *)e->line));
    char spacer[len]; spacer[0] = '\0';
    int i;
    for (i = 0; i < len; i++)
        spacer[i] = ' ';
    spacer[i] = '\0';
    STRPTR _spacer = StrDup((STRPTR)spacer);

    STRPTR _line = AllocVec(sizeof(STRPTR) * len, MEMF_ANY);
    snprintf((char *)_line, strlen((char *)_spacer), (char *)_spacer);
    strncat((char *)_line, (char *)e->line, strlen((char *)e->line));
    *array = _line;
    // FreeVec(_spacer);
#endif

    // jwarning("[listDisplay] Finished!");
}
MakeHook(listDisplayHook, listDisplay);

/* End Hooks */

IPTR DoSuperNew (struct IClass *cl, Object *obj, IPTR tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}

/**
    OM_NEW overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mJcalc_New (struct IClass *cl, Object *obj, struct opSet *msg)
{
    jdebug("[mJcalc_New] Init");

    // Parsing class init parameters
    struct TagItem *tags    = ((struct opSet *)msg)->ops_AttrList;
    BOOL toggleSaveAsCsv     = GetTagData(MUIA_JCALC_SaveAsCSV, (IPTR)FALSE, tags);
    BOOL toggleHistoryPanel    = GetTagData(MUIA_JCALC_toggleHistoryPanel, (IPTR)FALSE, tags);
    // BOOL setBase            = GetTagData(MUIA_JCALC_SetBase, (IPTR)FALSE, tags);

    jdebug("[mJcalc_New] Will instance class with: MUIA_JCALC_SaveAsCSV=%d, MUIA_JCALC_toggleHistoryPanel=%d",
        toggleSaveAsCsv, toggleHistoryPanel);

    // Object *aboutApp;

    struct eData temp;
       memset(&temp, 0, sizeof (temp));

       /* Init my private data struct */

       /* "Display" widget */

       struct MUI_CustomClass *displayClass = NULL;

    if (!(displayClass = initDisplayClass()))
        jerror("Could not create Jcalc custom class.");

    struct calcStack *cs = NULL;
    if (NULL == (cs = malloc(sizeof(struct calcStack))))
    {
        jerror("[mJcalc_New] Error allocating memory");
        //TODO freeList(cs);
        // freeList(stack);
        return EXIT_FAILURE;
    }
    // printf("[OM_NEW] Allocated %d 0x%p (malloc)\n", sizeof(struct calcStack), cs);

    temp.toggleSaveAsCsv    = toggleSaveAsCsv;
    temp.toggleHistoryPanel = toggleHistoryPanel;
    // temp.setBase            = setBase;

    /* init calc components */
    cs->op        = -1; // 0 is a valid value
    cs->n1        = .0f;
    cs->n2        = .0f;
    cs->res        = .0f;
    cs->base    = 10;

    /* Initializing the struct that will be passed to the HOOK function */
    // temp.display    = (Object *)"";
    temp.displayStr    = (STRPTR)"0";
    temp.cs            = cs;
    temp.mode        = 0;
    temp.operator    = 0;
    temp.mem        = 0; /* will store MEM related value */

    /* TODO
        this could be the switch to start jCalc in BASICMODE/PRGMODE
        (need methods and all...)
    */
    temp.interfaceMode = BASICMODE;

    /*
     * I'm "forced" to allocate memory now otherwise
     * I can't copy the starting address in 'head' and I lose the
     * possibility to walk the list with printStack.
     * Is there an alternate method?
    */

    // struct reel *stack    = NULL;
    // struct reel *head    = stack;
    if (NULL == (temp.history = malloc(sizeof(struct reel))))
    {
        D(bug("Malloc failed for stack, bail out!\n"));
        return EXIT_FAILURE;
    }
    // printf("[OM_NEW] Allocated %d 0x%p (malloc)\n", sizeof(struct reel), temp.history);
    temp.history->next    = NULL;
    temp.history->value    = .0f;
    temp.history->op    = -1;
    jdebug("[OM_NEW] History reel allocated @0x%p (-> 0x%p)", temp.history, temp.history->next);
    // printf("[OM_NEW] Allocated %d 0x%p\n", sizeof(struct reel), temp.history);

    /* Event handling init done in MUIM_Setup - why? */

    /* History panel container */
    temp.historyPanel =  MUI_NewObject(MUIC_Group, // Vgroup
        MUIA_Group_Horiz, FALSE,
        MUIA_ObjectID, "IdGrpHistoryPanel",
#if GUI_DEBUG
        GroupFrame,
        MUIA_FrameTitle, "IdGrpHistoryPanel",
#endif
        Child, ScrollgroupObject,
            MUIA_ObjectID, "IdGrpScrollHistoryPanel",
#if GUI_DEBUG
            GroupFrame,
            MUIA_FrameTitle, "IdGrpScrollHistoryPanel",
#endif
            /* from ./test/Zune/test.c */
            MUIA_Scrollgroup_Contents, MUI_NewObject(MUIC_Group, // VGroupV,
                MUIA_Group_Horiz, FALSE,
                Child, NListviewObject,
                    MUIA_NListview_NList, temp.historyList = NListObject,
                    MUIA_NList_Format, "MIW=95 W=200 D=1 P=\33r,",
                    MUIA_ContextMenu, NULL,
                    InputListFrame,
                        MUIA_NList_ConstructHook, (IPTR)&listConstructorHook,
                        MUIA_NList_DestructHook, (IPTR)&listDestructorHook,
                        MUIA_NList_DisplayHook, (IPTR)&listDisplayHook,
                    TAG_DONE),
                TAG_DONE),
                Child, MUI_NewObject(MUIC_Group, // HGroup,
                    MUIA_Group_Horiz, TRUE,
                    Child, temp.btn_history_save = MUI_MakeObject(MUIO_Button,"_Save As..."),
                    Child, temp.btn_history_clear = MUI_MakeObject(MUIO_Button,"_Clear"),
                    TAG_DONE),
               TAG_DONE),
        TAG_DONE),

    TAG_DONE); /* End of history panel */


    /* grp of base conversions and other functions */
    temp.grp_mode = MUI_NewObject(MUIC_Group,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Spacing, 5,
        MUIA_Group_SameSize, TRUE,
#if GUI_DEBUG
        GroupFrame,
        MUIA_FrameTitle, "Bases",
#endif
        Child, temp.btn_bin            = MakeButtonImmediate((UBYTE *)"BIN", (STRPTR)"IdBtnBin", 'B'),
        Child, temp.btn_dec            = MakeButtonImmediate((UBYTE *)"DEC", (STRPTR)"IdBtnDec", 'D'),
        Child, temp.btn_hex            = MakeButtonImmediate((UBYTE *)"HEX", (STRPTR)"IdBtnHex", 'H'),
        Child, temp.btn_oct            = MakeButtonImmediate((UBYTE *)"OCT", (STRPTR)"IdBtnOct", 'O'),
        Child, temp.btn_power_y        = MakeButton((UBYTE *)"x^y", 0),
        Child, temp.btn_power_two    = MakeButton((UBYTE *)"x^2", 0),
    TAG_DONE); /* end group of base conversions and other things */

    /* HEX figures */
    temp.grp_hex = MUI_NewObject(MUIC_Group,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Spacing, 5,
        MUIA_Group_SameSize, TRUE,
        MUIA_ObjectID, "IdGrpHex",
#if GUI_DEBUG
        GroupFrame,
        MUIA_FrameTitle, "HEX",
#endif
        Child, temp.btn_a   = MakeButton((UBYTE *)"A", 'a'),
        Child, temp.btn_b   = MakeButton((UBYTE *)"B", 'b'),
        Child, temp.btn_c   = MakeButton((UBYTE *)"C", 'c'),

        Child, temp.btn_d   = MakeButton((UBYTE *)"D", 'd'),
        Child, temp.btn_e   = MakeButton((UBYTE *)"E", 'e'),
        Child, temp.btn_f   = MakeButton((UBYTE *)"F", 'f'),
    TAG_DONE); /* end of grp_hex */


    temp.grp_scientific = MUI_NewObject(MUIC_Group,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Columns, 3, // 3, compreso il separatore verticale
        MUIA_Group_Spacing, 5,
        MUIA_Group_SameSize, TRUE,
        MUIA_ObjectID, "IdGrpSci",
#if GUI_DEBUG
        GroupFrame,
        MUIA_FrameTitle, "IdGrpSci",
#endif
        Child, temp.grp_mode,
        Child, temp.grp_hex,
        Child, MUI_MakeObject(MUIO_VBar,10),
    TAG_DONE); /* end of grp_scientific */


       if ((obj = (Object *) DoSuperNew(cl, obj,
           MUIA_ObjectID, "IdGrpRoot",
#if GUI_DEBUG
        GroupFrame,
        MUIA_FrameTitle, "IdGrpRoot",
#endif
           /* Here is the place for the history panel container */
           /* Disabled by default, enabled on user request */
           // Child, temp.historyPanel,

        /* Calculator display */
        /* old text display
        Child, temp.display = TextObject,
            TextFrame,
            MUIA_Background, MUII_TextBack,
            MUIA_Text_PreParse, "\33r",
            MUIA_Text_Contents, "0",
        End,
        */

        Child, (IPTR)(temp.displayDraw = NewObject(displayClass->mcc_Class, NULL,
                TAG_DONE)),

        /* grp_main (main container, two columns) */
        Child, temp.grp_main = HGroup,
            MUIA_Group_Columns, 2, /* DO NOT TOUCH! */
            MUIA_Group_Spacing, 5,
            MUIA_Group_SameHeight, TRUE,
            MUIA_ObjectID, "IdGrpMain",
#if GUI_DEBUG
            GroupFrame,
            MUIA_FrameTitle, "IdGrpMain",
#endif

               /* Here is the place for the scientific/programmer functions */
               /* Disabled by default, enabled on user request */
            // Child, temp.grp_scientific,

            /* grp_main */
            Child, temp.grp_base = VGroup,
                MUIA_Group_Columns, 5,
                MUIA_Group_Spacing, 5,
                MUIA_Group_SameSize, TRUE,
                MUIA_ObjectID, "IdGrpBase",
#if GUI_DEBUG
                GroupFrame,
                MUIA_FrameTitle, "IdGrpBase",
#endif

                Child, temp.btn_pi        = MakeButton((UBYTE *)"Pi", 'p'),
                Child, temp.btn_history    = MakeButton((UBYTE *)"HST", 'h'),
                Child, temp.btn_ca        = MakeButton((UBYTE *)"CA", 0),
                Child, temp.btn_ce        = MakeButton((UBYTE *)"CE", 0),
                Child, temp.btn_bs        = MakeButton((UBYTE *)"BS", 0),

                Child, temp.btn_mem_add    = MakeButton((UBYTE *)"M+", 0),
                Child, temp.btn_mem_sub    = MakeButton((UBYTE *)"M-", 0),
                Child, temp.btn_mem_mr    = MakeButton((UBYTE *)"MR", 0),
                Child, temp.btn_mem_mc    = MakeButton((UBYTE *)"MC", 0),
                Child, temp.btn_mem_ms    = MakeButton((UBYTE *)"MS", 0),

                Child, temp.btn_seven    = MakeButton((UBYTE *)"7", '7'),
                Child, temp.btn_eight    = MakeButton((UBYTE *)"8", '8'),
                Child, temp.btn_nine    = MakeButton((UBYTE *)"9", '9'),
                Child, temp.btn_divide    = MakeButton((UBYTE *)"/", '/'), // Key: 47
                Child, temp.btn_percent    = MakeButton((UBYTE *)"%", '%'),

                Child, temp.btn_four        = MakeButton((UBYTE *)"4", '4'),
                Child, temp.btn_five        = MakeButton((UBYTE *)"5", '5'),
                Child, temp.btn_six            = MakeButton((UBYTE *)"6", '6'),
                Child, temp.btn_multiply    = MakeButton((UBYTE *)"*", '*'), // Key: 42
                Child, temp.btn_sqrt        = MakeButton((UBYTE *)"SQRT", 0),

                Child, temp.btn_one            = MakeButton((UBYTE *)"1", '1'),
                Child, temp.btn_two            = MakeButton((UBYTE *)"2", '2'),
                Child, temp.btn_three        = MakeButton((UBYTE *)"3", '3'),
                Child, temp.btn_minus        = MakeButton((UBYTE *)"-", '-'), // Key: 45
                Child, temp.btn_invert_sign    = MakeButton((UBYTE *)"+/-", 0),

                Child, temp.btn_zero    = MakeButton((UBYTE *)"0", '0'),
                Child, temp.btn_dot        = MakeButton((UBYTE *)comma, '.'), // Key: 46
                Child, temp.btn_equals    = MakeButton((UBYTE *)"=", '\r'),  // Key: [return] or use 13
                Child, temp.btn_plus    = MakeButton((UBYTE *)"+", '+'), // Key: 43
                Child, temp.btn_inverse    = MakeButton((UBYTE *)"1/x", 0),

            End, /* end "IdGrpBase" */

            /* This is just to visually balance the above mentioned placeholder */
            // Child, MUI_MakeObject(MUIO_VBar,0),

        End, /* end "IdGrpMain" */

           TAG_MORE, (IPTR) msg->ops_AttrList))
       )
       {
        struct eData *data = INST_DATA(cl, obj);
        memcpy(data, &temp, sizeof (*data));

        /* Setting initial GUI values */

        // jdebug("[mJcalc_New] displayStr is '%s' 0x%p", data->displayStr, data->displayStr);

        // Base is decimal
        DoMethod(obj, MUIM_setBase, DECBASE);
        // SetAttrs(menu_dec, MUIA_Menuitem_Checked, TRUE, TAG_DONE);

        // Mode is BASIC
        DoMethod(obj, MUIM_setMode, BASICMODE);

        // Initial display value is 0 - is it needed? In theory NO! (FIXME)
        // jdebug("[mJcalc_New] displayStr is '%s' 0x%p", data->displayStr, data->displayStr);
        // DoMethod(obj, MUIM_setDisplay, "0", FALSE);
        // jdebug("[OM_NEW] displayStr at '%s' 0x%p", data->displayStr, data->displayStr);

           /* Listeners */

           /* History buttons hooks */

           DoMethod(data->btn_history_save, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_historyListSaveAs, data->toggleSaveAsCsv);

        DoMethod(data->btn_history_clear, MUIM_Notify, MUIA_Pressed, FALSE,
                obj, 1,
                MUIM_List_Clear);

           /* Numbers */

        DoMethod(data->btn_zero, MUIM_Notify, MUIA_Pressed, FALSE,
                obj, 2,
                MUIM_addNumber, 0);

           DoMethod(data->btn_one, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 1);

           DoMethod(data->btn_two, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 2);

           DoMethod(data->btn_three, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 3);

           DoMethod(data->btn_four, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 4);

           DoMethod(data->btn_five, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 5);

           DoMethod(data->btn_six, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 6);

           DoMethod(data->btn_seven, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 7);

           DoMethod(data->btn_eight, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 8);

           DoMethod(data->btn_nine, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 9);

           DoMethod(data->btn_a, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 'A');

           DoMethod(data->btn_b, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 'B');

           DoMethod(data->btn_c, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 4,
                   MUIM_addNumber, 'C');

           DoMethod(data->btn_d, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 'D');

           DoMethod(data->btn_e, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 'E');

           DoMethod(data->btn_f, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addNumber, 'F');

           /* Constants */

           DoMethod(data->btn_pi, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_addConstant, 'P');

           /* Operators */

        DoMethod(data->btn_plus, MUIM_Notify, MUIA_Pressed, FALSE,
                obj, 2,
                MUIM_setOperator, EVT_BTN_PLUS);

        DoMethod(data->btn_minus, MUIM_Notify, MUIA_Pressed, FALSE,
                obj, 2,
                MUIM_setOperator, EVT_BTN_MINUS);

        DoMethod(data->btn_divide, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_setOperator, EVT_BTN_DIVIDE);

           DoMethod(data->btn_multiply, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_setOperator, EVT_BTN_MULTIPLY);

           DoMethod(data->btn_equals, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_doCalc, TRUE);

           DoMethod(data->btn_percent, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_setOperator, EVT_BTN_PERCENT);

           DoMethod(data->btn_power_y, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 4,
                   MUIM_setOperator, EVT_BTN_POWERY);

           DoMethod(data->btn_dot, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 1,
                   MUIM_addDot);

           /* Immediate operations (one operand involved) */

           DoMethod(data->btn_sqrt, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_setOperatorImmediate, EVT_BTN_SQRT);

           DoMethod(data->btn_power_two, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_setOperatorImmediate, EVT_BTN_SQUARE);

           DoMethod(data->btn_inverse, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_setOperatorImmediate, EVT_BTN_INVERSE);

           /* Memory functions */

           DoMethod(data->btn_mem_add, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 1,
                   MUIM_memMgmt, EVT_BTN_MEMADD);

           DoMethod(data->btn_mem_sub, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 1,
                   MUIM_memMgmt,EVT_BTN_MEMSUB);

           DoMethod(data->btn_mem_mr, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 1,
                   MUIM_memMgmt, EVT_BTN_MEMRECALL);

           DoMethod(data->btn_mem_mc, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 1,
                   MUIM_memMgmt, EVT_BTN_MEMCLEAR);

           DoMethod(data->btn_mem_ms, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 1,
                   MUIM_memMgmt, EVT_BTN_MEMSAVE);

           /* Everything else */

           DoMethod(data->btn_ce, MUIM_Notify, MUIA_Pressed, FALSE,
                   obj, 2,
                   MUIM_clearDisplay, DISPLAY_CE);

           DoMethod(data->btn_ca, MUIM_Notify, MUIA_Selected, TRUE,
                   obj, 2,
                   MUIM_clearDisplay, DISPLAY_CA);

           DoMethod(data->btn_bs, MUIM_Notify, MUIA_Selected, TRUE,
                 obj, 3,
                 MUIM_clearOneChar);

        DoMethod(data->btn_bin, MUIM_Notify, MUIA_Selected, TRUE,
                obj, 2,
                MUIM_setBase, BINBASE);

           DoMethod(data->btn_dec, MUIM_Notify, MUIA_Selected, TRUE,
                   obj, 2,
                   MUIM_setBase, DECBASE);

           DoMethod(data->btn_hex, MUIM_Notify, MUIA_Selected, TRUE,
                   obj, 2,
                   MUIM_setBase, HEXBASE);

           DoMethod(data->btn_oct, MUIM_Notify, MUIA_Selected, TRUE,
                   obj, 2,
                   MUIM_setBase, OCTBASE);

           DoMethod(data->btn_invert_sign, MUIM_Notify, MUIA_Selected, TRUE,
                   obj, 1,
                   MUIM_changeSign);

           DoMethod(data->btn_history, MUIM_Notify, MUIA_Selected, TRUE,
                   obj, 1,
                   MUIM_toggleHistoryPanel);
       }

    return (IPTR)obj;
}

/**
    OM_SET overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mJcalc_Set (struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct eData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for (tstate = msg->ops_AttrList; (tag = NextTagItem((TAGITEM)&tstate)); )
    {
        switch(tag->ti_Tag)
        {
            case MUIA_JCALC_toggleHistoryPanel:
                jdebug("[mJcalc_Set] MUIA_JCALC_toggleHistoryPanel");
                if (data->toggleHistoryPanel == (LONG)tag->ti_Data)
                    tag->ti_Tag = TAG_IGNORE;
                else
                  data->toggleHistoryPanel = (LONG)tag->ti_Data;
                break;

            case MUIA_JCALC_SaveAsCSV:
                jdebug("[mJcalc_Set] MUIA_JCALC_SaveAsCSV");
                if (data->toggleSaveAsCsv == (LONG)tag->ti_Data)
                    tag->ti_Tag = TAG_IGNORE;
                else
                  data->toggleSaveAsCsv    = (LONG)tag->ti_Data;
                break;

            case MUIA_JCALC_SetBase:
                jdebug("[mJcalc_Set] MUIA_JCALC_SetBase");
                if (data->setBase == (LONG)tag->ti_Data)
                    tag->ti_Tag = TAG_IGNORE;
                else
                  data->setBase    = (LONG)tag->ti_Data;
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/**
    OM_GET overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mJcalc_Get (struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct eData *data = INST_DATA(cl, obj);
    switch(msg->opg_AttrID)
    {
        case MUIA_JCALC_toggleHistoryPanel:
            *msg->opg_Storage = (LONG)data->toggleHistoryPanel;
            return TRUE;

        case MUIA_JCALC_SaveAsCSV:
            *msg->opg_Storage = (LONG)data->toggleSaveAsCsv;
            return TRUE;

        case MUIA_JCALC_SetBase:
            *msg->opg_Storage = (LONG)data->setBase;
            return TRUE;
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}


/**
    OM_SETUP overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mJcalc_Setup (struct IClass *cl, Object *obj, Msg msg)
{
    jdebug("[MUIM_SETUP]");
    struct eData *data = INST_DATA(cl, obj);

    if (!DoSuperMethodA(cl, obj, msg))
        return (FALSE);

    memset(&data->ehnode, 0, sizeof (data->ehnode));
    data->ehnode.ehn_Object = obj;
    data->ehnode.ehn_Class     = cl;
    data->ehnode.ehn_Events = IDCMP_VANILLAKEY | IDCMP_RAWKEY;

    // Hide the historyPanel group
    // temp.historyPanel

    DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->ehnode);

    return (IPTR)TRUE;
}

/**
    OM_CLEANUP overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mJcalc_Cleanup(struct IClass *cl, Object * obj, Msg msg)
{
    jdebug("[MUIM_CLEANUP]");
    struct eData *data = INST_DATA(cl, obj);
    DoMethod(_win(obj), MUIM_Window_RemEventHandler, (IPTR) & data->ehnode);
    return DoSuperMethodA(cl, obj, msg);
}

/**
    OM_ASKMINMAX overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mJcalc_AskMinMax(struct IClass *cl UNUSED, Object * obj, struct MUIP_AskMinMax *msg)
{
    // Have GCC just shut up!
    // jdebug("[mJcalc_AskMinMax] Unused params cl=0x%p", cl);

    jdebug("[MUIM_ASKMINMAX]");

    DoSuperMethodA(cl, obj, (Msg)msg);

    jdebug("[MUIM_ASKMINMAX] Object is 0x%p (%s)", obj,
            (STRPTR)XGET(obj, MUIA_ObjectID));

    jdebug("[MUIM_ASKMINMAX] Height: %d, %d, %d",
            msg->MinMaxInfo->MinHeight,
            msg->MinMaxInfo->DefHeight,
            msg->MinMaxInfo->MaxHeight);

    jdebug("[MUIM_ASKMINMAX] Width: %d, %d, %d",
            msg->MinMaxInfo->MinWidth,
            msg->MinMaxInfo->DefWidth,
            msg->MinMaxInfo->MaxWidth);

    /*
     * Upon size change of the whole GUI, set +35% of min height.
     * I'm trying to keep a consistent "look" by having the various items
     * more or less of the same size even when adding/removing a group of objects
     */
    {
        LONG len = abs(((float)msg->MinMaxInfo->MinHeight/100)*135);
        msg->MinMaxInfo->MinHeight = len;
        msg->MinMaxInfo->DefHeight = len;

        // Not this! Or height will be fixed!
        // msg->MinMaxInfo->MaxHeight = len;
    }

    return (IPTR) TRUE;
}

// static IPTR mJcalc_Draw(struct IClass *cl, Object * obj, struct MUIP_Draw *msg)
// {
//     // Invoking this method without actually doing anything
//     // makes the application crash

//     IPTR ret = DoSuperMethodA(cl, obj, (Msg)msg);

//     if (msg->flags & MADF_DRAWUPDATE) /* called from our input method */
//         jwarning("[MUIM_Draw] MADF_DRAWUPDATE");
//     else if (msg->flags & MADF_DRAWOBJECT) /* redraw ourselves completely */
//         jwarning("[MUIM_Draw] MADF_DRAWOBJECT");

//     return ret;
// }

/**
    Insert a new item in the history reel (interface)
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR mInsert(struct IClass *cl, Object *obj, struct MUIP_JCALC_ListEntry *msg)
{
    // Don't print if the panel is closed
    if (XGET(obj, MUIA_JCALC_toggleHistoryPanel) == FALSE)
        return (IPTR)TRUE;

    struct eData *myData    = INST_DATA(cl, obj);
    DOUBLE number            = msg->number;
    BYTE op                 = msg->op;
    char ch;
    STRPTR historyTxt;
    struct entry *historyLine;

    STRPTR str = AllocVec(sizeof(STRPTR)*HISTORY_LINE_LENGTH, MEMF_CLEAR);
    // printf("[mInsert] Allocated %d at 0x%p\n", sizeof(STRPTR)*HISTORY_LINE_LENGTH, str);
    if (str == NULL)
    {
        jerror("[mInsert] Failed allocation!");
        fail((CONST_STRPTR)"JCALC", 101, (CONST_STRPTR)"[mInsert] Failed memory allocation!\n");
    }
    // jdebug("XXX str before: '%s' (0x%p)", str, str);

    /* FIXME
     *     why am I sending a -1 (in case of a line break) and I always read it here as -3?
     * */
    jdebug("[mInsert] Operator is %d", op);
    if (op != -3)
    {
        // manually remove zeroes in excess (can't do with printf formatting)
        snprintf((char *)str, HISTORY_LINE_LENGTH, "%f", number);
        removeTrailingZeroes(&str);
        // jdebug("XXX str after removeTrailingZeroes: '%s' (0x%p)", str, str);

        /* remove the comma if needed */
        removeComma(&str);

        max_decimals(&str);
        // jdebug("XXX str after max_decimals: '%s' (0x%p)", str, str);

        // Only convert display if the new base required is different from the old one
        if (10 != myData->cs->base)
            convertDisplay(&str, 10, &myData->cs->base);
        // jdebug("XXX str after convertDisplay: '%s' (0x%p)", str, str);

        // convert comma according to locale
        jdebug("[mInsert] Convert comma");
        convertComma(&str);
        // jdebug("XXX str convertComma: '%s' (0x%p)", str, str);

        historyTxt = AllocVec(sizeof(STRPTR)*HISTORY_LINE_LENGTH, MEMF_CLEAR);
        // printf("[mInsert] Allocated %d at 0x%p\n", sizeof(STRPTR)*HISTORY_LINE_LENGTH, historyTxt);
        if (historyTxt == NULL)
        {
            jerror("[mInsert] Failed allocation!");
            fail((CONST_STRPTR)"JCALC", 101, (CONST_STRPTR)"[mInsert] Failed memory allocation!\n");
        }

        if (op != -1)
            ch = char_operators[op];
        else
            ch = ' ';

        snprintf((char *)historyTxt, HISTORY_LINE_LENGTH, "%s", str);
    }
    else
    {
        historyTxt = AllocVec(sizeof(STRPTR)*16, MEMF_CLEAR);
        // printf("[mInsert] Allocated %d at 0x%p\n", sizeof(STRPTR)*16, historyTxt);
        if (historyTxt == NULL)
        {
            jerror("[mInsert] Failed allocation!");
            fail((CONST_STRPTR)"JCALC", 101, (CONST_STRPTR)"[mInsert] Failed memory allocation!\n");
        }
        snprintf((char *)historyTxt, strlen(LINE_SEPARATOR), "%s", LINE_SEPARATOR);
        ch = ' ';
    }

    historyLine = AllocVec(sizeof(struct entry), MEMF_CLEAR);
    // printf("[mInsert] Allocated %d at 0x%p\n", sizeof(struct entry), historyLine);
    if (historyLine == NULL)
    {
        jerror("[mInsert] Failed allocation!");
        fail((CONST_STRPTR)"JCALC", 101, (CONST_STRPTR)"[mInsert] Failed memory allocation!\n");
    }

    historyLine->line = StrDup((STRPTR)historyTxt);
    char _op[2];
    _op[0] = ch;
    _op[1] = '\0';
    historyLine->op = StrDup((STRPTR)_op);

    DoMethod(myData->historyList, MUIM_NList_Insert, &historyLine, 1, MUIV_List_Insert_Bottom);
    SetAttrs(myData->historyList, MUIA_List_Active, MUIV_List_Active_Bottom, TAG_DONE);
    jdebug("[mInsert] Added line to history panel: '%s'", historyTxt);

    if(str)
    {
        FreeVec(str); str = NULL;
    }

    if (historyTxt)
    {
        FreeVec(historyTxt); historyTxt = NULL;
    }

    if(historyLine)
    {
        if (historyLine->op)
        {
            FreeVec(historyLine->op); historyLine->op = NULL;
        }
        if (historyLine->line)
        {
            FreeVec(historyLine->line); historyLine->line = NULL;
        }

        FreeVec(historyLine); historyLine = NULL;
    }

    return (IPTR) TRUE;
}

/**
    Get the latest result from the display (interface)
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR doGetLastResult(struct IClass *cl, Object *obj, struct MUIP_JCALC_AREXX_resultMsg *msg)
{
    struct eData *myData = INST_DATA(cl,obj);
    STRPTR currdisplay    = myData->displayStr;
    jdebug("[doGetLastResult] starts with(0x%p): '%s'", currdisplay, currdisplay);
    DOUBLE num;
    IPTR ris = FALSE;
    STRPTR tmpstr;

    if (0 == strcmp((char *)currdisplay, "+inf"))
    {
        char _b[4+1] = {'+','i','n','f','\0'};
        strcpy((char *)msg->value, (char *)_b);
        jdebug("[doGetLastResult] bail out immediately due to error string '%s'", msg->value);
        ris = TRUE;
        return ris;
    }

    if (TRUE != DoMethod(obj, MUIM_getDisplay, currdisplay, &num, myData->cs->base))
    {
        jerror("[doGetLastResult] Error retrieving result from display '%s'", currdisplay);
    }
    else
    {
        jdebug("[doGetLastResult] got display '%f'", num);

        tmpstr = AllocVec(sizeof(STRPTR) * 100, MEMF_CLEAR);
        // printf("[doGetLastResult] Allocated %d at 0x%p\n", sizeof(STRPTR)*100, tmpstr);
        snprintf((char *)tmpstr, 100, "%f", num);

        max_decimals(&tmpstr);
        // jdebug("[doGetLastResult] >>> after maxdecimals tmpstr is (0x%p): '%s'", tmpstr, tmpstr);

        // Only convert display is new base required is different from old one
        if (10 != myData->cs->base)
            convertDisplay(&tmpstr, 10, &myData->cs->base);
        // jdebug("[doGetLastResult] >>> after convertDisplay tmpstr is (0x%p): '%s'", tmpstr, tmpstr);

        convertComma(&tmpstr);
        // jdebug("[doGetLastResult] >>> after convertComma tmpstr is (0x%p): '%s'", tmpstr, tmpstr);

        removeTrailingZeroes(&tmpstr);
        // jdebug("[doGetLastResult] >>> after removeTrailingZeroes tmpstr is (0x%p): '%s'", tmpstr, tmpstr);

        // Remove the comma, if needed
        int len = strlen((char *)tmpstr);
        char buf[len+1]; buf[0] = '\0';
        snprintf(buf, len+1, "%s", tmpstr);
        if (buf[len-1] == '.' || buf[len-1] == ',')
            buf[len-1] = '\0';

        // memset((char *)tmpstr, 0, strlen((char *)tmpstr));
        // tmpstr = (STRPTR)StrDup((STRPTR)buf);
        // jdebug("[doGetLastResult] >>> after removing comma tmpstr is (0x%p): '%s'", tmpstr, tmpstr);

        // memset((char *)msg->value, 0, strlen((char *)msg->value));

        // int l = strlen((char *)msg->value);
        // printf(">>>> parseCmdString buf:'%s'\n", buf);
        strcpy((char *)msg->value, (char *)buf);
        // msg->value[l] = '\0';
        // msg->value = StrDup((STRPTR)buf);
        // printf(">>>> parseCmdString snprintf result:'%s'\n", msg->value);

        // msg->value = StrDup((STRPTR)tmpstr);
        jdebug("[doGetLastResult] returning msg->value:0x%p='%s'",
            msg->value, msg->value);

        if (tmpstr)
        {
            FreeVec(tmpstr); tmpstr = NULL;
        }

        ris = TRUE;
    }
    return (IPTR) ris;
}

// AslRefreshFunc
// If user sets simplerefresh on his window, this hook is able to redraw a window
// in sleep while an asl requester is opened...
HOOKPROTONH(AslRefreshFunc, void, struct FileRequester *req, struct IntuiMessage *imsg)
{
     if (imsg->Class == IDCMP_REFRESHWINDOW)
         DoMethod(req->fr_UserData, MUIM_Application_CheckRefresh);
}
MakeStaticHook(AslRefreshHook,AslRefreshFunc);

/**
    Create filename requester
    @param win The window
    @param title Window title
    @param save Save flag
    @param path Path to save file into
**/
STRPTR getFilename(Object *win, STRPTR title, BOOL save, STRPTR *path)
{
    struct FileRequester *req = NULL;
    int l = 0;
    BOOL done = FALSE;
    STRPTR _path[ASL_BUFFER_LEN];
    struct Window w;
    CONST_STRPTR ASLTitle = title;

    Object *app = (Object *)XGET(win, MUIA_ApplicationObject);
    set(app, MUIA_Application_Sleep, TRUE);
    get(win,MUIA_Window_Window,&w);

    req = MUI_AllocAslRequest(ASL_FileRequest, NULL);

    while (TRUE)
    {
        if (req)
        {
            IPTR aslRes = (IPTR) FALSE;
            struct TagItem whatPath;

            whatPath.ti_Tag = ASLFR_InitialDrawer;
            whatPath.ti_Data = (IPTR)"RAM:";

            aslRes = MUI_AslRequestTags(req,
                    ASLFR_TitleText,(IPTR) ASLTitle,
                    whatPath.ti_Tag, whatPath.ti_Data,
                    ASLFR_DoSaveMode, save,
                    ASLFR_DoPatterns, TRUE,
                    ASLFR_RejectIcons, TRUE,
                    ASLFR_UserData, app,
                    ASLFR_IntuiMsgFunc, &AslRefreshHook,
                    ASLFR_Window, w,
                TAG_DONE);

            if (!aslRes)
            {
                done = FALSE;
                break;
            }

            l = strlen((char *)req->fr_File);

            #ifdef __AROS__
            if (l>2)
            /* on AROS when user press close gadget on Asl req of MUI_AslRequestTags()
                 req->fr_File len = 2... */
            #else
            if (l>0) /* user has selected of entered a path */
            #endif
            {
                if (!strlen((char *)req->fr_Drawer))
                {
                    break; /* User could write a file without choosing a Drawer... */
                }

                sprintf((char *)_path, "%s", req->fr_Drawer);

                AddPart((STRPTR)_path, req->fr_File, sizeof(_path));

                done = TRUE; /* returning saved path */
                break;
            }
            else
            {
                 done = FALSE; /* path not valid or empty */
                 break;
            }
        }
        break;
    }

    if (done)
        *path = StrDup((STRPTR)_path);
   else
        jdebug("[getFilename] Returning no value");

    if (req)
        MUI_FreeAslRequest(req);

    set(app, MUIA_Application_Sleep, FALSE);

    jdebug("[getFilename] Returning %s", *path);


    return *path;
}

/**
    Save history reel to file
    @param [cl, obj] pointer to the class private data struct
**/
ULONG historyListSaveAs(struct IClass *cl, Object *obj)
{
    struct eData *myData = INST_DATA(cl, obj);
    APTR *fp;
    BOOL toggleSaveAsCsv = myData->toggleSaveAsCsv;
    BOOL result = TRUE;
    STRPTR file_path = AllocVec(sizeof(STRPTR)*ASL_BUFFER_LEN, MEMF_CLEAR);
    // printf("[historyListSaveAs] Allocated %d at 0x%p\n", sizeof(STRPTR)*ASL_BUFFER_LEN, file_path);
    if (file_path == NULL)
    {
        jerror("[historyListSaveAs] Failed memory allocation!");
        fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[historyListSaveAs] Failed memory allocation!\n");
    }
    // file_path = 0;

    Object *win = _win(obj);
    getFilename(win, (STRPTR)"Save as...", FALSE, &file_path);

    if (NULL != (fp = Open(file_path, MODE_NEWFILE)))
    {
        struct entry *item;
        item = AllocVec(sizeof(struct entry), MEMF_CLEAR);
        // printf("[historyListSaveAs] Allocated %d at 0x%p\n", sizeof(struct entry), item);
        STRPTR _line = AllocVec(sizeof(STRPTR)*HISTORY_LINE_LENGTH, MEMF_CLEAR);
        // printf("[rexxsendkeys] Allocated %d at 0x%p\n", sizeof(STRPTR)*HISTORY_LINE_LENGTH, _line);
        if (!_line)
        {
            jerror("[historyListSaveAs] Failed memory allocation!");
            fail((CONST_STRPTR)"jCalc", 101, (CONST_STRPTR)"[historyListSaveAs] Failed memory allocation!\n");
        }

        int i = 0;
        while(result == TRUE)
        {
            DoMethod(myData->historyList, MUIM_List_GetEntry, i, &item);
            if (!item) break;

            if (toggleSaveAsCsv == 1)
                snprintf((char *)_line, HISTORY_LINE_LENGTH, "\"%s\";\"%s\"\n", item->line, item->op);
            else
                snprintf((char *)_line, HISTORY_LINE_LENGTH, "%s %s\n", item->line, item->op);

            if (!Write(fp, _line, strlen((char *)_line)))
                result = FALSE;

            i++;
        }

        Close(fp);
        FreeVec(_line); _line = NULL;
        FreeVec(item); item = NULL;

        // FIXME: crash if I deallocate ptr: shouldn't I?
        // FreeVec(fp);

        jdebug("[historyListSaveAs] Finished saving file");
    }
    else
        jdebug("[historyListSaveAs] Can't open file '%s'", file_path);

    if (file_path)
    {
        FreeVec(file_path); file_path = NULL;
    }

    return (IPTR) TRUE;
}

/**
    Events handler
    @param [cl, obj] pointer to the class private data struct
**/
ULONG mHandler(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
    struct IntuiMessage *imsg;
    // struct eData *myData = INST_DATA(cl, obj);

    if((imsg = ((struct MUIP_HandleEvent *)msg)->imsg))
    {
        if (imsg->Class == IDCMP_VANILLAKEY || imsg->Class == IDCMP_RAWKEY)
        {
            // jwarning("[mHandler] Received 0x%x", imsg->Code);
            switch(imsg->Code)
            {
                case RAWKEY_BACKSPACE:
                    jdebug("[mHandler] Will manage case 0x%x (RAWKEY_BACKSPACE)", imsg->Code);
                    DoMethod(obj, MUIM_clearOneChar);
                    return MUI_EventHandlerRC_Eat;

                case RAWKEY_DELETE:
                    jdebug("[mHandler] Will manage case 0x%x (RAWKEY_DELETE)", imsg->Code);
                    DoMethod(obj, MUIM_clearDisplay, DISPLAY_CA);
                    return MUI_EventHandlerRC_Eat;

                case RAWKEY_KP_PLUS:
                    jdebug("[mHandler] Will manage case 0x%x (KP_PLUS)", imsg->Code);
                    DoMethod(obj, MUIM_setOperator, EVT_BTN_PLUS);
                    return MUI_EventHandlerRC_Eat;

                case RAWKEY_RETURN:
                    jdebug("[mHandler] Will manage case 0x%x (RAWKEY_RETURN)", imsg->Code);
                       DoMethod(obj, MUIM_doCalc, TRUE);
                    return MUI_EventHandlerRC_Eat;

                case RAWKEY_PERIOD:
                case RAWKEY_KP_DECIMAL:
                    jdebug("[mHandler] Will manage case 0x%x (RAWKEY_PERIOD|RAWKEY_KP_DECIMAL)", imsg->Code);
                       DoMethod(obj, MUIM_addDot);
                    return MUI_EventHandlerRC_Eat;

                case RAWKEY_P:
                    jdebug("[mHandler] Will manage case 0x%x (RAWKEY_P)", imsg->Code);
                       DoMethod(obj, MUIM_addConstant, 'P');
                    return MUI_EventHandlerRC_Eat;
            }
        }
        else if (imsg->Class == IDCMP_MOUSEBUTTONS)
        {
            jdebug("[mHandler] Mouse click with code=%d!", imsg->Code);
        }
        else
            jdebug("[mHandler] Event %d not managed", imsg->Code);
    }
    else
        jdebug("[mHandler] Could not get the imsg");

    return(DoSuperMethodA(cl, obj, (Msg)msg));
}

/**
    My event dispatcher
**/
DISPATCHER(jCalcDispatcher)
{
    switch(msg->MethodID)
    {
        case OM_NEW:                    return mJcalc_New(cl, obj, (APTR) msg);
        case OM_SET:                    return mJcalc_Set(cl, obj, (APTR) msg);
        case OM_GET:                    return mJcalc_Get(cl, obj, (APTR) msg);
        case MUIM_Setup:                return mJcalc_Setup(cl, obj, (APTR) msg);
        case MUIM_Cleanup:              return mJcalc_Cleanup(cl, obj, (APTR) msg);
        case MUIM_AskMinMax:            return mJcalc_AskMinMax(cl, obj, (struct MUIP_AskMinMax *) msg);
        // case MUIM_Draw:                return mJcalc_Draw(cl, obj, (struct MUIP_Draw *) msg);
        case MUIM_HandleEvent:          return mHandler(cl, obj, (struct MUIP_HandleEvent *) msg);

        case MUIM_ManageGroup:          return mManageGroup(cl, obj, (struct MUIP_JCALC_manageGroup *) msg);
        case MUIM_historyListInsert:    return mInsert(cl, obj, (struct MUIP_JCALC_ListEntry *) msg);
        case MUIM_doCalc:               return doCalcWrapper(cl, obj, (struct MUIP_JCALC_doCalcMsg *) msg);
        case MUIM_getDisplay:           return doGetDisplayWrapper(cl, obj, (struct MUIP_JCALC_getDisplayMsg *) msg);
        case MUIM_setDisplay:           return doSetDisplayWrapper(cl, obj, (struct MUIP_JCALC_setDisplayMsg *) msg);
        case MUIM_setOperator:          return doOpWrapper(cl, obj, (struct MUIP_JCALC_doOpMsg *) msg);
        case MUIM_setOperatorImmediate: return doOpWrapperImmediate(cl, obj, (struct MUIP_JCALC_doOpMsg *) msg);
        case MUIM_addDot:               return doAddDot(cl, obj);
        case MUIM_addNumber:            return doAddNumber(cl, obj, (struct MUIP_JCALC_numberMsg *) msg);
        case MUIM_addConstant:          return doAddConstant(cl, obj, (struct MUIP_JCALC_numberMsg *) msg);
        case MUIM_clearDisplay:         return doClearDisplay(cl, obj,(struct MUIP_JCALC_ClearMsg *) msg);
        case MUIM_clearOneChar:         return doClearOneChar(cl, obj);

        case MUIM_setSaveAs:            return setSaveAs(cl, obj);
        case MUIM_toggleHistoryPanel:   return toggleHistoryPanel(cl, obj);

        case MUIM_setBase:              return setBase(cl, obj, (struct MUIP_JCALC_setBaseMsg *) msg);
        case MUIM_setBaseBin:           return setBaseBin(cl, obj);
        case MUIM_setBaseDec:           return setBaseDec(cl, obj);
        case MUIM_setBaseHex:           return setBaseHex(cl, obj);
        case MUIM_setBaseOct:           return setBaseOct(cl, obj);

        case MUIM_setMode:              return setMode(cl, obj, (struct MUIP_JCALC_setModeMsg *) msg);
        // case MUIM_setModeBasic:        return setModeBasic(cl, obj);
        // case MUIM_setModePrg:        return setModePrg(cl, obj);

        case MUIM_historyListSaveAs:    return historyListSaveAs(cl, obj);

        case MUIM_changeSign:           return changeSign(cl, obj);
        case MUIM_memMgmt:              return memMgmt(cl, obj, (struct MUIP_JCALC_memMsg *) msg);

        // Added for AREXX purposes
        case MUIM_getLastResult:        return doGetLastResult(cl, obj, (struct MUIP_JCALC_AREXX_resultMsg *)msg);
    }

    return DoSuperMethodA(cl, obj, msg);
}

/**
    Create class instance
**/
struct MUI_CustomClass *initCalcClass(VOID)
{
    jdebug("class init");
    return (struct MUI_CustomClass *) MUI_CreateCustomClass(NULL, (ClassID)MUIC_Group, NULL, sizeof(struct eData), ENTRY(jCalcDispatcher));
}
