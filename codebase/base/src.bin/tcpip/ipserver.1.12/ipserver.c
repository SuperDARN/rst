/* ipserver.c
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "option.h"
#include "connex.h"
#include "log_info.h"

#include "errstr.h"
#include "hlpstr.h"
#include "fork.h"




int outpipe=-1;
int resetflg=1;

char *dfpname={"port.id"};
char *dfpidname={"pid.id"};
char *dflogfname={"log.rt"};


char *pname=NULL;
char *pidname=NULL;
extern char *logfname;

struct OptionData opt;

void trap_pipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

void trap_signal(int signal) {
  resetflg=1;
}


int main(int argc,char *argv[]) {
  
  int port=0,arg=0;
  int status;
  

  char *date;
  

  unsigned char help=0; 
  unsigned char option=0; 
 

  sigset_t set;
  struct sigaction act;
  
  time_t ltime;
  struct tm *time_of_day;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"L",'t',&logfname);
  OptionAdd(&opt,"pf",'t',&pname);
  OptionAdd(&opt,"if",'t',&pidname);
  OptionAdd(&opt,"lp",'i',&port);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (logfname==NULL) logfname=dflogfname;
  if (pname==NULL) pname=dfpname;
  if (pidname==NULL) pidname=dfpidname;

  signal(SIGCHLD,SIG_IGN); 

  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);
  sigaddset(&set,SIGPIPE);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_signal;
  sigaction(SIGUSR1,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_pipe;
  sigaction(SIGPIPE,&act,NULL);

  outpipe=fork_inet(port);
  
  status=0;
 
  do {
 
    /* get date */
    fprintf(stderr,".");
    fflush(stderr);

    time(&ltime);  
    time_of_day=localtime(&ltime);

    date=asctime(time_of_day);  

    if (outpipe==-1) {
      log_info("Child process died - Restarting.");
      outpipe=fork_inet(port);
    }
    if (outpipe !=-1) status=ConnexWriteIP(outpipe,(void *) date,
                                          strlen(date)+1);
    
    sleep(5);
    status=0;

  } while(status==0);
  return 0;

}
   

 






















