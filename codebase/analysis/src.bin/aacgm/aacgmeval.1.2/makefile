# Makefile for aacgmeval
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis
OBJS = aacgm.o

SRC=hlpstr.h errstr.h aacgm.c
DSTPATH = $(BINPATH)
OUTPUT = aacgmeval
LNK=$(ARC)
LIBS=-laacgm.1 -lopt.1 -lrtime.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1
SLIB=-lm

 
include $(MAKEBIN).$(SYSTEM)
