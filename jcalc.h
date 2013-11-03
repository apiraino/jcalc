/*
 * jcalc.c
 *
 *  Created on: Mar 11, 2013
 *      Author: jman
 *
 *  Copyright © 2012-2013, Antonio Piraino
 *  Distributed under the terms of the AROS Public License 1.1
 *	http://aros.sourceforge.net/license.html
 *
 *  Desc: a simple calculator
 *  Language: English
 *
 */

#ifndef JCALC_H_
#define JCALC_H_

/* MUI defines */

#define MUI_CLASS_JCALC					(TAG_USER + 20)
#define MUIA_JCALC_toggleHistoryPanel	MUI_CLASS_JCALC + 20
#define MUIA_JCALC_SaveAsCSV			MUI_CLASS_JCALC + 30
#define MUIA_JCALC_SetBase				MUI_CLASS_JCALC + 40
#define MUIA_JCALC_SetMode				MUI_CLASS_JCALC + 50

#define MUIM_KeyPress		 			MUI_CLASS_JCALC + 200
#define MUIM_AddPanel					MUI_CLASS_JCALC + 210
#define MUIM_doCalc						MUI_CLASS_JCALC + 220
#define MUIM_getDisplay					MUI_CLASS_JCALC + 230
#define MUIM_setDisplay					MUI_CLASS_JCALC + 240
#define MUIM_setOperator				MUI_CLASS_JCALC + 250
#define MUIM_setOperatorImmediate		MUI_CLASS_JCALC + 260
#define MUIM_ManageGroup				MUI_CLASS_JCALC + 270
#define MUIM_addDot						MUI_CLASS_JCALC + 280
#define MUIM_addNumber					MUI_CLASS_JCALC + 290
#define MUIM_addConstant				MUI_CLASS_JCALC + 300
#define MUIM_clearDisplay				MUI_CLASS_JCALC + 310
#define MUIM_clearOneChar				MUI_CLASS_JCALC + 320
#define MUIM_setSaveAs					MUI_CLASS_JCALC + 330
#define MUIM_setBaseBin					MUI_CLASS_JCALC + 340
#define MUIM_setBaseDec					MUI_CLASS_JCALC + 350
#define MUIM_setBaseHex					MUI_CLASS_JCALC + 360
#define MUIM_setBaseOct					MUI_CLASS_JCALC + 370
#define MUIM_setModeBasic				MUI_CLASS_JCALC + 380
#define MUIM_setModePrg					MUI_CLASS_JCALC + 390
#define MUIM_historyListInsert			MUI_CLASS_JCALC + 400
#define MUIM_historyListSaveAs			MUI_CLASS_JCALC + 410
#define MUIM_changeSign					MUI_CLASS_JCALC + 420
#define MUIM_memMgmt					MUI_CLASS_JCALC + 430
#define MUIM_setBase					MUI_CLASS_JCALC + 440
#define MUIM_setMode					MUI_CLASS_JCALC + 450
#define MUIM_toggleHistoryPanel			MUI_CLASS_JCALC + 460

#define MUIM_getLastResult				MUI_CLASS_JCALC + 500

/* Other defines */

#define LINE_SEPARATOR "---------------" /* 15 chars (for the lazy) */
#define APPHEIGHT			256
#define HISTORY_LINE_LENGTH 90
#define ASL_BUFFER_LEN 		2048

#define MAX_DECIMALS 2

#define INSMODE		1
#define OPMODE		2
#define CALCMODE	4

#define PI 			"3.141592"

enum
{
	BINBASE = 2,
	DECBASE = 10,
	HEXBASE = 16,
	OCTBASE = 8
};

enum
{
	/* Max 32 bit because strtol overflows */
	MAX_DISPLAY_BIN_FIGURES = 32,

	// A DOUBLE allows more or less 17 figures; beyond that the
	// behaviour of the printf is undetermined.
	MAX_DISPLAY_DEC_FIGURES = 16,

	/* Max FFFF FFFF because strtol overflows */
	MAX_DISPLAY_HEX_FIGURES = 8,

	MAX_DISPLAY_OCT_FIGURES = 10
};

enum
{
	BASICMODE = 0x0,
	PRGMODE
};

#define isInsertMode(x)	(x & INSMODE)
#define isOpMode(x)		(x & OPMODE)
#define isCalcMode(x)	(x & CALCMODE)

STRPTR comma;

/* MUI Messages */

struct MUIP_JCALC_ListEntry
{
  STACKED ULONG  MethodID;
//  STACKED STRPTR str;
  STACKED DOUBLE number;
  STACKED LONG op;
};

struct MUIP_JCALC_getDisplayMsg
{
	STACKED ULONG MethodID;
	STACKED STRPTR src;
	STACKED DOUBLE *dst;
	STACKED LONG base;
};

struct MUIP_JCALC_setDisplayMsg
{
	STACKED ULONG MethodID;
	// STACKED DOUBLE val;
	STACKED STRPTR val;
	STACKED ULONG doProcess;
};

struct MUIP_JCALC_doOpMsg
{
	STACKED ULONG MethodID;
	STACKED LONG value;
};

struct MUIP_JCALC_doCalcMsg
{
	STACKED ULONG MethodID;
	/*
	 * This flag is a shame.
	 * Its only purpose is to tell whether the doCalc method must be invoked
	 * from a direct key "=" hit or not.
	 * Based on this flag, I'll write different lines into the history
	 * reel.
	 *
	 */
	STACKED LONG clickedReturnBtn;

	/* This flag tells us if we are performing an immediate calculation (e.g. EVT_BTN_PERCENT) */
	/* In this case the second operand is already logged, I need to log also the results lines */
	STACKED LONG isImmediateCalc;
};

