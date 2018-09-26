# Makefile for TCPIP Message Library
# ===================================
# by J.Spaleta & R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn 

SRC = tcpipmsg.c err.c
OBJS = tcpipmsg.o err.o
INC=$(IPATH)/superdarn
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT = tcpipmsg

include $(MAKELIB).$(SYSTEM)
