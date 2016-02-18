#!/usr/bin/env python

# Create a sample input file to test AREXX port

TOTAL_OPS = 50

"""
POW='Y'		/* x power y: "4 POW 3 EQU" returns 64 */

/* These operators don't need EQU to calculate the result */
PCT='%' 	/* Calculate percentage: "72 ADD 5 %" return 77 */
SQR='S' 	/* Square root: "9 S" returns 3 */
SQU='Q' 	/* Square: "4 S" returns 16 */
INV='I' 	/* Invert (1/x): "4 I" returns 0.25 */
"""

OPERATORS = ['+', '-', '/', '*', 'Y', '%', 'S', 'Q', 'I']
BASIC_OPERATORS = ['+', '-', '/', '*']
SPECIAL_OPERATORS = ['S', 'Q', 'I']

MAX_FIGS = 7

import random

out_file = open("input.txt","w")

top = ''
for _ in range(MAX_FIGS):
	top = "%s%s" % (top , "9")

for count in range(TOTAL_OPS):
	random.seed()
	op1 = random.randrange(1, int(top))
	op2 = random.randrange(1, int(top))
	operator = random.choice(OPERATORS)

	if operator in SPECIAL_OPERATORS:
		content = "%d %s\n" % (op1, operator)
	elif operator == '%':
		operator = random.choice(BASIC_OPERATORS)
		content = "%d %s %d %s\n" % (op1, operator,  op2, '%')
	else:
		content = "%d %s %d =\n" % (op1, operator,  op2)

	out_file.write(content)

out_file.close()
