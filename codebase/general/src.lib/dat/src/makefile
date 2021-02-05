# Copyright (C) 2021 SuperDARN Canada, University of Saskatchewan
# Author: Marina Schmidt
#
# Modified
#
# Disclaimer:

 
*/include $(MAKECFG).$(SYSTEM)

INCLUDE= -g -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS=datread.o datreadcurrent.o datopen.o dattodmap.o datclose.o 
SRC=datreadcurrent.c hlpstr.h lmt.h raw.h errstr.h datclose.c raw_versions.h datopen.c datread.c datread.h dattodmap.c
INC= $(IPATH)/general
DSTPATH=$(LIBPATH)
OUTPUT = dat
LINK="1"
 

include $(MAKELIB).$(SYSTEM)
