# Makefile for savetodmap
# =======================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC=savetodmap.c
IGNVER=1
OBJS = savetodmap.o 
DSTPATH=$(BINPATH)
OUTPUT = savetodmap

LIBS=-lidlsave.1 -lrcnv.1 -lopt.1 -ldmap.1 -lrcnv.1 

SLIB= -lm -lz
include $(MAKEBIN).$(SYSTEM)

