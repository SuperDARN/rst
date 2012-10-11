# Makefile for OldCnvMapFwrite
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = OldCnvMapFwrite.o
OUTPUT = OldCnvMapFwrite
LIBS=-loldcnvmap.1 -lcnvmap.1 -loldgrd.1 -lgrd.1 -lrfile.1 -lrtime.1 -ldmap.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
