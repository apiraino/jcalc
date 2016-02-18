#ifndef DISPLAY_H_
#define DISPLAY_H_

// #include <libraries/mui.h>

#define CALCDISPFLAG_HEXMODE (1 << 0)
#define CALCDISPFLAG_HASPERIOD (1 << 5)
#define CALCDISPFLAG_CLEAROP (1 << 30)
#define CALCDISPFLAG_CALCULATED (1 << 31)

enum
{
    CALCDISPOP_NONE = 0,
    CALCDISPOP_ADD,
    CALCDISPOP_SUB,
    CALCDISPOP_MUL,
    CALCDISPOP_DIV
};

#define MUIA_CalcDisplay_Input             0x80088008
// #define MUIA_CalcDisplay_Calculated     0x80088009
// #define MUIM_CalcDisplay_DoCurrentStep  0x80088008
// #define MUIA_CalcDisplay_Base           0x8008800A

#define MUIV_CalcDisplay_MaxInputLen 32
#define MUIV_CalcDisplay_InputCA -1
#define MUIV_CalcDisplay_InputCE -2

struct MyData
{
    char *disp_buff;
    char *disp_prev;
    struct TextFont   *disp_font;
    struct TextFont   *disp_fontsmall;
    struct TextAttr   disp_textattr;
    struct TextAttr   disp_smalltextattr;
    ULONG displ_operator;
    ULONG displ_flags;
};

struct MUI_CustomClass *initDisplayClass(void);

#endif /* DISPLAY_H_ */
