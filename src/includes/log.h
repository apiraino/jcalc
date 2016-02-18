/*
 * log.h
 *
 *  Created on: Apr 25, 2013
 *      Author: jman
 */

/* Logging functions */

#ifndef __JLOG_STRUCT__
#define __JLOG_STRUCT__

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

CONST_STRPTR logfile;

typedef enum	// warning levels:
				// I will print logs *up to* the level choosen
{
	J_NEVER				= -1,
	J_CURRENT_LEVEL		= -1,
	J_ALWAYS 			= 0,
	J_NOTHING			= 0,
	J_CONFIG,			/* Configuration errors */
	J_ERROR,
	J_WARNING,		
	J_CONNECT,
	J_INFO,				/* Informations */
	J_VERBOSE,
	J_DEBUG,
	J_EVERYTHING		/* 0x0F */
} JLEVEL;

#define	J_LEVEL		0x0F

typedef enum	// where the log will be directed (tipically 40,20,10)
{
	J_USE_SYSLOG	= 0x10,	// system log // TODO Sashimi?
	J_USE_LOGFILE	= 0x20,	// disk file (to be specified)
	J_USE_STDOUT	= 0x30,
	J_USE_STDERR	= 0x40,
	J_USE_DEVNULL	= 0x50,	// nowhere
	J_USE_CONSOLE	= 0x60,	// windows console // TODO not needed (?)
} JDESTINATION;

#define	J_DESTINATION	0x70	/* Power of 2 minus 1 */

#ifndef DEBUGBUFFERSIZE
#define DEBUGBUFFERSIZE 10240 /* 10 kb */
#endif

#endif /* __JLOG_STRUCT__ */

#ifndef __JLOG_FUNC_
#define __JLOG_FUNC_

/**
 * Functions are declared if JLOG is defined (whichever its value) 
 * or left as empty macros, so I can leave them in the code being
 * compiled.
 */

#if defined(JLOG)
	int jinit		(JLEVEL minlevel, JDESTINATION logmethod, CONST_STRPTR logtag);
	int jsetfile	(CONST_STRPTR file);
	int __jprintf	(char *, long, JLEVEL, const char *fmt, ...)	__attribute__ ((format (printf, 4, 5)));
	int jsetlev		(JLEVEL newlevel);
	int jsetmode	(JDESTINATION newmode);
	#define jprintf(l, ...) __jprintf(__FILE__, __LINE__, l, __VA_ARGS__ )
#else
	/*
	 * Placeholders. If JLOG is not defined, these won't be compiled.
	 */
	#define jconfig (...)
	#define	jinit(...)	EXIT_SUCCESS
	#define jprintf(...)
	#define jsetlev(...)
	#define jsetfile(...)
	#define jsetmode(...)
	/**
	 * Exception
	 */
	#define	jerror(...)	{fprintf(stderr, __VA_ARGS__);fputc('\n',stderr);}
#endif

#endif
// ex: jverbose(...) = jprintf(J_VERBOSE,...)
#ifndef	jinfo
#define jinfo(...)		jprintf(J_VERBOSE, __VA_ARGS__)
#define jwarning(...)	jprintf(J_WARNING, __VA_ARGS__)
#define jconfig(...)	jprintf(J_CONFIG, __VA_ARGS__)
#define jerror(...)		jprintf(J_ERROR, __VA_ARGS__)
#define	jverbose(...)	jprintf(J_VERBOSE, __VA_ARGS__)
#define	jconnect(...)	jprintf(J_CONNECT, __VA_ARGS__)
#define jdebug(...)		jprintf(J_DEBUG, __VA_ARGS__)

#define	J_MAX_COMPILED_LEVEL	J_DEBUG
#define	CLI_JINIT				jinit(J_DEBUG, CLI_JINIT_TARGET, NULL)

#endif /* __JLOG_FUNC__ */
