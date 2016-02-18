#CC=/home/jman/wip/aros/AROS-20130420-source/bin/linux-i386/tools/i386-linux-aros-gcc
CC=$(AROSBIN)/i386-aros-gcc
CFLAGS_DBG=-Wall -Wextra -D__AROS__ -g -fno-stack-protector
CFLAGS_OPT=-Wall -Wextra -D__AROS__ -fno-stack-protector
LIBS=-lmui -llocale
SRCDIR=.#/home/jman/wip/aros/Aros2/src
DSTDIR=../bin#/home/jman/wip/aros/Aros2/bin
RUNDIR=/home/jman/aros/jman

mem_debug_test:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS_DBG) $(SRCDIR)/$@.c -o $(DSTDIR)/$@ $(CFLAGS) 
	cp $(DSTDIR)/$@ $(RUNDIR)/mem_debug_test

set_function:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS_DBG) $(SRCDIR)/$@.c -o $(DSTDIR)/$@ $(CFLAGS) 
	cp $(DSTDIR)/$@ $(RUNDIR)/set_function

rtdemo:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS_DBG) -Wno-pointer-sign $(SRCDIR)/$@.c -o $(DSTDIR)/$@ $(CFLAGS) 
	cp $(DSTDIR)/$@ $(RUNDIR)/rtdemo

rt:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS_DBG) -Wno-pointer-sign $(SRCDIR)/$@.c -o $(DSTDIR)/$@ $(CFLAGS) 
	cp $(DSTDIR)/$@ $(RUNDIR)/rt

vrfylog:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS_DBG) $(SRCDIR)/$@.c -o $(DSTDIR)/$@ $(CFLAGS) 
	cp $(DSTDIR)/$@ $(RUNDIR)/vrfylog

logtest:
	rm -f $(DSTDIR)/$@ $(RUNDIR)/$@
	$(CC) $(CFLAGS_DBG) $(SRCDIR)/$@.c -o $(DSTDIR)/$@ $(CFLAGS) 
	cp $(DSTDIR)/$@ $(RUNDIR)/logtest

jcalc:
	$(MAKE) -C $(SRCDIR)/jcalc/ clean
	$(MAKE) -C $(SRCDIR)/jcalc/ all

clean:
	rm -f $(DSTDIR)/*

all:	vrfylog
