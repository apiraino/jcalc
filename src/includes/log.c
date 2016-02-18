/*
 * log.c
 *
 *  Created on: Apr 25, 2013
 *      Author: jman
 */

#include "common.h"
#include "log.h"

struct DateTime *dt;
struct DateStamp *ds;
struct ClockData cd;
struct Task *task;

#define MAXLOGBUF 4096
#define DATESIZE 32

static char	*logbuffer;
static JLEVEL loglevel = J_WARNING;
static char datestamp[DATESIZE+1];
static JDESTINATION	logmode = J_USE_LOGFILE;

#define	NOEXIT		0x1000
exit_t fail(CONST_STRPTR title, int code, CONST_STRPTR message)
{
	printf("[FAIL] Something really bad happened!\n");
	fprintf(stderr, "%s: %s %d: %s\n", title, code & NOEXIT ? "NOTIFY" : "ABORT", code, message);
    if (code & NOEXIT)
    	return EXIT_SUCCESS;
    exit(code);
    return EXIT_FAILURE;
}

// void jshutdown()
// {
// 	// printf("TODO Logging shut down\n");
// }

// static exit_t jrealloc(VOID)
// {
// 	logbuffer = AllocVec(MAXLOGBUF, MEMF_CLEAR);
// 	if (logbuffer == NULL)
// 	{
// 		printf("Fatal error occurred while allocating buffer\n");
// 		return EXIT_FAILURE;
// 	}

// 	FreeVec(logbuffer); logbuffer = NULL;
// 	return EXIT_SUCCESS;
// }

static void jshutdown(void)
{
	if (logbuffer)
	{
		free(logbuffer); logbuffer = NULL;
	}
}

static int jrealloc(void)
{
	if (NULL != (logbuffer = malloc(MAXLOGBUF)))
	{
		// if (NULL != (dupbuffer = malloc(MAXLOGBUF)))
		{
			*logbuffer = 0;
			// *dupbuffer = 0;
			return EXIT_SUCCESS;
		}
		free(logbuffer); logbuffer = NULL;
	}
	fprintf(stderr, "cannot allocate log buffer");
	return EXIT_FAILURE;
}

