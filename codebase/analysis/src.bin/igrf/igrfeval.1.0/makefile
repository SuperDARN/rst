# Makefile for igrfeval
# =====================
# Author: E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis
OBJS = igrf.o

SRC=hlpstr.h errstr.h igrf.c
DSTPATH = $(BINPATH)
OUTPUT = igrfeval
LNK=$(ARC)
LIBS=-laacgm.1 -lopt.1 -lrtime.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1
SLIB=-lm

 
include $(MAKEBIN).$(SYSTEM)
