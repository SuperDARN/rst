/* shmem.c
   =======
   Author R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

unsigned char *ShMemAlloc(char *memname,int size,int flags,int unlink,
			  int *fdptr) {

  int s;
  unsigned char *p;
  int fd;

  if (unlink) {
    s=shm_unlink(memname); /* ignore errors on this */
  }
  
  fd=shm_open(memname,flags,0777);
  if (fd==-1) {
    return NULL;
  }

  if (flags & O_CREAT) {
    s=ftruncate(fd,size);
    if (s==-1) {
      close(fd);
      return NULL;
    }
  }

  p=mmap(0,size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if ((int) p==-1) {
    close(fd);
    return NULL;
  }

  if (fdptr !=NULL) *fdptr=fd;
  return p;

}

int ShMemFree(unsigned char *p,char *memname,int size,int unlink,int fd) {
  int s=0;
  munmap(p,size);
  close(fd);
  if (unlink) s=shm_unlink(memname);
  return s;
}