int jsetfile(CONST_STRPTR logfile)
{
	APTR *fp;
	if (NULL != (fp = Open(logfile, MODE_READWRITE)))
	{
		Close(fp);
		logmode = J_USE_LOGFILE;
	}
	else
	{
		printf("Couldn't open file %s: why??\n", logfile);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static int output_message(int level, const char *message, LONG openmode, JDESTINATION plogmode)
{
	if (0 == plogmode)
	{
		if (0 == logmode)
			abort();
		plogmode = logmode;
	}

	switch(plogmode & J_DESTINATION)
	{
		case J_USE_DEVNULL:
			break;
		case J_USE_SYSLOG:
		{
			// TODO See how to write to Sashimi
			// Does syslog logging support priority?
#if 0
			int	priority	= LOG_ALERT;	/* Not otherwise classified */
			switch(level)
			{
				/* Emergency. The system takes action deemed best, without waiting user confirmation. */
				case J_CURRENT_LEVEL:
				case J_ALWAYS:
				case J_CONFIG:
					priority	= LOG_EMERG;
					break;
				case J_ERROR:
					priority	= LOG_ERR;
					break;
				case J_WARNING:
					priority	= LOG_WARNING;
					break;
				case J_CONNECT:
				case J_INFO:
					priority	= LOG_NOTICE;
					break;
				case J_VERBOSE:
					priority	= LOG_INFO;
					break;
				case J_EXTRA:
				case J_DEBUG:
				case J_EVERYTHING:
					priority	= LOG_DEBUG;
					break;
			}
			// syslog(priority, "%s", message);
#endif
			fprintf(stderr, "[FAKE-SASHIMI] level=%d, %s\n", level, message);
			break;
		}
		case J_USE_LOGFILE:
		{
			APTR *fp;
			if (NULL != (fp = Open(logfile, openmode)))
			{
			    Seek(fp, 0, OFFSET_END);
				Write(fp, message, strlen(message));
				Close(fp);
			}
			else
			{
				fprintf(stderr, "cannot write message to '%s' \nMessage was: '%s'\n",
					logfile, message);
				plogmode -= J_USE_LOGFILE;
				plogmode += J_USE_STDERR;
			}
			break;
		}
		case J_USE_CONSOLE: /* TODO Here what ? */
		case J_USE_STDERR:
		{
			fprintf(stderr, "%s", message);
			break;
		}
		case J_USE_STDOUT:
		{
			fprintf(stdout, "%s", message);
			break;
		}
	}
	return EXIT_SUCCESS;
}

int jsetmode(JDESTINATION mode)
{
	logmode = (JDESTINATION)(mode & (J_DESTINATION));
	switch(logmode)
	{
		case J_USE_SYSLOG:
		{
			// TODO write to Sashimi
			break;
		}
		case J_USE_LOGFILE:
		{
			if (EXIT_SUCCESS != jsetfile(logfile))
			{
				logmode = J_USE_SYSLOG;
				fprintf(stderr, "Cannot set logfile to '%s'\n", logfile);
			}
			break;
		}
		case J_USE_STDOUT:
		{
			/* if we use STDOUT, we must make it UNBUFFERED! */
			/* Apparently this is not necessary ... */
			// setvbuf(stdout, NULL, _IONBF, 0);
			// SetVBuf(stdout, NULL, BUF_NONE, -1);
			break;
		}
		case J_USE_STDERR:
			break;
		case J_USE_DEVNULL:
			break;
		case J_USE_CONSOLE:
		{
			fprintf(stderr, "unsupported destination %d (%04X)\n", logmode, logmode);
			logmode = J_USE_STDERR;
		}
	}
	return EXIT_SUCCESS;
}

int jsetlev(int newlevel)
{
	int olev;
	if (J_CURRENT_LEVEL == newlevel)
		return loglevel;
	olev = loglevel;
	if (newlevel > J_MAX_COMPILED_LEVEL)
	{
		jprintf(J_ALWAYS, "requested log level %d, only available up to %d",
			newlevel, J_MAX_COMPILED_LEVEL);
		newlevel = J_MAX_COMPILED_LEVEL;
	}
	if (newlevel > J_DEBUG)
		newlevel = J_DEBUG;
	if (newlevel < J_ALWAYS)
		newlevel = J_ALWAYS;
	loglevel = (JLEVEL)(newlevel & J_LEVEL);
	return olev;
}

int jinit(JLEVEL level, JDESTINATION mode, CONST_STRPTR tag)
{
	if (EXIT_SUCCESS != jrealloc())
	{
		fprintf(stderr, "\x07\nFATAL: jrealloc failed\n");
		exit(-1);
	}

	if (J_USE_LOGFILE != mode)
		logfile	= tag;
	else
		if (EXIT_SUCCESS != jsetfile(tag))
			return EXIT_FAILURE;

	if (EXIT_SUCCESS != jsetmode(mode))
		return EXIT_FAILURE;
	loglevel = level;
	jsetlev(level);
	return EXIT_SUCCESS;
}

static inline void _myctime(void)
{
	printf("[LOGGER] Invoked myctime\n");
}

static inline void myctime(void)
{
	// printf("[TEST] About to alloc DateStamp\n");
	ds = AllocVec(sizeof(struct DateStamp), MEMF_CLEAR);
	if (!ds)
		printf("[LOGGER] Error allocating DateStamp\n");
	ds->ds_Days = 0;
	ds->ds_Minute = 0;
	ds->ds_Tick = 0;

	if (!DateStamp(ds))
		printf("[LOGGER] Error generating the DateStamp\n");

	STRPTR _day, _date, _time;
	// printf("[TEST] About to alloc _day\n");
	_day = AllocVec(LEN_DATSTRING, MEMF_CLEAR);
	if (!_day)
		printf("[LOGGER] Error allocating _day\n");

	// printf("[TEST] About to alloc _date\n");
	_date = AllocVec(LEN_DATSTRING, MEMF_CLEAR);
	if (!_date)
		printf("[LOGGER] Error allocating _date\n");
	
	// printf("[TEST] About to alloc _time\n");
	_time = AllocVec(LEN_DATSTRING, MEMF_CLEAR);
	if (!_time)
		printf("[LOGGER] Error allocating _time\n");
	
	// printf("[TEST] About to alloc _dt\n");
	dt = AllocVec(sizeof(struct DateTime), MEMF_CLEAR);
	if (!dt)
		printf("[LOGGER] Error allocating dt\n");
	dt->dat_Stamp = *ds;
	dt->dat_Format = FORMAT_DOS;
	dt->dat_Flags = 0; // DTF_SUBST;
	dt->dat_StrDay = _day;
	dt->dat_StrDate = _date;
	dt->dat_StrTime = _time;

	if (!DateToStr(dt))
		printf("[LOGGER] Error generating DateTime\n");

    Amiga2Date(dt->dat_Stamp.ds_Days * 60 * 60 * 24 + 
               dt->dat_Stamp.ds_Minute * 60 +
               dt->dat_Stamp.ds_Tick / 50, &cd);

	snprintf(datestamp, DATESIZE,
			"%04d-%02d-%02d %02d:%02d:%02d",
				cd.year,
				cd.month,
				cd.mday,
				cd.hour,
				cd.min,
				cd.sec);

	FreeVec(dt);
	FreeVec(_day);
	FreeVec(_date);
	FreeVec(_time);
	FreeVec(ds);

	// return datestamp;
}

int __jprintf(char *filename, long srcline, JLEVEL level, const char *fmt, ...)
{
	size_t i;

	char *logp = logbuffer;
	LONG openmode = MODE_OLDFILE;

	static char DebugBuffer[DEBUGBUFFERSIZE]; DebugBuffer[0] = '\0';
	if ((level & J_LEVEL) > loglevel)
		return EXIT_FAILURE;

	myctime();

	/* FIXME adesso stampa il path completo ! */
	/* ... dipende da dove si compila (vedere Makefile) */
	// printf(">>> filename=%s\n", filename);

	i = snprintf(logp, MAXLOGBUF, "%s|%ld|", filename, srcline);
	va_list	marker;
	va_start(marker, fmt);
	i += vsnprintf(logp + i, MAXLOGBUF, fmt, marker);
	va_end(marker);

	task = FindTask(NULL);

	snprintf(DebugBuffer, DEBUGBUFFERSIZE,
		"%s|%x|%lu|%s\n",
		datestamp,
		level,
		GetETaskID(task),
		logp);

	output_message(level,
		((J_USE_CONSOLE == logmode)
		||
		((level & J_DESTINATION) == J_USE_CONSOLE)) ? logp : DebugBuffer, openmode, loglevel & J_DESTINATION);

	return EXIT_SUCCESS;
}
