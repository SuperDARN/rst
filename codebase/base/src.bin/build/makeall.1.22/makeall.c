/* makeall.c
   =========
   Author: R.J.Barnes

   LICENSE AND DISCLAIMER

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.



 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include "hlpstr.h"
#include "errstr.h"




int vflg=1; /* compile only most recent version of code */
int num=0;
int lnum=0;
char *makelist[4096];
char *makename[4096];
char *makeroot[4096];
int makevmajor[4096];
int makevminor[4096];
char maketype[4096];
char line[256];
int vbflg=0;
char *src=NULL;
char *patn=NULL;


void log_info(char *text) {
  if (vbflg==1) fprintf(stderr,"%s",text);
  fprintf(stdout,"%s",text);
  fflush(stdout);
}

int do_make(char *target,char *path) {
  int status;
  char tmpname[256];
  time_t ltime;
  pid_t pid;

  pid=getpid();
  time(&ltime);

  sprintf(tmpname,"log.%d.%d",pid,(int) ltime);

  pid=fork();
  if (pid==-1) return -1;
  if (pid==0) { /* execute the make command here */
    int s;
    FILE *ret;
    ret=freopen(tmpname,"a+",stderr);
    if (ret == NULL) return -1;
    ret=freopen(tmpname,"a+",stdout);
    if (ret == NULL) return -1;

    if (target !=NULL) s=execlp("make","make",target,NULL);
    else s=execlp("make","make",NULL);

    exit(s);
  }
  do {
     if (waitpid(pid, &status, 0) == -1) {
       FILE *fp;
       /* now display log message and delete */
       fp=fopen(tmpname,"r");
       while(fgets(line,256,fp) !=NULL) {
         log_info(line);
       }
       fclose(fp);
       unlink(tmpname);
       return status;
     }
  } while(1);
}

int instr(char *body,char *str) {

  int i=0;
  int j=0;
  if (str==NULL) return 0;
  while (body[i] !=0) {
    if (body[i]==str[j]) j++;
    else j=0;
    if (str[j]==0) break;
    i++;
  }
  if (body[i]==0) return -1;
  return i-j+1;
}

void add_make(char *name,char type) {

  int i,j,k;
  int major=0,minor=0;

  i=strlen(src)+1;

  /* the end of the path is the version number if any */

  for (j=strlen(name)-1;(name[j] !='.') && (j>0);j--);
  if (j>0) {
    minor=atoi(name+j+1); /* +1 bugfix found by SGS 12Dec2012 */
    for (k=j-1;(name[k] !='.') && (k>0);k--);
    if (k>0) {
      name[j]=0;
      major=atoi(name+k+1); /* +1 bugfix found by SGS 12Dec2012 */
      name[j]='.';
      j=k;
    } else {
      minor=0;
      j=strlen(name);
    }
  } else j=strlen(name);

  if (vflg==1) {
    for (k=0;k<num;k++) {
      if ((strlen(makeroot[k])==(j-i)) &&
          (strncmp(makeroot[k],name+i,j-i)==0)) break;
    }
    if (k<num) {

      /* now compare version numbers */
      if (major<makevmajor[k]) return;
      if (minor<makevminor[k] && major == makevmajor[k]) return;
			/* bugfix SGS: 17Feb2017 was returning when major> and minor< */

      makevmajor[k]=major;
      makevminor[k]=minor;
      makename[k]=realloc(makename[k],strlen(name)+1);
      strcpy(makename[k],name);
      return;
    }
  }
  makename[num]=malloc(strlen(name)+1);
  maketype[num]=type;
  makevmajor[num]=major;	/* added from SGS bugfix 12Dec2012 */
  makevminor[num]=minor;	/* added from SGS bugfix 12Dec2012 */
  strcpy(makename[num],name);
  name[j]=0;
  makeroot[num]=malloc(strlen(name+i)+1);
  strcpy(makeroot[num],name+i);
  num++;

}


int find_makefile(char *path) {
  struct dirent *direntp=NULL;
  DIR *dirp;
  int s;
  struct stat dstat;
  int state=0;
  char spath[4096];

  dirp=opendir(path);
  if (dirp==NULL) return 0;

  while ((direntp=readdir(dirp)) !=NULL) {
     if (direntp->d_name[0]=='.') continue;
     if ((strcmp(direntp->d_name,"makefile")==0) && (instr(path,patn) !=-1))
        add_make(path,'b');
     if (strcmp(direntp->d_name,"src")==0) state++;
     if (strcmp(direntp->d_name,"include")==0) state++;

     if (state==2) { /* possible library source directory */
       struct dirent *direntpl=NULL;
       DIR *dirpl;
       sprintf(spath,"%s/src",path);
       dirpl=opendir(spath);
       if (dirpl==NULL) break;
       while ((direntpl=readdir(dirpl)) !=NULL) {

         if (direntpl->d_name[0]=='.') continue;
	 if ((strcmp(direntpl->d_name,"makefile")==0) &&
             (instr(path,patn)!=-1)) add_make(path,'l');
       }
       closedir(dirpl);
       break;
     }

     if (state !=0) continue;


     sprintf(spath,"%s/%s",path,direntp->d_name);
     s=stat(spath,&dstat);

     if ((s==0) && (S_ISDIR(dstat.st_mode))) {
       /* recurse here */
       find_makefile(spath);
     }
  }
  closedir(dirp);
  return 0;
}



