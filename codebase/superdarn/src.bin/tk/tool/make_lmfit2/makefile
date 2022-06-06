# Makefile for make_fit
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I/home/k2/Documents/ISAS_git/fitacf.3.0/include
OBJS = make_fit.o

SRC=hlpstr.h errstr.h make_fit.c

DSTPATH = $(@D)
OUTPUT = make_fit
LIBS=  ~/Documents/ISAS_git/fitacf.3.0/lib/libfitacf.a -loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lfitacf.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz -lpthread
 

include $(MAKEBIN).$(SYSTEM)
