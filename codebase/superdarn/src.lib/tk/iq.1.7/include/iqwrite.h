/* iqwrite.h
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _IQWRITE_H
#define _IQWRITE_H


int IQEncode(struct DataMap *ptr,struct IQ *iq,unsigned int *badtr,int16 *samples);

int IQWrite(int fid,struct RadarParm *prm,
	    struct IQ *iq,unsigned int *badtr,int16 *samples);

int IQFwrite(FILE *fp,struct RadarParm *prm,
	     struct IQ *iq,unsigned int *badtr,int16 *samples);

#endif

