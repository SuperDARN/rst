/* msg_mem.h
   =========
   Author: R.J.Barnes
*/

int write_raw(char *buf,int sze); 
void read_sock(fd_set *fdset,char *tmp_buf,int tmp_sze);
int write_sock();



