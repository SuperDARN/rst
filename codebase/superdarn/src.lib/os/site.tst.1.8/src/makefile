# Makefile for Site Library
# =========================
# by J.Spaleta & R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -I$(USR_IPATH)/superdarn

SRC = site.c
OBJS = site.o
INC=${USR_IPATH}/superdarn
LINK="1"
DSTPATH=$(USR_LIBPATH)
OUTPUT = site.tst
LIBS=-lacf.1 -ltsg.1 -lacfex.1 -lshmem.1 

include $(MAKELIB).$(SYSTEM)
