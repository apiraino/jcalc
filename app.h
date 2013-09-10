#ifndef __APP_H__
#define __APPH__

#define VERSTAG		"\0$VER: jCalc 0.3 (10.3.2013)"
#define VERSION		"0.3"

Object *app, *window, *jcalc_win;

exit_t parseCmdString (STRPTR line, STRPTR ret);

struct inputList
{
	STRPTR line;
	struct inputList *next;
};

#endif