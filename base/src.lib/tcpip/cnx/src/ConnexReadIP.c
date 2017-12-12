/* ConnexReadIP.c
   ==============
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "connex.h"

int main(int argc,char *argv[]) {
  int sock;
  int port=0;
  char *host=NULL;
  int size;
  unsigned char *buffer=NULL;
 
  
  if (argc<3) { 
    fprintf(stderr,"host and port must be specified.\n");
    exit(-1);
  }

  host=argv[1];
  port=atoi(argv[2]);
 
  sock=ConnexOpen(host,port); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  do {
   size=ConnexReadIP(sock,&buffer);
   if (size==-1) break;
   fprintf(stderr,"Message size:%d\n",size);
     
  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;

}
   

 










