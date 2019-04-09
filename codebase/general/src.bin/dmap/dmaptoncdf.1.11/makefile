# Makefile for dmaptoncdf
# =======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(NETCDF_PATH)/include
SRC=hlpstr.h errstr.h dmaptoncdf.c
OBJS =dmaptoncdf.o

LIBS= -lopt.1 -ldmap.1 -lrcnv.1
SLIB= -lm -lz -L$(NETCDF_PATH)/lib -lnetcdf

ifdef NETCDF_PATH
DSTPATH = $(BINPATH)
OUTPUT = dmaptoncdf
endif
include $(MAKEBIN).$(SYSTEM)
