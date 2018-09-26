# Makefile for Ops Library
# ===================================
# by J.Spaleta & R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -I$(USR_IPATH)/superdarn

SRC = build.c global.c reopen.c setup.c sync.c 
OBJS = build.o global.o reopen.o setup.o sync.o 
INC=${USR_IPATH}/superdarn
LINK="1"
DSTPATH=$(USR_LIBPATH)
OUTPUT = ops

include $(MAKELIB).$(SYSTEM)
