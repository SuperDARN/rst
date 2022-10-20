# Makefile for rpos library
# ==========================
# by R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn 
SRC = cnvtcoord.c calcvector.c invmag.c geodtgc.h rpos_v2.c
OBJS = cnvtcoord.o calcvector.o invmag.o rpos_v2.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT = rpos
LINK="1"

include $(MAKELIB).$(SYSTEM)
