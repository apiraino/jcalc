/* JCALC remote management AREXX */

OPTIONS RESULTS

ADD='+'
MIN='-'
MUL='*'
DIV='/'
EQU='='

PortName = 'JCALC'
IF ~SHOW(P, PortName) THEN DO
	ADDRESS COMMAND "run <nil: >nil: jcalc"
	ADDRESS COMMAND "WaitForPort "PortName
END
SAY "Port "PortName" available."
ADDRESS 'JCALC'

VERSION
SAY 'VERSION return code is:'rc' and msg:' result

SEND 1
SEND ADD
SEND 3
SEND EQU
SAY 'EQU return code is:'rc' and msg:' result
QUIT

EXIT