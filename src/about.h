#ifndef __ABOUTEDCLASS__
#define __ABOUTEDCLASS__

#define MUIA_About_Close TAG_USER + 50

struct MUI_CustomClass  *initAboutClass(void);

#define DEFSTRLEN 50
#define VRSTRING 50
#define DATE "YYYY-MM-DD"

struct aboutData
{
    Object *okBut, *okButGrp, *logo;
    LONG close;
};

#endif
