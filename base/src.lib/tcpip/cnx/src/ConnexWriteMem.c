/* ConnexWriteMem.c
   ================
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "connex.h"

unsigned char buffer[4096];
int bufsze=4096;
int bufptr=0;

int main(int argc,char *argv[]) {

 int sock,outs,i=0,size;
 unsigned int clength,length;
 
  int port=0;
  fd_set ready;

  struct sockaddr_in server;
  struct sockaddr_in client;

  struct hostent *gethostbyname();
  struct hostent *cname;


  time_t ltime;
  struct tm *time_of_day;
  char *date;
  int status=0;

  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) {
    perror("opening stream socket");
    exit(1);
  }

  /* name and bind socket to an address and port number */

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  if (port !=0) server.sin_port=htons(port); 
  else server.sin_port=0;
  
  if (bind(sock,(struct sockaddr *) &server,sizeof(server))) {
     perror("binding stream socket");
     exit(1);
  }

  /* Find out assigned port number and print it out */

  length=sizeof(server);
  if (getsockname(sock,(struct sockaddr *) &server,&length)) {
     perror("getting socket name");
     exit(1);
  }

  fprintf(stdout,"Socket has port #%d\n",ntohs(server.sin_port));

  listen(sock,5); /* mark our socket willing to accept connections */

  do {

    /* block until someone wants to attach to us */

    FD_ZERO(&ready);
    FD_SET(sock,&ready);
    if (select(sock+1,&ready,0,0,NULL) < 0) { 
      perror("while testing for connections");
      continue;
    }
     
    fprintf(stdout,"Accepting a new connection...\n");
    clength=sizeof(client);
    outs=accept(sock,(struct sockaddr *) &client,&clength);
        
    if (outs==-1) {
       perror("accept"); 
       continue;
    }

    cname=gethostbyaddr((char *) &client.sin_addr,
                        sizeof (client.sin_addr),AF_INET);

    if ((cname !=NULL) && (cname->h_name !=NULL))
      fprintf(stdout,"[%s (%s)]\n",cname->h_name,inet_ntoa(client.sin_addr));
    else fprintf(stdout,"[%s]\n",inet_ntoa(client.sin_addr));


    /* create the child process to send messages to the client */

    if (fork() == 0) {
      close(sock);

      /* this is the child process */

      do {
   
        time(&ltime);  
        time_of_day=localtime(&ltime);

        date=asctime(time_of_day);
        fprintf(stdout,"Sending message.\n");  
        size=ConnexWriteMem(buffer+bufptr,bufsze,(unsigned char *)date,
                            strlen(date)+1);
        bufptr+=size;
        i++;
        if ((i>0) && ((i % 5)==0)) {
          write(outs,buffer,bufptr);
          bufptr=0;
        }
        sleep(1);
      } while (status !=-1);
      fprintf(stdout,"Connection closes.\n");
      close(outs);
    
      break;
        
      
    }
    close(outs); 
  } while(1);

  return 0;

}
   

 










