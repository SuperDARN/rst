# Makefile for eval
# =================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general \
        -I$(IPATH)/analysis -I$(IPATH)/imagery
SRC=hlpstr.h errstr.h eval.c
OBJS =eval.o
DSTPATH = $(BINPATH)
OUTPUT = rst_eval
LIBS= -lreval.1 -lopt.1 
SLIB= -lm

include $(MAKEBIN).$(SYSTEM)
