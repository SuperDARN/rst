/* fit_speck_removal.c
   ==========
   
Removes salt & pepper noise from a fitacf file. 
The quality flag (fit->qflg) in the center cell of a 3x3 range-time grid is set to zero if the median 
of the quality flags in the 3x3 grid is zero. This procedure is performed separately for each beam and channel. 
The output is a fitacf file with the salt & pepper noise removed, but otherwise identical to the input file.

(C) Copyright 2021 E.C.Bland
author: E.C.Bland

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
       
Modifications:
    2021-09-17 E.C.Bland, University Centre in Svalbard (UNIS): fix handling of qflg>1 data from fitacf2.5, 
	           which in rare cases might include values other than qflg=0 for rejected ACFs.
    2022-02-23 E.C.Bland, (UNIS): add statistics on number of rejected ACFs
    2024-04-04 E.C.Bland: Bugfix - xqflg also needs to be set to zero to properly remove isolated ACFs
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <zlib.h>
#include <errno.h>

#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

// Define maximum number of time records, beams, channels and range gates (used for memory allocation)
#define tmax 2000       // initial number of time records
#define maxbeam 30      // max number of beams
#define maxchannel 3    // max number of channels (0, 1, or 2)
#define maxrange 250    // max number of range gates

int fnum=0;

struct RadarParm *prm;
struct FitData *fit;

struct OptionData opt;


int rst_opterr (char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: trim_fit --help\n");
  return(-1);
}


// function to return the index in an array
int get_index(int a, int b, int c, int d, int aSize, int bSize, int cSize) {
    return (d * aSize * bSize * cSize) + (c * aSize * bSize) + (b * aSize) + a; 
}

// structure to store the qflgs. Can be dynamically resized
typedef struct {
  int *value;
  size_t used;
  size_t size;
} qflgData;

// function to free memory when program exits
void free_parameters(struct RadarParm *prm, struct FitData *fit, FILE *fp, qflgData *q)
{
  if (prm != NULL) RadarParmFree(prm);
  if (fit != NULL) FitFree(fit);
  if (fp !=NULL) free(fp);
  if (q->value !=NULL) {
    free(q->value);
    q->value=NULL;
  }
}



int main (int argc,char *argv[]) {
  
  unsigned char vb=0;
  unsigned char quiet=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"quiet",'x',&quiet);

  int arg;
  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }
  
  FILE *fp=NULL;
  fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  
  //set origin.command field
  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];
  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) 
        break;
    if (c !=0) 
        strcat(command," ");
    strcat(command,argv[c]);
  }
  
  
  prm=RadarParmMake();
  fit=FitMake();
  if (FitFread(fp,prm,fit)==-1) {
    fprintf(stderr,"Error reading file\n");
    free_parameters(prm, fit, fp, NULL);
    exit(-1);
  }
  
  // Initial memory allocation to store qflgs
  qflgData *qflgs=malloc(sizeof(qflgData));
  qflgs->value = malloc(tmax*maxrange*maxchannel*maxbeam * sizeof(int));
  if ((qflgs->value == NULL) || (errno > 1)) {
    fprintf(stderr,"Error: %s\n", strerror(errno));
    free_parameters(prm, fit, fp, qflgs);
    exit(-1);
  }
  qflgs->used = 0;
  qflgs->size = tmax*maxrange*maxchannel*maxbeam;
  
  
  // Read the FITACF file to populate the qflg array
  int index;
  int maxindex=-1;
  int beam, channel;
  int nrec[maxbeam][maxchannel]; // counts number of records for each beam/channel
  memset(nrec,0,sizeof(nrec));
  if (nrec==NULL) {
    fprintf(stderr,"Error: %s\n", strerror(errno));
    free_parameters(prm, fit, fp, qflgs);
    exit(-1);
  }
  
  do {
  
    beam=prm->bmnum;
    channel=prm->channel;
    for (int range=0;range<prm->nrang;range++) {
      index=get_index(beam,channel,range,nrec[beam][channel],maxbeam,maxchannel,maxrange);
      if (maxindex < index) 
          maxindex=index;
      if (qflgs->size < maxindex) {
        qflgs->size += tmax*maxrange*maxchannel*maxbeam;
        qflgs->value = realloc(qflgs->value, qflgs->size * sizeof(int));
        
        // check that memory was reallocated successfully
        if (qflgs->value==NULL) {
          fprintf(stderr,"Error: %s\n", strerror(errno));
          free_parameters(prm, fit, fp, NULL);
          exit(-1);
        }
      }
      
      // Populate the qflg array using the values from the input file
      // if fit->rng[range].qflg>1 (ref: fitacf2.5), we set qflg=0
      if (fit->rng[range].qflg==1) {
        qflgs->value[index] = 1;
      } else {      
        qflgs->value[index] = 0;
      }
      qflgs->used = maxindex;
      
    }
    nrec[beam][channel]++;
  
  } while (FitFread(fp,prm,fit) !=-1);
  
  
  // Identify isolated points by calculating the median of the quality flags
  //   in a 3x3 grid. Since qflg can only be 0 or 1, the median qflg of the 
  //   3x3 grid can be calculated using the test ( sum_of_qflgs >= 5 ).
  //   Replicate padding is used to handle corner/edge cases
  int sum;
  int index_list[9];  
  memset(index_list,-1,sizeof(index_list));
  if (index_list==NULL) {
    fprintf(stderr,"Error: %s\n", strerror(errno));
    free_parameters(prm, fit, fp, qflgs);
    exit(-1);
  }
  
  
  //** Read the file again, remove the isolated points, and write a new file
  rewind(fp); // rewind the file pointer
  if (FitFread(fp,prm,fit)==-1) {
    fprintf(stderr,"Error reading file\n");
    free_parameters(prm, fit, fp, qflgs);
    exit(-1);
  }
  int status=0;
  int irec[maxbeam][maxchannel];
  memset(irec,0,sizeof(irec));
  if (irec==NULL) {
    fprintf(stderr,"Error: %s\n", strerror(errno));
    free_parameters(prm, fit, fp, qflgs);
    exit(-1);
  }
  int echoes_total=0;
  int echoes_removed=0;
  do {
  
    if (vb) {
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d %.2d %.2d\n",
              prm->time.yr,prm->time.mo,prm->time.dy,
              prm->time.hr,prm->time.mt,prm->time.sc,
              prm->channel,prm->bmnum);
    }
    
    beam=prm->bmnum;
    channel=prm->channel;
    for (int range = 0; range<prm->nrang; range++) {
      
      if (fit->rng[range].qflg==1) {
        index_list[0]=get_index(beam,channel,range  ,irec[beam][channel]  ,maxbeam,maxchannel,maxrange);
        index_list[1]=get_index(beam,channel,range-1,irec[beam][channel]  ,maxbeam,maxchannel,maxrange);
        index_list[2]=get_index(beam,channel,range+1,irec[beam][channel]  ,maxbeam,maxchannel,maxrange);
        index_list[3]=get_index(beam,channel,range,  irec[beam][channel]-1,maxbeam,maxchannel,maxrange);
        index_list[4]=get_index(beam,channel,range-1,irec[beam][channel]-1,maxbeam,maxchannel,maxrange);
        index_list[5]=get_index(beam,channel,range+1,irec[beam][channel]-1,maxbeam,maxchannel,maxrange);
        index_list[6]=get_index(beam,channel,range,  irec[beam][channel]+1,maxbeam,maxchannel,maxrange);
        index_list[7]=get_index(beam,channel,range-1,irec[beam][channel]+1,maxbeam,maxchannel,maxrange);
        index_list[8]=get_index(beam,channel,range+1,irec[beam][channel]+1,maxbeam,maxchannel,maxrange);
        
        // corners
        if (irec[beam][channel]==0 && range==0)
          sum = 3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[2]] 
              + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[8]];
        else if (irec[beam][channel]==0 && range==maxrange-1)
          sum = 3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] 
              + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[7]];
        else if (irec[beam][channel]==nrec[beam][channel] && range==0)
          sum = 3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[2]] 
              + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[5]];
        else if (irec[beam][channel]==nrec[beam][channel] && range==maxrange-1)
          sum = 3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] 
              + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[4]];
        
        // edges
        else if (irec[beam][channel]==0)
          sum = 2*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] 
              + 2*qflgs->value[index_list[2]] + qflgs->value[index_list[6]] 
              + qflgs->value[index_list[7]]   + qflgs->value[index_list[8]];
        else if (irec[beam][channel]==nrec[beam][channel])
          sum = 2*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] 
              + 2*qflgs->value[index_list[2]] + qflgs->value[index_list[3]] 
              + qflgs->value[index_list[4]]   + qflgs->value[index_list[5]];
        else if (range==0)
          sum = 2*qflgs->value[index_list[0]] + qflgs->value[index_list[2]] 
              + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[5]] 
              + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[8]];
        else if (range==maxrange-1)
          sum = 2*qflgs->value[index_list[0]] + qflgs->value[index_list[1]] 
              + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[4]] 
              + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[7]];
        
        // all other cells
        else 
          sum = qflgs->value[index_list[0]] + qflgs->value[index_list[1]] 
              + qflgs->value[index_list[2]] + qflgs->value[index_list[3]] 
              + qflgs->value[index_list[4]] + qflgs->value[index_list[5]] 
              + qflgs->value[index_list[6]] + qflgs->value[index_list[7]] 
              + qflgs->value[index_list[8]];
        
        // Remove record if median=0 (sum of qflgs < 5)
        if (sum < 5) 
        {
            fit->rng[range].qflg=0;
            if (prm->xcf!=NULL) fit->xrng[range].qflg=0;
            echoes_removed+=1;
        }
        echoes_total+=1;
      }
    }
    irec[beam][channel]++;
    
    
    // Set origin fields
    prm->origin.code = 1; // file was not generated at radar site
    ctime = time((time_t) 0);
    if (RadarParmSetOriginCommand(prm,command) == -1) 
    {
        fprintf(stderr,"Error: cannot set Origin Command\n");
        free_parameters(prm, fit, fp, qflgs);
        exit(-1);
    }

    strcpy(tmstr,asctime(gmtime(&ctime)));
    if (tmstr==NULL) {
      fprintf(stderr,"Error: %s\n", strerror(errno));
      free_parameters(prm, fit, fp, qflgs);
      exit(-1);
    }
    tmstr[24]=0;
    if (RadarParmSetOriginTime(prm,tmstr) == -1)
    {
        fprintf(stderr,"Error: cannot set Origin Time\n");
        free_parameters(prm, fit, fp, qflgs);
        exit(-1);
    }
    
    // write the output file
    status=FitFwrite(stdout,prm,fit);
    if (status ==-1) {
        fprintf(stderr,"Error writing output file.\n");
        exit(-1);
    }
  
  } while (FitFread(fp,prm,fit) !=-1);

  fclose(fp);
  
  // Print statistics
  if (quiet==0) 
     fprintf(stderr,"Number of echoes removed: %d of %d (%4.1f%%)\n",echoes_removed,echoes_total,100*(float)(echoes_removed)/(float)(echoes_total));

  // Free memory
  free_parameters(prm, fit, NULL, qflgs);
  return 0;
}
