/* acfex.h
   =======
   Author: R.J.Barnes
*/

/* 
   See license.txt
*/

#ifndef _ACFEX_H
#define _ACFEX_H

int ACFexCalculate(struct TSGprm *prm,int16 *iqptr,int iqnum,
		 int nave,int nsample,int roffset,int ioffset,
                 int mplgs,int lagnum,int *lagtable[2],int *lagsum,
                 float *pwr0,float *acfbuf,float *noise);

#endif
