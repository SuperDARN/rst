# Makefile for shmemsend
# ======================
# by R.J.Barnes
#
#

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base
OBJS = shmemsend.o
SRC=shmemsend.c
DSTPATH = $(BINPATH)
OUTPUT = shmemsend
LIBS=-lopt.1 -lshmem.1


ifeq ($(SYSTEM),linux)
  SLIB=-lm -lrt
else
  SLIB=-lm
endif


include $(MAKEBIN).$(SYSTEM)






