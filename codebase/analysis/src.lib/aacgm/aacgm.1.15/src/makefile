# Makefile for aacgm library
# ==========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general
SRC = math.c math.h aacgm.c altitude_to_cgm.h altitude_to_cgm.c \
      cgm_to_altitude.c cgm_to_altitude.h coeff.c \
      convert_geo_coord.c convert_geo_coord.h \
      rylm.c rylm.h default.h   
OBJS = math.o aacgm.o altitude_to_cgm.o cgm_to_altitude.o coeff.o \
       convert_geo_coord.o rylm.o 
INC=$(IPATH)/analysis
DSTPATH=$(LIBPATH)
LINK="1"
OUTPUT = aacgm

 

 





include $(MAKELIB).$(SYSTEM)
