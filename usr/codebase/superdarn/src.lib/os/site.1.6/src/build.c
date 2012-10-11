/* site.c
   ====== 
   Author R.J.Barnes & J.Spaleta
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
  
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"
#include "maketsg.h"
#include "acf.h"
#include "tcpipmsg.h"
#include "rosmsg.h"
#include "shmem.h"
#include "global.h"
#include "site.h"
#include "siteglobal.h"

char *sitefn[]={"Site%sStart",
		"Site%sSetupRadar",
		"Site%sStartScan",
		"Site%sStartIntt",
		"Site%sFCLR",
		"Site%sTimeSeq",
		"Site%sIntegrate",
		"Site%sEndScan",
		"Site%sExit",
                0};

void **siteptr[]={ (void **) &sitelib.start,
		   (void **) &sitelib.setupradar,
		   (void **) &sitelib.startscan,
		   (void **) &sitelib.startintt,
		   (void **) &sitelib.fclr,
		   (void **)  &sitelib.tmseq,
		   (void **) &sitelib.integrate,
		   (void **) &sitelib.endscan,
		   (void **) &sitelib.exit};


void *dlhandle=NULL;

int SiteBuild(char *stid,char *vstr) {

  char dlname[256];
  char symbol[256];
  char st[6];
  

  char *error;
  int n=0;

  if (vstr !=NULL) sprintf(dlname,"libsite.%s.%s.so",stid,vstr);
  else  sprintf(dlname,"libsite.%s.1.so",stid);
  
  sprintf(st,"%c%s",toupper(stid[0]),stid+1);

  dlhandle = dlopen(dlname,RTLD_NOW | RTLD_GLOBAL);
  if (!dlhandle) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  dlerror(); /* Clear any existing error */

  while (sitefn[n] !=0) {
    sprintf(symbol,sitefn[n],st);

    fprintf(stderr,"%s\n",symbol);

    *(siteptr[n]) = dlsym(dlhandle,symbol);

    if ((error = dlerror()) != NULL)  {
      fprintf(stderr, "%s\n", error);
      exit(EXIT_FAILURE);
    }
    n++;

  }
  return 0;
}





