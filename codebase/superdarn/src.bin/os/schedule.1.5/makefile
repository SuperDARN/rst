# Makefile for schedule
# =====================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = schedule.o log_info.o load_schedule.o print_schedule.o \
       set_schedule.o test_schedule.o refresh.o check_program.o execute.o
SRC=hlpstr.h errstr.h schedule.c log_info.c load_schedule.c print_schedule.c set_schedule.c \
    test_schedule.c refresh.c check_program.c execute.c

DSTPATH = $(BINPATH)
OUTPUT = schedule 
LIBS=-ltcpipmsg.1 -lrtime.1 -lopt.1
SLIB=-lm

include $(MAKEBIN).$(SYSTEM)

