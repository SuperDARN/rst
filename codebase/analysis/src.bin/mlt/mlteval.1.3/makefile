# Makefile for mlteval
# ================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis
OBJS = mlt.o

SRC=hlpstr.h errstr.h mlt.c
DSTPATH = $(BINPATH)
OUTPUT = mlteval
LNK=$(ARC)
LIBS=-lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 -lmlt.1 -laacgm.1 -lastalg.1 -lopt.1 -lrtime.1
SLIB=-lm

 
include $(MAKEBIN).$(SYSTEM)
