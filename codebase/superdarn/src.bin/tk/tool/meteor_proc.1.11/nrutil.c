/* nrutil.c
   ======== */

/*
$Log: nrutil.c,v $
Revision 1.2  2005/07/15 18:26:53  barnes
Numerical recipe routines.

*/


/*
Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
All rights reserved.

This material may be used, modified, or reproduced by or for the U.S.
Government pursuant to the license rights granted under the clauses at DFARS
252.227-7013/7014.

For any other permissions, please contact the Space Department
Program Office at JHU/APL.

This Distribution and Disclaimer Statement must be included in all copies of
"Radar Software Toolkit - Meteor Echo Software" (hereinafter "the Program").

The Program was developed at The Johns Hopkins University/Applied Physics
Laboratory (JHU/APL) which is the author thereof under the "work made for
hire" provisions of the copyright law.  

JHU/APL assumes no obligation to provide support of any kind with regard to
the Program.  This includes no obligation to provide assistance in using the
Program or to provide updated versions of the Program.

THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
PROGRAM."






*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#define NR_END 1
#define FREE_ARG char*

void nrerror(char *error_text) {
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}



double *dvector(long nl, long nh) {
	double *v;
	v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
	if (!v) nrerror("allocation failure in dvector()");
	return v-nl+NR_END;
}


double **dmatrix(long nrl, long nrh, long ncl, long nch) {
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	double **m;

	/* allocate pointers to rows */
	m=(double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	/* allocate rows and set pointers to them */
	m[nrl]=(double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;
}

void free_dvector(double *v, long nl, long nh) {
	free((FREE_ARG) (v+nl-NR_END));
}

void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch) {
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}

