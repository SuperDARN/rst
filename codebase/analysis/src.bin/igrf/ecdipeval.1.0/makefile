# Makefile for ecdipeval
# ======================
# Author: E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis
OBJS = ecdip.o

SRC=hlpstr.h errstr.h ecdip.c
DSTPATH = $(BINPATH)
OUTPUT = ecdipeval
LNK=$(ARC)
LIBS=-lopt.1 -lrtime.1 -ligrf_v2.1 -lastalg.1
SLIB=-lm

 
include $(MAKEBIN).$(SYSTEM)
