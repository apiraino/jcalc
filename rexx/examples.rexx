/* JCALC remote management AREXX */

/*
	Always enable this: it will let you evaluate the last ReplyMsg
	from jcalc; 'rc' is the (mandatory) return code, 'result' is
	(an optional) return message.
*/
OPTIONS RESULTS

/*
	You must define some variables for those operators that overlap
	with AREXX syntax. It's a kludge, I'm sorry about that.
	Variable names can be anything: just find a way to send jcalc
	that character!
*/

ADD='+'
PLUS='+'
IMSODIZZYTODAY='+'
MIN='-'
MUL='*'
DIV='/'
EQU='='
POW='Y' /* x power y: "4 POW 3 EQU" returns 64 */

/* These operators don't need EQU to calculate the result */

PCT='%' /* Calculate percentage: "72 ADD 5 %" return 77 */
SQR='S' /* Square root: "9 S" returns 3 */
SQU='Q' /* Square: "4 S" returns 16 */
INV='I' /* Invert (1/x): "4 I" returns 0.25 */

/*
	OR you can "double quote" every line:
	"72 + 5 %"

	equals:
	72 ADD 5 PCT
*/

/* Locate jcalc port. 'JCALC' is mandatory */
PortName = 'JCALC'
IF ~SHOW(P, PortName) THEN DO
	ADDRESS COMMAND "run <nil: >nil: jcalc"
	ADDRESS COMMAND "WaitForPort "PortName
END
SAY "Port "PortName" available."

/* After the following instruction, any command will be sent to jcalc */
ADDRESS 'JCALC'

/* returns jcalc AREXX port version */
VERSION
SAY 'VERSION return code is:'rc' and msg:' result

/* Usage examples */

/* After every EQU you can print the current result */
1 ADD 5 EQU
SAY 'Result return code is: 'rc' and msg:' result
"1 + 5 ="
SAY 'Result return code is: 'rc' and msg:' result

10 MUL 51 EQU

/* comma MUST be a dot */
5.3 MIN 2.0 EQU

50 DIV 2 EQU

72 ADD 5 PCT

3 MIN 5 EQU

4 INV

4 POW 3 EQU

9 S

/*
	Doesn't mean much printing the result before EQU
	except for %,S,Q operators
*/
5
SAY 'Result return code is: 'rc' and msg:' result

/*
	Constants have a special char, e.g.:
	P = Pi
*/
P ADD 5 EQU

/* Syntax is pretty free */

1
 ADD
3
EQU
 EQU
  EQU
   EQU MIN 3
   				EQU

/* .12 + 33 = */
.   		12
  ADD
33
EQU


/* When you have finished playing, you can close jcalc if you want */
QUIT

/* End the AREXX script gracefully */
EXIT