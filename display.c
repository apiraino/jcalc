/*
 * display.c
 *
 *  Created on: Jan 1, 2013
 *      Author: jman
 */

#include "includes/common.h"
#include "includes/zunecommon.h"
#include "display.h"

#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
// #include <proto/utility.h>
// #include <proto/muimaster.h>
// #include <libraries/mui.h>
#include <diskfont/diskfont.h>
// #include <devices/rawkeycodes.h>
// #include <zune/customclasses.h>

#include <proto/utility.h>

#ifndef JLOG
#define JLOG
#include "includes/log.h"
#endif

#include <aros/debug.h>

// IPTR DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...)
// {
//     AROS_SLOWSTACKTAGS_PRE(tag1)
//     retval = (IPTR)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
//     AROS_SLOWSTACKTAGS_POST
// }

/**
    OM_NEW overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
IPTR mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
	// Printf((STRPTR)"mNew\n");
    struct MyData *data = NULL;

    obj = (Object *)DoSuperNewTags(cl, obj, NULL,
            ReadListFrame,
        TAG_MORE, (IPTR) msg->ops_AttrList);

    if (!obj)
    {
        return FALSE;
    }
   
    data = INST_DATA(cl, obj);

    data->disp_textattr.ta_Name = (STRPTR)"aroscalculatorregular.font";
    data->disp_textattr.ta_YSize = 22;
    data->disp_textattr.ta_Style = FS_NORMAL;
    data->disp_textattr.ta_Flags = 0;

    data->disp_smalltextattr.ta_Name = (STRPTR)"aroscalculatorregular.font";
    data->disp_smalltextattr.ta_YSize = 8;
    data->disp_smalltextattr.ta_Style = FS_NORMAL;
    data->disp_smalltextattr.ta_Flags = 0;

    data->disp_font = OpenDiskFont(&data->disp_textattr);
    data->disp_fontsmall = OpenDiskFont(&data->disp_smalltextattr);

    data->disp_buff = NULL;
    data->disp_prev = NULL;

    // data->displ_operator = CALCDISPOP_NONE;
    // data->displ_flags = CALCDISPFLAG_CALCULATED;

    return (IPTR)obj;
}

/**
    OM_DISPOSE overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
IPTR CalcDisplay__OM_DISPOSE(struct IClass *cl, Object *obj, struct opSet *msg)
{
    jdebug("[CalcDisplay__OM_DISPOSE]");
    struct MyData *data = INST_DATA(cl, obj);

    if (data->disp_prev != data->disp_buff)
        FreeVec(data->disp_buff);
    data->disp_buff = NULL;
    FreeVec(data->disp_prev);
    data->disp_prev = NULL;

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/**
    OM_ASKMINMAX overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
IPTR mAskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
    struct MyData *data = INST_DATA(cl, obj);
    IPTR height = 3; // spacing

    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth += 50;
    msg->MinMaxInfo->DefWidth += 10;
    msg->MinMaxInfo->MaxWidth = MUI_MAXMAX;

    if (data->disp_font)
        height += data->disp_font->tf_YSize;
    else if ((_rp(obj)) && (_rp(obj)->Font))
        height += _rp(obj)->Font->tf_YSize;
    else
        height += 10;

    if (data->disp_fontsmall)
        height += data->disp_fontsmall->tf_YSize;
    else if ((_rp(obj)) && (_rp(obj)->Font))
        height += _rp(obj)->Font->tf_YSize;
    else
        height += 8;

    height = 0;

    msg->MinMaxInfo->MinHeight += height;
    msg->MinMaxInfo->DefHeight += height;
    msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

    return TRUE;
}

/**
    OM_DRAW overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
IPTR mDraw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct MyData *data = INST_DATA(cl, obj);
    APTR clip = NULL;
    char *dispstr = data->disp_buff;
    ULONG dispstrlen, dispstroff, opwidth = 0;

    DoSuperMethodA(cl, obj, (Msg)msg);

    jdebug("[mDraw] Entering with data->disp_buff '%s' (0x%p)", data->disp_buff, data->disp_buff);
    jdebug("[mDraw] Entering with dispstr '%s' (0x%p)", dispstr, dispstr);

    if (dispstr == NULL)
        dispstr = "0";

#if 1
    if (NULL != strstr(dispstr, "inf"))
    {
        jdebug("[mDraw] rcvd %s, will turn into %s", dispstr, "baadfood");
        strcpy(dispstr,"BAADF00D");
    }
    else
        jdebug("[mDraw] '%s' is ok...", dispstr);
#endif

    dispstrlen = strlen(dispstr);

    if (data->disp_font)
        SetFont(_rp(obj), data->disp_font);
    else
        SetFont(_rp(obj), _font(obj));

    dispstroff = TextLength(_rp(obj), (STRPTR)dispstr, dispstrlen);

    SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
    RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));

    SetAPen(_rp(obj), _pens(obj)[MPEN_TEXT]);

    // TODO: map also the other operators

    switch(data->displ_operator)
    {
        case CALCDISPOP_ADD:
            Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
            Text(_rp(obj), (STRPTR)"+", 1);
            opwidth = TextLength(_rp(obj), (STRPTR)"+", 1) + 4;
            break;
        case CALCDISPOP_SUB:
            Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
            Text(_rp(obj), (STRPTR)"-", 1);
            opwidth = TextLength(_rp(obj), (STRPTR)"-", 1) + 4;
            break;
        case CALCDISPOP_MUL:
            Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
            Text(_rp(obj), (STRPTR)"x", 1);
            opwidth = TextLength(_rp(obj), (STRPTR)"x", 1) + 4;
            break;
        case CALCDISPOP_DIV:
            Move(_rp(obj), _mleft(obj) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
            Text(_rp(obj), (STRPTR)"/", 1);
            opwidth = TextLength(_rp(obj), (STRPTR)"/", 1) + 4;
            break;
    }

    jdebug("[mDraw] drawing '%s' (%d)", dispstr, dispstrlen);
    clip = MUI_AddClipping(muiRenderInfo(obj), _mleft(obj) + opwidth, (_mbottom(obj) - _rp(obj)->Font->tf_YSize), (_mright(obj) - (_mleft(obj) + opwidth) + 1), _rp(obj)->Font->tf_YSize);
    Move(_rp(obj), (_mright(obj) - dispstroff) + 2, (_mbottom(obj) - _rp(obj)->Font->tf_YSize) + _rp(obj)->Font->tf_Baseline);
    Text(_rp(obj),(STRPTR) dispstr, dispstrlen);
    MUI_RemoveClipping(muiRenderInfo(obj), clip);

    return TRUE;
}

/**
    OM_SET overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
IPTR CalcDisplay__OM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct MyData *data = INST_DATA(cl, obj);
    struct TagItem      *tags = msg->ops_AttrList;
    struct TagItem      *tag;
    jdebug("[CalcDisplay__OM_SET] Entered");

    while ((tag = NextTagItem((const struct TagItem **)&tags)) != NULL)
    {
        jdebug("[CalcDisplay__OM_SET] while... switching 0x%p", (void *)tag->ti_Tag);
        switch (tag->ti_Tag)
        {
            case MUIA_CalcDisplay_Input:
            {
                jdebug("[CalcDisplay__OM_SET] MUIA_CalcDisplay_Input tag->ti_Data = '%s'", (STRPTR)tag->ti_Data);
                if ((data->disp_buff == NULL))
                {
                    jdebug("[CalcDisplay__OM_SET] data->disp_buff is NULL");
                    // if (data->disp_buff != data->disp_prev)
                    // {
                    //     jdebug("[CalcDisplay__OM_SET] Freeing data->disp_buff");
                    //     FreeVec(data->disp_buff); data->disp_buff = NULL;
                    // }
                    if (data->disp_buff)
                    {
                        FreeVec(data->disp_buff); data->disp_buff = NULL;
                    }
                    data->disp_buff = AllocVec(2, MEMF_CLEAR);
                    // printf("[CalcDisplay__OM_SET] Allocated %d at 0x%p\n", 2, data->disp_buff);
                    // data->disp_buff = (char *)tag->ti_Data;
                    int num = strtod((char *)tag->ti_Data, NULL);
                    snprintf(data->disp_buff, 2, "%d", num);
                }
                else
                {
                    jdebug("[CalcDisplay__OM_SET] data->disp_buff is already '%s', setting to '%s'", data->disp_buff, (STRPTR)tag->ti_Data);
                    // char *oldbuff = data->disp_buff;
                    // ULONG oldlen = strlen(oldbuff);
                    ULONG oldlen = strlen(data->disp_buff);
                    ULONG newlen = strlen((char *)tag->ti_Data);
                    if (oldlen < MUIV_CalcDisplay_MaxInputLen)
                    {
                        // FIXME: buffer should long as the new value
                        data->disp_buff = AllocVec(oldlen + newlen, MEMF_CLEAR);
                        // printf("[CalcDisplay__OM_SET] Allocated %d at 0x%p\n", oldlen+2, data->disp_buff);
                        // CopyMem(oldbuff, data->disp_buff, oldlen);
                        // data->disp_buff[oldlen] = (UBYTE)tag->ti_Data;

                        jdebug("[CalcDisplay__OM_SET] oldlen %d(+%d)", oldlen, newlen);
                        // snprintf(data->disp_buff, oldlen + 2, "%s%s", oldbuff, (char *)tag->ti_Data);
                        snprintf(data->disp_buff, oldlen + newlen, "%s", (char *)tag->ti_Data);

                        // FreeVec(oldbuff); oldbuff = NULL;
                    }
                }
                jdebug("[CalcDisplay__OM_SET] [A] Display set to: '%s' (0x%p)", data->disp_buff, data->disp_buff);

                MUI_Redraw(obj, MADF_DRAWOBJECT);
            }; break;
            
            default:
                // This happens when the history panel is opened/closed
                jdebug("[CalcDisplay__OM_SET] tag->ti_Tag not managed (%p)", (void *)tag->ti_Tag);
                // jdebug("[CalcDisplay__OM_SET] tag->ti_Tag = not managed (%lu) %s", tag->ti_Tag, (STRPTR)tag->ti_Data);
                // data->disp_buff = (char *)tag->ti_Data;
                // jdebug("[CalcDisplay__OM_SET] [B] *WOULD* probably set display to: '%s'", (STRPTR)tag->ti_Data);
                MUI_Redraw(obj, MADF_DRAWOBJECT);
            break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

// MyDispatcher
DISPATCHER(MyDispatcher)
{
	// Printf((STRPTR)"Dispatcher: rcvd msg %d\n", msg->MethodID);
	switch (msg->MethodID)
	{
        case OM_NEW:            return mNew(cl, obj, (struct opSet *)msg);
        case OM_SET:            return CalcDisplay__OM_SET(cl, obj, (struct opSet *)msg);

        case MUIM_Draw:         return mDraw(cl, obj, (struct MUIP_Draw *)msg);

        case MUIM_AskMinMax:    return mAskMinMax(cl, obj, (struct MUIP_AskMinMax *)msg);

        case OM_DISPOSE:        return CalcDisplay__OM_DISPOSE(cl, obj, (APTR)msg);

	}
	return(DoSuperMethodA(cl,obj,msg));
}

/**
    Create class instance
**/
struct MUI_CustomClass *initDisplayClass(VOID)
{
     return (struct MUI_CustomClass *) MUI_CreateCustomClass(NULL, (ClassID)MUIC_Area, NULL, sizeof(struct MyData), MyDispatcher);
}