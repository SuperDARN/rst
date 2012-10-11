# Makefile for ipclient
# =====================
# Author: R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base
OBJS = ipclient.o
SRC=hlpstr.h errstr.h ipclient.c
LIBS=-lcnx.1 -lopt.1
DSTPATH = $(BINPATH)
OUTPUT = ipclient
include $(MAKEBIN).$(SYSTEM)
