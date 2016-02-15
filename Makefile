CC=$(AROS_BIN)/i386-aros-gcc
# -D__REXXSYSLIB_STDLIBBASE__ is for having RexxSysBase of type Library (see proto/rexxsyslib.h)
CFLAGS= -Wall -Wextra -D__AROS__ -m32 -fno-stack-protector -D__REXXSYSLIB_STDLIBBASE__ -g
LIBS=-lmui -llocale
SRCDIR=src
DOCS=../docs
DSTDIR=../bin
DISTDIR=/home/jman/Desktop
RUNDIR=/home/jman/aros/jman
RUNDBGDIR=/home/jman/aros/AROS-20160119-source/bin/linux-i386/AROS/jman

OBJ =	about.o	\
	rexx.o \
	display.o \
	jcalc.o \
	main.o

all: jcalc

jcalc.o: jcalc.c jcalc.h
	@$(CC) $(CFLAGS) -c jcalc.c -o $@

about.o: about.h
	@$(CC) $(CFLAGS) -c about.c -o $@

display.o: display.h
	@$(CC) $(CFLAGS) -c display.c -o $@

rexx.o: rexx.h
	@$(CC) $(CFLAGS) -c rexx.c -o $@

main.o: main.c
	@$(CC) $(CFLAGS) -c main.c -o $@

update_build:
	sh update_build.sh
	@#buildno := sh update_build.sh
	@#echo $(buildno)

jcalc: $(OBJ)
	@rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	@$(CC) $(CFLAGS) -o $(DSTDIR)/$@ $(OBJ) $(LIBS)
	@cp $(DSTDIR)/$@ $(RUNDBGDIR)/jcalc
	@cp $(DSTDIR)/$@ $(RUNDIR)/jcalc
	@cp ../$@.info $(RUNDBGDIR)/$@.info
	@cp ../$@.info $(RUNDIR)/$@.info

pack: clean jcalc
	zip -r ../old/jcalc_bXXXX.zip *.h *.c Makefile
	zip -r ../old/jcalc_bXXXX.zip libs/ rexx/ batch/ includes/
	zip -j ../old/jcalc_bXXXX.zip $(RUNDIR)/jcalc.info $(DOCS)/README.txt $(DOCS)/CHANGELOG

dist: jcalc
	rm -f $(DISTDIR)/jcalc.zip
	zip -j $(DISTDIR)/jcalc.zip $(DSTDIR)/jcalc $(RUNDIR)/jcalc.info $(DOCS)/README.txt $(DOCS)/CHANGELOG
	zip -r $(DISTDIR)/jcalc.zip libs/ rexx/ batch/

vrfylog:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS) vrfylog.c -o $(DSTDIR)/$@
	cp $(DSTDIR)/$@ $(RUNDIR)/

clean:
	rm -f $(OBJ) $(DSTDIR)/jcalc
