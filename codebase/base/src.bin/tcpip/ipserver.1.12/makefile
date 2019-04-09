# Makefile for ipserver
# =====================
# Author: R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -DPOSIX -D__EXTENSIONS__
OBJS = log_info.o fork.o socket.o connect.o msg_mem.o ipserver.o 
SRC=hlpstr.h errstr.h msg_mem.c msg_mem.h connect.c connect.h log_info.c log_info.h fork.c fork.h ipserver.c socket.c socket.h

LIBS=-lcnx.1 -lopt.1
DSTPATH = $(BINPATH)
OUTPUT = ipserver

include $(MAKEBIN).$(SYSTEM)
