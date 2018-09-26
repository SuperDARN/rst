# Makefile for radarshell
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = radarshell.o shell.o
SRC=hlpstr.h errstr.h radarshell.c shell.c

DSTPATH = $(BINPATH)
OUTPUT = radarshell
LIBS=-ltcpipmsg.1 -lradarshell.1 -lrtime.1 -lopt.1
SLIB=-lm -lcurses

include $(MAKEBIN).$(SYSTEM)

