# Makefile for FitCnxRead
# ==========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn 
IGNVER=1
OBJS = FitCnxRead.o
OUTPUT = FitCnxRead
LIBS=-lfitcnx.1 -lfit.1 -lcnx.1 -ldmap.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
