# Makefile for RfileRead
# ======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
IGNVER=1
OBJS = RfileRead.o
OUTPUT = RfileRead
LIBS=-lrfile.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
