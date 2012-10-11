# Makefile for rtserver
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = rtserver.o connect.o fitpacket.o fork.o msgmem.o socket.o  
SRC=hlpstr.h errstr.h rtserver.c connect.c connect.h fitpacket.c fitpacket.h fork.c fork.h 0 msgmem.c msgmem.h socket.c socket.h 

DSTPATH = $(BINPATH)
OUTPUT = rtserver
LIBS=-lerrlog.1 -lfit.1 -lcfit.1 -lrscan.1 -lfio.1 -lrmsgrcv.1 -ltcpipmsg.1 -lcnx.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1 -lopt.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)

