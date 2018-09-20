# Makefile for meteor_proc
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=hlpstr.h errstr.h meteor_proc.c math.c nrutil.c nrutil.h math.h meteor.h
OBJS = meteor_proc.o math.o nrutil.o

DSTPATH = $(BINPATH)
OUTPUT = meteor_proc

LIBS=-loldfit.1 -lcfit.1 -lfit.1 -lrpos.1 -lradar.1 -ldmap.1 -laacgm.1 -lrtime.1 -lopt.1 -lrcnv.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)
