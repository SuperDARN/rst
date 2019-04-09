#  Makefile for istp_plot.c
#  ========================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(CDF_PATH)/include -I$(IPATH)/analysis


OBJS = wind.o ace.o imp.o geotail.o locate.o istp_text.o 
SRC=   wind.c wind.h ace.c ace.h imp.c imp.h geotail.c geotail.h locate.c locate.h istp_text.c istp.h hlpstr.h errstr.h

LIBS= -lrtime.1 -lopt.1 -lgeopack.1 -lrcdf.1  -L$(CDF_PATH)/lib -lcdf
SLIB= -lm

ifdef CDF_PATH
DSTPATH = $(BINPATH)
OUTPUT = istp_text
endif















include $(MAKEBIN).$(SYSTEM)
