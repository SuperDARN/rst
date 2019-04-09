# Makefile for rtfitacftofit
# ==========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -DPOSIX -D__EXTENSIONS__
OBJS = rtfitacftofit.o loginfo.o fork.o fitpacket.o \
	socket.o connect.o msgmem.o 
SRC=hlpstr.h errstr.h connect.c connect.h loginfo.c loginfo.h cnxstr.h fork.c fork.h rtfitacftofit.c fitpacket.c fitpacket.h msgmem.c msgmem.h socket.c socket.h

LIBS=-lfitcnx.1 -lfit.1 -lcfit.1 -lcnx.1 -lradar.1 -lrscan.1 -ldmap.1 -lrtime.1 -lopt.1 -lrcnv.1 
SLIB=-lz
DSTPATH = $(BINPATH)
OUTPUT = rtfitacftofit

include $(MAKEBIN).$(SYSTEM)
