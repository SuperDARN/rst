#  Makefile for imfdelay.c
#  =======================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(CDF_PATH)/include -I$(IPATH)/analysis


OBJS = wind.o ace.o locate.o imfdelay.o 
SRC=   wind.c wind.h ace.c ace.h locate.c locate.h imfdelay.c istp.h hlpstr.h errstr.h

LIBS= -lrtime.1 -lopt.1 -lgeopack.1 -lrcdf.1  -L$(CDF_PATH)/lib -lcdf
SLIB= -lm

ifdef CDF_PATH
DSTPATH = $(BINPATH)
OUTPUT = imfdelay
endif















include $(MAKEBIN).$(SYSTEM)
