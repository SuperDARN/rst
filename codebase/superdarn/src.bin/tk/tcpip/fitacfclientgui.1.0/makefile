# Makefile for fitacfclientgui
# ============================
# Author: E.G.Thomas
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitacfclientgui.o
SRC=hlpstr.h errstr.h fitacfclientgui.c

LIBS=-lfitcnx.1 -lfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lcnx.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 -lncurses -ltinfo
SLIB=-lz
DSTPATH = $(BINPATH)
OUTPUT = fitacfclientgui

include $(MAKEBIN).$(SYSTEM)
