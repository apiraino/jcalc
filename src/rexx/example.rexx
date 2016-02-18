/* JCALC remote management AREXX */

/* CONFIG: don't touch */

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

/* END CONFIG */

/* Start your batch here */




1 + 3 = 
SAY 'EQU return code is:'rc' and msg:' result





/* Be nice and close jCalc once you're finished */
QUIT

/* End the AREXX script gracefully */
EXIT