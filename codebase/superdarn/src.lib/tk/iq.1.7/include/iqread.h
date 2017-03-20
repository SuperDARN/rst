/* iqread.h
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _IQREAD_H
#define _IQREAD_H

int IQDecode(struct DataMap *ptr,struct IQ *iqdata,unsigned int **badtr,int16 **samples);

int IQRead(int fid,struct RadarParm *prm,
	   struct IQ *iq,unsigned int **badtr,int16 **samples);

int IQFread(FILE *fp,struct RadarParm *prm,
	    struct IQ *iq,unsigned int **badtr,int16 **samples);

#endif

