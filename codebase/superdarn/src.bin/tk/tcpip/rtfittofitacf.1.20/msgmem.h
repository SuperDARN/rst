/* msgmem.h
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



int writeraw(char *buf,int sze); 
void readsock(fd_set *fdset,char *tmp_buf,int tmp_sze);
int writesock();



