Due to the limitation of the AREXX port (namely the ugly usage of variable to pass operators to jCalc), there's an additional way to feed jCalc with a batch: you can have jCalc parse a text file of operations and it will return the last result of the file.
This way you still need "symbols" to pass certain operators but it's a bit less awkward.

Here's how it works:

Operators can be represented according to the following schema:

Usual operators can be represented by their usual symbol:
+
-
/
*
%

Special operators are represented by the following symbols:

POW='Y'		/* x power y: "4 POW 3 EQU" returns 64 */

/* The following operators don't need EQU to calculate the result */

SQR='S'		/* Square root: "9 S" returns 3 */
SQU='Q'		/* Square: "4 S" returns 16 */
INV='I'			/* Invert (1/x): "4 I" returns 0.25 */

/* ";" is the comment marker: the following line will be ignored */

; 3 + 4 =

You can send a batch file tojCalc using the DOS shell and write the following:
jcalc --input input.txt

You can write a script and associate an icon to on Wanderer:
- Create a txt file with the above content, ensure to include full path to reach jcalc executable and the input file, example: "MYPROGDIR:jcalc --input RAM:input.txt"
- Create a PNG icon or set Wanderer window to show all files, select the input file and open Information on it, click save (a default icon will appear automagically)
- Set the Protection bits to: read, write, execute, delete, script
- Clicking on the icon will make invoke jCalc with file "input.txt". Now you can run different batch of calculation just changing the content of the input text file.