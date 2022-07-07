# Makefile for rpos_v2_test
# =========================
# Author: E.G.Thomas
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = rpos_v2_test.o
SRC=hlpstr.h errstr.h rpos_v2_test.c
DSTPATH = $(BINPATH)
OUTPUT = rpos_v2_test
LNK=$(ARC)
LIBS=-lrpos.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 -lradar.1 -lopt.1 -lrcnv.1 -lrtime.1
SLIB=-lm

 

include $(MAKEBIN).$(SYSTEM)
