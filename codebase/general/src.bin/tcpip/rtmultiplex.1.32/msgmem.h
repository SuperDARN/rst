/* msgmem.h
   ========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




int writeraw(unsigned char *buf,int sze); 
void readsock(fd_set *fdset,unsigned char *tmp_buf,int tmp_sze);
int writesock();



