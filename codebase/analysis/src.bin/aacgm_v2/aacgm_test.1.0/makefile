# Makefile for aacgm_v2 test program
# ======================
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis
OBJS = test_aacgm.o

SRC=hlpstr.h errstr.h test_aacgm.c
DSTPATH = $(BINPATH)
OUTPUT = test_aacgm
LNK=$(ARC)
LIBS=-laacgm_v2.1 -ligrf_v2.1 -lmlt_v2.1 -lastalg.1 -lopt.1 -lrtime.1
SLIB=-lm

 
include $(MAKEBIN).$(SYSTEM)
