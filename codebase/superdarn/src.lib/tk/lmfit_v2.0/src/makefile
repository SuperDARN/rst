# makefile for lmfit 2.0

include $(MAKECFG).$(SYSTEM)



INCLUDE= -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/analysis

SRC = lmfit_determinations.c error_estimates.c lmfit_fitting.c lmfit_leastsquares.c llist.c lmfit2toplevel.c lmfit_preprocessing.c selfclutter.c lmfit_determinations.h error_estimates.h lmfit_fitting.h lmfit_leastsquares.h llist.h lmfit2toplevel.h lmfit_preprocessing.h selfclutter.h
OBJS = lmfit_determinations.o error_estimates.o lmfit_fitting.o lmfit_leastsquares.o llist.o lmfit2toplevel.o lmfit_preprocessing.o selfclutter.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT=lmfit
LINK="2.0"



include $(MAKELIB).$(SYSTEM)
