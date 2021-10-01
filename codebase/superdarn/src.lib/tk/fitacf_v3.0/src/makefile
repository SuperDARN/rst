# makefile for fitacf 3.0
# =====================
#
# Disclaimer:  RST is licensed under GPL v3.0. Please visit 
#              <https://www.gnu.org/licenses/> to see the full license
#
# Modifications:
#  Angeline G. Burrell - Modified to use new stats library

include $(MAKECFG).$(SYSTEM)

INCLUDE	= -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
		-I$(IPATH)/superdarn/fit -I$(IPATH)/superdarn
INC	= $(IPATH)/superdarn
DSTPATH	= $(LIBPATH)
OUTPUT	= fitacf
LINK	= "3.0"

SRC	= determinations.c fitacftoplevel.c fitting.c leastsquares.c \
		llist.c preprocessing.c determinations.h fit_structures.h \
		leastsquares.h preprocessing.h fitacftoplevel.h fitting.h \
		llist.h
OBJS	= determinations.o  fitacftoplevel.o fitting.o leastsquares.o  \
		llist.o preprocessing.o

include $(MAKELIB).$(SYSTEM)
