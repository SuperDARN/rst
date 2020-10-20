# Makefile for snd library
# ========================
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=snd.c sndread.c sndwrite.c sndseek.c fitsnd.c sndscan.c
OBJS = snd.o sndread.o sndwrite.o sndseek.o fitsnd.o sndscan.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT = snd
LINK="1"

include $(MAKELIB).$(SYSTEM)
