# Makefile for dmaptocdf
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(CDF_PATH)/include
SRC=hlpstr.h errstr.h dmaptocdf.c
OBJS=dmaptocdf.o
LIBS= -lopt.1 -ldmap.1 -lrcnv.1 -L$(CDF_PATH)/lib -lcdf
SLIB= -lm -lz

ifdef CDF_PATH
DSTPATH = $(BINPATH)
OUTPUT = dmaptocdf
endif



include $(MAKEBIN).$(SYSTEM)
