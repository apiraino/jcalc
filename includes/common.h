/*
 * common.h
 *
 *  Created on: Jan 1, 2013
 *      Author: jman
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <exec/types.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/intuition.h>

/* DOS */
#include <dos/stdio.h>
#include <dos/datetime.h>

/* MsgPort */
#include <proto/alib.h>
#include <exec/ports.h>

/* AREXX */
#include <proto/rexxsyslib.h>
#include <rexx/errors.h>
#include <rexx/storage.h>

#ifndef __amigaos4__
#include <clib/alib_protos.h>
#include <clib/locale_protos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include <devices/rawkeycodes.h>
#include <proto/keymap.h>

#include <SDI/SDI_hook.h>

#define DEBUG 0 		// 0 = do not print D(bug()) macros and logging
#define ENABLE_MEMDBG 0	// 0 = do not trace memory usage
#define GUI_DEBUG 0 	// 0 = do not show frames around groups

#ifdef __amigaos4__
	#if DEBUG
		#define D(x) x
	#else
		#define D(x)
	#endif
	#define bug printf //or DebugPrintF or any other kind of printf
#else
	// #include <aros/debug.h>
	/* Eclipse syntax hightlight gets confused without this
	 * and cannot find D(bug())
	 */
	#if DEBUG
		#define D(x)
	#endif
#endif

#ifndef MAKE_ID
	#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifdef D
	#define D_ISDEFINED 1
#else
	#define D_ISDEFINED 0
#endif

#define TAGITEM const struct TagItem **

typedef enum
{
	EXIT_OK		= EXIT_SUCCESS,
	EXIT_ERR	= EXIT_FAILURE
} exit_t;

exit_t fail(CONST_STRPTR title, int code, CONST_STRPTR message);
IPTR DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...);

#endif /* COMMON_H_ */

