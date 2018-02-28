# makefile for fitacf 3.0
#
#
include $(MAKECFG).$(SYSTEM)



INCLUDE= -I$(IPATH)/base -I$(IPATH)/general  -I$(IPATH)/superdarn/fit -I$(IPATH)/superdarn
SRC = determinations.c fitacftoplevel.c fitting.c leastsquares.c llist.c \
	preprocessing.c determinations.h fit_structures.h leastsquares.h \
	preprocessing.h fitacftoplevel.h fitting.h llist.h
OBJS = determinations.o  fitacftoplevel.o  fitting.o  leastsquares.o  llist.o \
	preprocessing.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT=fitacf
LINK="3.0"



include $(MAKELIB).$(SYSTEM)
