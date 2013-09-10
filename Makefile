CC=$(AROS_BIN)/i386-aros-gcc
# -D__REXXSYSLIB_STDLIBBASE__ is for having RexxSysBase of type Library (see proto/rexxsyslib.h)
CFLAGS_DBG= -Wall -Wextra -D__AROS__ -m32 -fno-stack-protector -D__REXXSYSLIB_STDLIBBASE__ -g
CFLAGS_OPT= -Wall -Wextra -D__AROS__ -m32 -fno-stack-protector -D__REXXSYSLIB_STDLIBBASE__
#CFLAGS= -I$(AROS_SDK)/include -Wall -Wextra -D__AROS__ -m32 -fno-stack-protector -g
LIBS=-lmui -llocale
SRCDIR=src
DOCS=../docs
DSTDIR=../bin
DISTDIR=/home/jman/Desktop
RUNDIR=/home/jman/aros/jman
RUNDIRDBG=/home/jman/aros/AROS-20130406-source/bin/linux-i386/AROS/jman

OBJ	=	about.o	\
		rexx.o \
		display.o \
		jcalc.o \
		main.o

all: jcalc

jcalc.o: jcalc.c jcalc.h
	@$(CC) $(CFLAGS_DBG) -c jcalc.c -o $@

about.o: about.h
	@$(CC) $(CFLAGS_DBG) -c about.c -o $@

display.o: display.h
	@$(CC) $(CFLAGS_DBG) -c display.c -o $@

rexx.o: rexx.h
	@$(CC) $(CFLAGS_DBG) -c rexx.c -o $@

main.o: main.c
	@$(CC) $(CFLAGS_DBG) -c main.c -o $@

update_build:
	sh update_build.sh
	@#buildno := sh update_build.sh
	@#echo $(buildno)

jcalc: $(OBJ)
	@rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	@$(CC) $(CFLAGS_DBG) -o $(DSTDIR)/$@ $(OBJ) $(LIBS)
	@cp $(DSTDIR)/$@ $(RUNDIRDBG)/jcalc
	@cp ../$@.info $(RUNDIR)/$@.info

pack: clean jcalc
	zip -rj ../old/jcalc_bXXXX.zip $(DSTDIR)/jcalc $(RUNDIR)/jcalc.info $(DOCS)/README.txt $(DOCS)/CHANGELOG *.h *.c libs/ rexx/

dist: jcalc
	rm -f $(DISTDIR)/jcalc.zip
	zip -j $(DISTDIR)/jcalc.zip $(DSTDIR)/jcalc $(RUNDIR)/jcalc.info $(DOCS)/README.txt $(DOCS)/CHANGELOG libs/ rexx/

clean:
	rm -f $(OBJ) $(DSTDIR)/jcalc
