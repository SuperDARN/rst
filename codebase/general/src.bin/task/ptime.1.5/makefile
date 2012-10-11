# Makefile for ptime
# ==================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
SRC=ptime.c
IGNVER=1
OBJS = ptime.o 
DSTPATH=$(BINPATH)
OUTPUT = ptime
LIBS=-lopt.1 -lrtime.1
SLIB= -lm
include $(MAKEBIN).$(SYSTEM)

