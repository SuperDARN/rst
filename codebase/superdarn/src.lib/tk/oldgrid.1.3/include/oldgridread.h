/* oldgridread.h
   =============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _OLDGRIDREAD_H
#define _OLDGRIDREAD_H

int OldGridDecodeOne(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr);

int OldGridDecodeTwo(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr);

int OldGridDecodeThree(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr);

int OldGridDecodeFour(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr);

int OldGridDecodeFive(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr);

int OldGridDecodeSix(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct RfileData *data,void *ptr);


int OldGridFread(FILE *fp,struct GridData *);

int OldGridFseek(FILE *fp,
	         int yr,int mo,int dy,int hr,int mt,int sc,
                 struct RfileIndex *inx,
                 double *aval);
#endif
