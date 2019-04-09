# Makefile for shmemrecv
# ======================
# by R.J.Barnes
#
#

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base
OBJS = shmemrecv.o
SRC=shmemrecv.c
DSTPATH = $(BINPATH)
OUTPUT = shmemrecv
LIBS=-lopt.1 -lshmem.1

ifeq ($(SYSTEM),linux)
  SLIB=-lm -lrt
else
  SLIB=-lm
endif


include $(MAKEBIN).$(SYSTEM)






