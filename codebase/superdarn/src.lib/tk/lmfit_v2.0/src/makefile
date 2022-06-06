# makefile for lmfit 2.0

include $(MAKECFG).$(SYSTEM)



INCLUDE= -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/analysis

SRC = determinations.c error_estimates.c fitting.c leastsquares.c llist.c lmfit2toplevel.c mpfit.c preprocessing.c selfclutter.c determinations.h error_estimates.h fitting.h leastsquares.h llist.h lmfit2toplevel.h mpfit.h preprocessing.h selfclutter.h
OBJS = determinations.o error_estimates.o fitting.o leastsquares.o llist.o lmfit2toplevel.o mpfit.o preprocessing.o selfclutter.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT=lmfit
LINK="2.0"



include $(MAKELIB).$(SYSTEM)
