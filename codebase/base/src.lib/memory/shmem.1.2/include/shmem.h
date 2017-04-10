/* shmem.h
   =======
   Author R.J.Barnes
*/

/*
   See license.txt
*/

#ifndef _SHMEM_H
#define _SHMEM_H

unsigned char *ShMemAlloc(char *memname,int size,int flags,int unlink,
			  int *fdptr);
int ShMemFree(unsigned char *p,char *memname,int size,int unlink,int fd);

#endif
