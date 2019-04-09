# Makefile for rtpoll
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general
OBJS = rtpoll.o log_info.o
DSTPATH = $(BINPATH)
OUTPUT=rtpoll
SRC=hlpstr.h errstr.h rtpoll.c log_info.c log_info.h
LNK=$(ARC)
LIBS=-lrtime.1 -lopt.1 
SLIB=-lm

 

include $(MAKEBIN).$(SYSTEM)