void load_list(FILE *fp) {
  int i;
  char line[1024];
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i]==' ') && (line[i] !=0);i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

    memmove(line,line+i,strlen(line)+1-i);
    for (i=0;(line[i] !=0) && (line[i] !='\n') && (line[i] !=' ');i++);
    line[i]=0;
    if (strlen(line)==0) continue;
    makelist[lnum]=malloc(strlen(line)+1);
    strcpy(makelist[lnum],line);
    lnum++;

  }
}

int dsort(const void *a,const void *b) {
  char **x;
  char **y;
  x=(char **) a;
  y=(char **) b;
  return strcmp(*x,*y);
}

void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,"%s",str[i]);
}



int main(int argc,char *argv[]) {

  char logstr[1024];
  char path[4096];

  char sep[256];
  int i,j,status,s;
  int aflg=1;
  char *target=NULL;

  for (i=0;i<80;i++) sep[i]='=';
  sep[i]='\n';
  sep[i+1]=0;



  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-a")==0) vflg=0;
    else if (strcmp(argv[i],"-q")==0) aflg=0;
    else if (strcmp(argv[i],"-vb")==0) vbflg=1;
    else if (strcmp(argv[i],"-p")==0) {
      patn=argv[i+1];
      i++;
    } else if (strcmp(argv[i],"-t")==0) {
      target=argv[i+1];
      i++;
    } else if (strcmp(argv[i],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } else break;
  }

  if (argc<(1+i)) {
    print_info(stderr,errstr);
    exit(1);
  }

  src=argv[i];
  if (argc>(i+1)) {
    if (strcmp(argv[i+1],"stdin")==0) load_list(stdin);
    else {
      for (j=0;j<(argc-i-1);j++) makelist[j]=argv[j+i+1];
      lnum=j;
    }
  }



  find_makefile(src);

  log_info("makeall\n");


  sprintf(logstr,"Package Source Directory:%s\n",src);
  log_info(logstr);

  if (aflg !=0) sprintf(logstr,"Abort on failure:yes\n");
  else sprintf(logstr,"Abort on failure:no\n");

  log_info(logstr);



  if (vflg !=0) sprintf(logstr,
                "Compile most recent version of code only:yes\n");
  else sprintf(logstr,"Compile most recent version of code only:no\n");

  log_info(logstr);
  log_info(sep);

  if (lnum !=0) {
    sprintf(logstr,"Compilation Order List:\n");
    log_info(logstr);
    for  (i=0;i<lnum;i++) {
      sprintf(logstr,"%d:%s\n",i+1,makelist[i]);
      log_info(logstr);
    }
  }

  if (lnum==0) {
    lnum=num;
    for (i=0;i<num;i++) makelist[i]=makeroot[i];

    /* sort the make list into alphabetical order */
       qsort(makelist,num,sizeof(char *),dsort);



  }


  log_info("Located Source Code:\n");

  for (i=0;i<lnum;i++) {
    for (j=0;j<num;j++) if (strcmp(makelist[i],makeroot[j])==0) break;
    if (j==num) continue;
    if (maketype[j]=='b') sprintf(logstr,"%d.Located Binary:%s\n",
                                  i+1,makename[j]);
    else sprintf(logstr,"%d.Located Library:%s\n",i+1,makename[j]);
    log_info(logstr);
  }


  for (i=0;i<lnum;i++) {
     for (j=0;j<num;j++) if (strcmp(makelist[i],makeroot[j])==0) break;
    if (j==num) continue;
    log_info(sep);
    if (maketype[j]=='b') sprintf(logstr,"Compiling Binary:%s\n",makename[j]);
    else sprintf(logstr,"Compiling Library:%s\n",makename[j]);
    log_info(logstr);
    log_info(sep);



    if (maketype[j]=='b') sprintf(path,"%s",makename[j]);
    else sprintf(path,"%s/src",makename[j]);
    s=chdir(path);
    if (s !=0) {
      fprintf(stderr,"chdir failed.\n");
      exit(-1);
    }


    if (target==NULL) {
      log_info("make clean\n");
      status=do_make("clean",path);
      if ((aflg !=0) && (status !=0)) break;
    }
    if (target==NULL) log_info("make\n");
    else {
      sprintf(logstr,"make %s\n",target);
      log_info(logstr);
    }

    status=do_make(target,path);
    if ((aflg !=0) && (status !=0)) break;

  }
  if (i<lnum) {
    log_info("Compilation Aborted.\n");
    return 1;
  }



  return 0;
}








