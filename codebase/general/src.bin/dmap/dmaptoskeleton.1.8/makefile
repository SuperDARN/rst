# Makefile for dmaptoskeleton
# ===========================
# Author: R.J.Barnes
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
SRC=hlpstr.h errstr.h dmaptoskeleton.c
OBJS =dmaptoskeleton.o
DSTPATH = $(BINPATH)
OUTPUT = dmaptoskeleton
LIBS= -lopt.1 -ldmap.1 -lrcnv.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
