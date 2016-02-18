#include "includes/common.h"

#ifndef JLOG
#define JLOG
#include "includes/log.h"
#endif

#include "includes/zunecommon.h"
#include "app.h"
#include "build.h"
#include "about.h"

/* Hooks */

HOOKPROTONHNP(aboutRemoveFunc, IPTR, Object *obj)
{
    SetAttrs(obj, MUIA_About_Close, TRUE, TAG_DONE);
    return (IPTR)obj;
}
MakeStaticHook(aboutRemoveHook, aboutRemoveFunc);

/**
    OM_NEW overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR aboutNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct aboutData temp;
    memset(&temp, 0, sizeof(temp));

    STRPTR aboutText = AllocVec(sizeof(STRPTR)*1024, MEMF_CLEAR);
    // printf("[aboutNew] Allocated %d at 0x%p\n", sizeof(STRPTR)*1024, aboutText);

    snprintf((char *)aboutText, 1024,
        "\33cjCalc %s (build: %d)\nCopyright (C) 2012-2013\n\nDistributed under the terms of the AROS Public License 1.1\nhttp://aros.sourceforge.net/license.html",
        VERSION, BUILDNO);

    temp.okButGrp = MUI_NewObject(MUIC_Group,
            Child, HSpace(0),
            Child, MUI_NewObject(MUIC_Group,
                MUIA_Group_SameSize, TRUE,
                MUIA_Group_Horiz, TRUE,
                Child, HSpace(0),
                Child, temp.okBut = MUI_MakeObject(MUIO_Button, (IPTR) "OK", TAG_DONE),
                Child, HSpace(0),
            TAG_DONE),
            Child, HSpace(0),
        TAG_DONE);

    temp.logo =  MUI_NewObject(MUIC_Group,
            MUIA_Background, MUII_SHINE,
            Child, MUI_NewObject(MUIC_Dtpic,
                MUIA_Dtpic_Name, "PROGDIR:jcalc.info",
            TAG_DONE),
        TAG_DONE);

    char cmsg[38] = { '\x77','\x68','\x79','\x69','\x61','\x6d','\x77','\x61','\x73','\x74',
                      '\x69','\x6e','\x67','\x6d','\x79','\x74','\x69','\x6d','\x65','\x6c',
                      '\x69','\x6b','\x65','\x74','\x68','\x69','\x73','\x69','\x73','\x61',
                      '\x6d','\x69','\x73','\x74','\x65','\x72','\x79','\0'};

    if ((obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Window_ScreenTitle, (IPTR)cmsg,
            MUIA_Window_SizeGadget, FALSE,
            MUIA_Window_CloseGadget, FALSE,
            MUIA_Window_ID, (IPTR) MAKE_ID('J','C','A','B'),
            MUIA_Window_SizeGadget, FALSE,
            WindowContents, MUI_NewObject(MUIC_Group,
                MUIA_Background, MUII_SHINE,

                Child, temp.logo,

                Child, MUI_NewObject(MUIC_Text,
                    MUIA_Background, MUII_SHINE,
                    MUIA_Text_Contents, aboutText,
                TAG_DONE),

                Child, temp.okButGrp,

            TAG_DONE),
            MUIA_Window_UseRightBorderScroller, FALSE,

        TAG_MORE, (IPTR)msg->ops_AttrList,
        TAG_DONE)))
    {
        struct aboutData *data = INST_DATA(cl,obj);

        memcpy(data,&temp,sizeof(*data));

        DoMethod(obj, MUIM_Notify,
                MUIA_Window_CloseRequest, TRUE,
                obj,
                2,
                MUIM_CallHook,
                &aboutRemoveHook);

        DoMethod(data->okBut, MUIM_Notify,
                MUIA_Pressed, FALSE,
                obj,
                2,
                MUIM_CallHook,
                &aboutRemoveHook);
    }

    if(aboutText)
    {
        FreeVec(aboutText); aboutText = NULL;
    }

    return (IPTR)obj;
}

/**
    OM_GET overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR aboutGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct aboutData *data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID)
    {
            case MUIA_About_Close:
                *msg->opg_Storage =(LONG) data->close;
             return TRUE;
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/**
    OM_SET overload
    @param [cl, obj] pointer to the class private data struct
    @param msg Message to the interface
**/
static IPTR aboutSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct aboutData *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for (tstate = msg->ops_AttrList; (tag = NextTagItem((TAGITEM)&tstate)); )
    {
        switch(tag->ti_Tag)
        {
            // case MUIA_Window_Sleep:
            //   SetAttrs(data->okButGrp, MUIA_Disabled, tag->ti_Data, TAG_DONE);
            // break;

            case MUIA_About_Close:
                    if (data->close == (LONG) tag->ti_Data)
                        tag->ti_Tag = TAG_IGNORE;
                    else
                        data->close = (LONG) tag->ti_Data;
            break;

            case MUIA_Window_Open:
                if (tag->ti_Data)
                    data->close = FALSE;
            break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/* My event dispatcher */
DISPATCHER(aboutDispatcher)
{
    switch (msg->MethodID)
    {
        case OM_NEW:        return aboutNew(cl,obj,(APTR)msg);
        case OM_GET:        return aboutGet(cl,obj,(APTR)msg);
        case OM_SET:        return aboutSet(cl,obj,(APTR)msg);

        default:            return DoSuperMethodA(cl,obj,msg);
    }
}

/* Create class instance */
struct MUI_CustomClass *initAboutClass(void)
{
    return (struct MUI_CustomClass *) MUI_CreateCustomClass(NULL, (ClassID)MUIC_Window, NULL, sizeof(struct aboutData), ENTRY(aboutDispatcher));
}
