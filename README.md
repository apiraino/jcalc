# jCalc 0.4 (06.10.2013)

## This is a simple pocket calculator and it simply does what it says.

### Requirements

jCalc currently runs on AROS hosted/native or other AROS distributions such as Icaros (+2.0)

### Download and compile

- ``git clone https://github.com/apiraino/jcalc.git``
- ``cd src``
- ``make jcalc``

### A brief overview of the main features:

- Decimal, octal, binary, hexadecimal conversions
- A couple of useful (?) math shortcuts for powers and square root
- Full memory registry use
- AREXX port (see "rexx" directory for info)
- Batch execution through text file input (see "batch" directory for info)

Shortcuts available:
- `SHIFT+B` Convert to binary
- `SHIFT+D` Convert to decimal
- `SHIFT+H` Convert to hexadecimal
- `SHIFT+O` Convert to octal
- `ENTER` = perform the calculation
- `+`, `-`, `*`, `/`, `%` are operators
- `a` to `f` hex numbers
- `p` to type in Pi
- `h` open the "tape" panel (a sheet of paper to remember past operations)
- `s` save tape to disk (optionally as a CSV)
- `DEL` to "Clear All"
- `Backspace`

KNOWN BUGS/LIMITS:
- Due to the actual limit of AROS 32bit build, I cannot allow input of more than 10 figures
- Random crashes occur due to the above mentioned limit
- This application eats memory (noticeable when running a batch of calculations): this is likely due to memory leaks outside jcalc.

jCalc icon: courtesy of http://www.iconsea.com
