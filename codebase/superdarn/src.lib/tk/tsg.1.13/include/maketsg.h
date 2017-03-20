/* maketsg.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _MAKETSG_H
#define _MAKETSG_H

void TSGFree(struct TSGbuf *ptr);
struct TSGbuf *TSGMake(struct TSGprm *tsg,int *flg);


#endif
