/* maptometaxml.c
   ============== 
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "hlpstr.h"



struct CnvMapData *map;
struct GridData *grd;

char *model_dir[]={
    "Bz+","Bz+/By+","By+","Bz-/By+","Bz-",
    "Bz-/By-","By-","Bz+/By-",0};

char *model_mag[]={"0<BT<4","4<BT<6","6<BT<12",0};
char *model_mag_ent[]={"0&lt;BT&lt;4","4&lt;BT&lt;6","6&lt;BT&lt;12",0};

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: maptometaxml --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  int old=0;

  int s=0;
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  
  FILE *fp;   
  char *fname=NULL;
  int yr,mo,dy,hr,mt;
  double sc;
  int c;

  grd=GridMake();
  map=CnvMapMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);

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


  if (arg !=argc) fname=argv[arg];
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  fprintf(stdout,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
  fprintf(stdout,"<data>\n");

  if (old) s=OldCnvMapFread(fp,map,grd);
  else s=CnvMapFread(fp,map,grd);

  while (s !=-1) {
    fprintf(stdout,"<map>\n");
    TimeEpochToYMDHMS(map->st_time,&yr,&mo,&dy,&hr,&mt,&sc);    
    fprintf(stdout,"<UTCtime>\n");
    fprintf(stdout,"<start>\n");
    fprintf(stdout,"<YMDHMS>\n");
    fprintf(stdout,"<year>%.4d</year>\n",yr);
    fprintf(stdout,"<month>%.2d</month>\n",mo);
    fprintf(stdout,"<day>%.2d</day>\n",dy);
    fprintf(stdout,"<hour>%.2d</hour>\n",hr);
    fprintf(stdout,"<minute>%.2d</minute>\n",mt);
    fprintf(stdout,"<second>%g</second>\n",sc);
    fprintf(stdout,"</YMDHMS>\n");
    fprintf(stdout,"</start>\n");
    TimeEpochToYMDHMS(map->ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"<end>\n");
    fprintf(stdout,"<YMDHMS>\n");
    fprintf(stdout,"<year>%.4d</year>\n",yr);
    fprintf(stdout,"<month>%.2d</month>\n",mo);
    fprintf(stdout,"<day>%.2d</day>\n",dy);
    fprintf(stdout,"<hour>%.2d</hour>\n",hr);
    fprintf(stdout,"<minute>%.2d</minute>\n",mt);
    fprintf(stdout,"<second>%g</second>\n",sc);
    fprintf(stdout,"</YMDHMS>\n");
    fprintf(stdout,"</end>\n");
    fprintf(stdout,"</UTCtime>\n");
    fprintf(stdout,"<version>\n");
    fprintf(stdout,"<major>%d</major><minor>%d</minor>\n",map->major_rev,
            map->minor_rev);
    fprintf(stdout,"</version>\n");
    fprintf(stdout,"<source>%s</source>\n",map->source);
    if (map->hemisphere==1) fprintf(stdout,"<hemisphere>north</hemisphere>\n");
    else fprintf(stdout,"<hemisphere>south</hemisphere>\n");
    fprintf(stdout,"<order>%d</order>\n",map->fit_order);
    fprintf(stdout,"<latmin>%g</latmin>\n",map->latmin);
    fprintf(stdout,"<imf>\n");
    fprintf(stdout,"<gsm>\n");
    fprintf(stdout,"<bx>%g</bx><by>%g</by><bz>%g</bz>\n",map->Bx,map->By,map->Bz);
    fprintf(stdout,"</gsm>\n");
    fprintf(stdout,"</imf>\n");
    fprintf(stdout,"<imfdelay>%d</imfdelay>\n",map->imf_delay*60);
    fprintf(stdout,"<model>\n");
    fprintf(stdout,"<angle>%s</angle>\n",map->imf_model[0]);
    for (c=0;model_mag[c] !=0;c++) 
      if (strcmp(map->imf_model[1],model_mag[c])==0) break;
    fprintf(stdout,"<magnitude>%s</magnitude>\n",model_mag_ent[c]);
    fprintf(stdout,"</model>\n");
    fprintf(stdout,"<vectors>\n");
    fprintf(stdout,"<radars>%d</radars>\n",grd->stnum);
    fprintf(stdout,"<total>%d</total>\n",grd->vcnum);
    fprintf(stdout,"</vectors>\n");
    fprintf(stdout,"<potential>\n");
    fprintf(stdout,"<delta>\n");
    fprintf(stdout,"<value>%g</value><error>%g</error>\n",map->pot_drop,
                   map->pot_drop_err);
    fprintf(stdout,"</delta>\n");
    fprintf(stdout,"</potential>\n");
    fprintf(stdout,"<chisquared>\n");
    fprintf(stdout,"<true>%g</true>\n",map->chi_sqr);
    fprintf(stdout,"<vector>%g</vector>\n",map->chi_sqr_dat);
    fprintf(stdout,"</chisquared>\n");
    fprintf(stdout,"</map>\n");

    if (old) s=OldCnvMapFread(fp,map,grd);
    else s=CnvMapFread(fp,map,grd);

  }
  fprintf(stdout,"</data>\n");
  fclose(fp);
  return 0;
}






















