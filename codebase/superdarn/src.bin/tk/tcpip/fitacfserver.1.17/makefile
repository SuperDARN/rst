# Makefile for fitacfserver
# =========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -DPOSIX -D__EXTENSIONS__
OBJS = loginfo.o fork.o  fitpacket.o \
	socket.o connect.o msgmem.o fitacfserver.o 
SRC=hlpstr.h errstr.h connect.c connect.h fitacfserver.c loginfo.c loginfo.h cnxstr.h fork.c fork.h fitpacket.c fitpacket.h msgmem.c msgmem.h socket.c socket.h

LIBS=-loldfit.1 -lcnx.1 -lfit.1 -lcfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lrtime.1 -lopt.1 -lrcnv.1
SLIB=-lm -lz 
DSTPATH = $(BINPATH)
OUTPUT = fitacfserver

include $(MAKEBIN).$(SYSTEM)