struct MUIP_JCALC_manageGroup
{
	STACKED ULONG MethodID;
	/* container */
	STACKED Object *_mainGrp;
	/* child group */
	STACKED Object *_subGrp;
};

struct MUIP_JCALC_numberMsg
{
	STACKED ULONG MethodID;
	STACKED LONG value;
};

struct MUIP_JCALC_ClearMsg
{
	STACKED ULONG MethodID;
	STACKED LONG flag;
};

struct MUIP_JCALC_memMsg
{
	STACKED ULONG MethodID;
	STACKED LONG operator;
};

struct MUIP_JCALC_setBaseMsg
{
	STACKED ULONG MethodID;
	STACKED LONG base;
};

struct MUIP_JCALC_setModeMsg
{
	STACKED ULONG MethodID;
	STACKED LONG mode;
};

struct MUIP_JCALC_setSaveAsMsg
{
	STACKED ULONG MethodID;
	STACKED ULONG value;
};

struct MUIP_JCALC_AREXX_resultMsg
{
	STACKED ULONG MethodID;
	STACKED STRPTR value;
};

enum
{
	DISPLAY_CA = 0,
	DISPLAY_CE
} DISPLAY_CLEAR_FLAGS;

struct entry
{
    STRPTR line;
    STRPTR op;
};

/* JCALC FUNCTION PROTO */

struct MUI_CustomClass *initCalcClass(void);
exit_t removeTrailingZeroes(STRPTR *);
exit_t convertDisplay(STRPTR *, BYTE, BYTE *);
exit_t convertComma(STRPTR *);

enum
{
	EVT_BTN_PLUS = 0,
	EVT_BTN_MINUS,
	EVT_BTN_DIVIDE,
	EVT_BTN_MULTIPLY,
	EVT_BTN_EQUALS,
	EVT_BTN_DOT,
	EVT_BTN_PERCENT,
	EVT_BTN_SQRT,
	EVT_BTN_SQUARE,
	EVT_BTN_POWERY,
	EVT_BTN_INVERSE,
} operators;

enum
{
	EVT_BTN_MEMADD = 0,
	EVT_BTN_MEMSUB,
	EVT_BTN_MEMRECALL,
	EVT_BTN_MEMCLEAR,
	EVT_BTN_MEMSAVE
} mem_op;

struct reel
{
	BYTE	op;
	DOUBLE	value;
	struct reel *next;
};

struct calcStack
{
	BYTE	op;		// see operators enum
	DOUBLE	n1;
	DOUBLE	n2;
	DOUBLE	res;
	BYTE	base;	// 2=bin, 8=oct, 10=dec, 16=hex, ...
};

struct eData
{
	/* Display of the calculator */
	// This object will be useless
	// Object *display;

	// Store what I write into the display
	// because I cannot read it back
	STRPTR displayStr;

	// this is a "write-only" object
	Object *displayDraw;

	/* Current calculator mode */
	BYTE mode;

	/* Keeps track of all the past calculations */
	struct calcStack *cs;

	/*
	 * This is used to save the real operator (+, -, /, ...) when the user clicks "="
	 * which itself is considered an operator.
	 */
	int operator;

	/* Manage MEM operations */
	DOUBLE mem;

	/* History reel (not used) */
	struct reel *history;

	/* This group contains everything but the display */
	Object *grp_main;

	/* This group contains buttons for A-F numbers, BIN,DEC,HEX,OCT converters
	 * and other special math operations
	 */
	Object *grp_scientific;

	/* Buttons for HEX numbers */
	Object *grp_hex;

	/* Buttons for switching base mode */
	Object *grp_mode;

	/* Am I in basic or programmer mode */
	/* (see enum: BASICMODE or PRGMODE) */
	ULONG interfaceMode;

	/* Store the current base mode (see enum: BINBASE, DECBASE, HEXBASE, OCTBASE */
	ULONG baseMode;

	/* This group contains buttons for base calculator use */
	Object *grp_base;

	/* This contains the scrollable panel with calculations history */
	Object *historyPanel;

	/* Object containing the history text */
	// Object *historyPanelTextContent;

	/* History list object */
	Object *historyList;

	/* A lot of buttons */
	Object *btn_zero, *btn_one, *btn_two, *btn_three, *btn_four, *btn_five, *btn_six,
			*btn_seven, *btn_eight, *btn_nine;
	Object *btn_dot, *btn_plus, *btn_minus, *btn_multiply, *btn_divide, *btn_equals;
	Object *btn_ce, *btn_ca, *btn_bs, *btn_pi, *btn_history;
	Object *btn_mem_mc, *btn_mem_mr, *btn_mem_ms, *btn_mem_add, *btn_mem_sub;
	Object *btn_percent, *btn_sqrt, *btn_invert_sign, *btn_inverse, *btn_power_two, *btn_power_y;
	Object *btn_bin, *btn_oct, *btn_dec, *btn_hex;
	Object *btn_a, *btn_b, *btn_c, *btn_d, *btn_e, *btn_f;

	Object *btn_history_save;
	Object *btn_history_clear;

	/* About Window object */
	Object *winAbout;

	/* Event handler for keystrokes */
	struct MUI_EventHandlerNode ehnode;

	/* Class public attributes */
	LONG toggleSaveAsCsv;
	LONG toggleHistoryPanel;
	LONG setMode;
	LONG setBase;

	// Object *sep;
};

/* Since I can't reference these Application objects from my jcalc
 * Window class, I need to store them globally (which is not great)
 * FIXME: instance the Application object from a custom object, create
 * the main Window from it as a MCC as usual.
 */
Object *menu_about, *menu_quit, *menu_csv, *menu_history,
	*menu_bin, *menu_dec, *menu_hex, *menu_oct, *menu_basic,
	*menu_prg, *menu_mode, *menu_base;

#endif /* JCALC_H_ */
