# Makefile for make_sim
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_sim.o
SRC = hlpstr.h errstr.h make_sim.c
DSTPATH = $(BINPATH)
OUTPUT = make_sim
LIBS=-lsim_data.1 -lrpos.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 -loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lfitacf.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 -lrmath.1 -liqdata.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
