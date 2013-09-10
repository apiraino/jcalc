/*
 * rexx.h
 *
 *  Created on: Aug 12, 2013
 *      Author: jman
 */

#ifndef REXX_H_
#define REXX_H_

#define REPLY_STR			(STRPTR)"No action defined for command"
#define AREXXPORTVERSION 	(STRPTR)"0.3"

STRPTR portName; // = (STRPTR)"JCALC";
struct MsgPort *port;
struct Library *RexxSysBase;

/*
	rm_Args		Specifies the command/function name and arguments, stored as an array of 
				argstring pointers. For functions, the first slot (referred to as  ARGO), holds the 
				function name and the remaining fifteen slots the arguments. For commands, 
				only the first slot is used —- it holds the complete command string, of which the 
				first word is assumed to be the command name (the interpretation, however, 
				if left to the program that receives the command).

	The following struct will be used to store the Msg divided in command and its parameter(s)
*/

struct ArexxCmdParams
{
	STRPTR cmd;
	// STRPTR params[];
};

struct cmdList {
	char *name;
	void (*f)(struct RexxMsg *msg, struct ArexxCmdParams *command);
};
extern struct cmdList rexxCommandList[];

exit_t openRexxLibPort();
exit_t closeRexxLibPort();

// My Regina callbacks
void rexxversion (struct RexxMsg *msg, struct ArexxCmdParams *command);
void rexxquit (struct RexxMsg *msg, struct ArexxCmdParams *command);
void rexxsendkeys (struct RexxMsg *msg, struct ArexxCmdParams *command);

exit_t ValidateArexxCmdParams(STRPTR string, struct ArexxCmdParams *amp);
exit_t PrintArexxCmdParams(struct ArexxCmdParams *amp);
exit_t FreeArexxCmdParams(struct ArexxCmdParams *amp);

#endif /* REXX_H_ */
