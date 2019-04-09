# Makefile for OldGridFwrite
# ==========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = OldGridFwrite.o
OUTPUT = OldGridFwrite
LIBS= -loldgrd.1 -lrfile.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
