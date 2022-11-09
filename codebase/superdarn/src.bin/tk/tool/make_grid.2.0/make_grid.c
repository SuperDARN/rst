/* make_grid.c
   ===========
*/

/*
 Copyright (C) <year>  <name of author>



 
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
 2021-01-11 Marina Schmidt fixing concatenation bug with start/end time, and deprecated -c flag
 2021-08-27 Angeline G Burrell removed duplicate code now present in libraries

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include <errno.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitscan.h"
#include "fitindex.h"
#include "fitseek.h"
#include "oldfitread.h"
#include "oldfitscan.h"
#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"
#include "cfitscan.h"
#include "fitscan.h"
#include "filter.h"
#include "bound.h"
#include "checkops.h"
#include "rpos.h"

#include "gtable.h"
#include "gtablewrite.h"
#include "oldgtablewrite.h"

#include "hlpstr.h"
#include "errstr.h"
#include "channel.h"

#include "aacgm.h"
#include "aacgmlib_v2.h"


struct RadarParm *prm;
struct FitData *fit;
struct CFitdata *cfit;
struct RadarScan *src[3];
struct RadarScan *dst;
struct RadarScan *out;

struct FitIndex *inx;

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

int nbox;

int ebmno=0;
int ebm[32*3];
int minrng=-1;
int maxrng=-1;
double minsrng=-1;
double maxsrng=-1;

struct GridTable *grid;


/**
 * Exclude scatter in range gates below minrng or beyond maxrng,
 * or from slant ranges below minsrng or beyond maxsrng. If range
 * gate and slant range thresholds are both provided, only the
 * slant range thresholds are considered.
 **/
void exclude_range(struct RadarScan *ptr,int minrng,int maxrng,
                   double minsrng,double maxsrng) {

    int bm,rng;
    int frang,rsep,rxrise,nrang;
    double r;
    double range_edge=0;

    /* Loop over number of beams in RadarScan structure */
    for (bm=0;bm<ptr->num;bm++) {

        /* If RadarBeam structure not set then continue */
        if (ptr->bm[bm].bm==-1) continue;

        /* If minsrng or maxsrng option set then exclude data using
         * slant range instead of range gate */
        if ((minsrng !=-1) || (maxsrng !=-1)) {

            /* Get radar operating parameters from RadarBeam structure */
            frang=ptr->bm[bm].frang;
            rsep=ptr->bm[bm].rsep;
            rxrise=ptr->bm[bm].rxrise;
            nrang=ptr->bm[bm].nrang;

            /* Calculate slant range to each range gate and compare to thresholds */
            for (rng=0;rng<nrang;rng++) {
                r=slant_range(frang,rsep,rxrise,range_edge,rng+1);
                if ((minsrng !=-1) && (r<minsrng)) ptr->bm[bm].sct[rng]=0;
                if ((maxsrng !=-1) && (r>maxsrng)) ptr->bm[bm].sct[rng]=0;
            }

        } else {
            /* If minrng option set then mark all scatter in range gates
             * less than minrng as being empty */
            if (minrng !=-1) for (rng=0;rng<minrng;rng++) ptr->bm[bm].sct[rng]=0;

            /* If maxrng option set then mark all scatter in range gates
             * greater than maxrng as being empty */
            if (maxrng !=-1) for (rng=maxrng;rng<ptr->bm[bm].nrang;rng++)
                ptr->bm[bm].sct[rng]=0;
        }

    }

}



/**
 *
 **/
void parse_ebeam(char *str) {

    int i,j=0;

    /* Loop over number of characters in str */
    for (i=0;str[i] !=0;i++) {

        if (str[i]==',') {
            str[i]=0;
            ebm[ebmno]=atoi(str+j);
            ebmno++;
            j=i+1;
        }

    }

    ebm[ebmno]=atoi(str+j);

    ebmno++;

}


int rst_opterr(char *txt) {
    fprintf(stderr,"Option not recognized: %s\n",txt);
    fprintf(stderr,"Please try: make_grid --help\n");
    return(-1);
}

/**
 * Creates a grd or grdmap format file from a fit, fitacf, or cfit format file.
 **/
struct OptionData opt;

int main(int argc,char *argv[]) {

    int old=0;

    int farg=0;
    int fnum=0;

    unsigned char help=0;
    unsigned char option=0;
    unsigned char version=0;

    unsigned char vb=0;

    char *chnstr=NULL;
    char *chnstr_fix=NULL;
    char *bmstr=NULL;

    char *stmestr=NULL;
    char *etmestr=NULL;
    char *sdtestr=NULL;
    char *edtestr=NULL;
    char *exstr=NULL;

    /* Default lower limits for velocity, power, spectral width, and velocity error */
    double min[4]={35,3,10,0};

    /* Default upper limits for velocity, power, spectral width, and velocity error */
    double max[4]={2500,60,1000,200};

    /* Default maximum allowable frequency variation [Hz] */
    int fmax=500*1000;

    int mode=0;
    int tlen=0;

    double stime=-1;
    double tmp_stime=stime;
    double etime=-1;
    double extime=0;

    double sdate=-1;
    double edate=-1;

    char *envstr;
    FILE *fp;

    int bxcar=0;
    int limit=0;
    int bflg=0;
    int isort=0;

    unsigned char gsflg=0,ionflg=0,bthflg=0;
    unsigned char nsflg=0;
    int channel=0;
    int channel_fix=-1;

    int syncflg=1;

    int chisham=0;
    int old_aacgm=0;

    int s=0,i;
    int state=0;
    FILE *fitfp=NULL;
    struct OldFitFp *oldfitfp=NULL;
    struct CFitfp *cfitfp=NULL;

    int yr,mo,dy,hr,mt;
    double sc;

    int num,index,nbox;
    int chk;

    char vstr[256];
    char *vbuf=NULL;

    /* Default altitude at which mapping is done [km] */
    double alt=300.0;

    /* Default time interval to store in each grid record [s] */
    int avlen=120;

    int iflg=0;
    unsigned char xtd=0;

    unsigned char cfitflg=0;
    unsigned char fitflg=0;

    int found_record=0;

    /* Initialize radar parameter and fit/cfit structures */
    prm=RadarParmMake();
    fit=FitMake();
    cfit=CFitMake();

    /* Initialize RadarScan structures */
    for (i=0;i<3;i++) src[i]=RadarScanMake();
    dst=RadarScanMake();

    /* Initialize GridTable structure */
    grid=GridTableMake();

    /* Make sure the SD_RADAR environment variable is set */
    envstr=getenv("SD_RADAR");
    if (envstr==NULL) {
        fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
        exit(-1);
    }

    /* Open the radar information file */
    fp=fopen(envstr,"r");
    if (fp==NULL) {
        fprintf(stderr,"Could not locate radar information file.\n");
        exit(-1);
    }

    /* Load the radar network information */
    network=RadarLoad(fp);
    fclose(fp);
    if (network==NULL) {
        fprintf(stderr,"Failed to read radar information.\n");
        exit(-1);
    }

    /* Make sure the SD_HDWPATH environment variable is set */
    envstr=getenv("SD_HDWPATH");
    if (envstr==NULL) {
        fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
        exit(-1);
    }

    /* Load the hardware information for the radar network */
    RadarLoadHardware(envstr,network);

    /* Set up command line options */
    OptionAdd(&opt,"-help",'x',&help);     /* Print the help message and exit */
    OptionAdd(&opt,"-option",'x',&option); /* Print all command line options */
    OptionAdd(&opt,"-version",'x',&version);

    OptionAdd(&opt,"old",'x',&old);     /* Input file is in fit format */

    OptionAdd(&opt,"vb",'x',&vb);       /* Log information to console */

    OptionAdd(&opt,"st",'t',&stmestr);  /* Start time in HH:MM format */
    OptionAdd(&opt,"et",'t',&etmestr);  /* End time in HH:MM format */
    OptionAdd(&opt,"sd",'t',&sdtestr);  /* Start date in YYYYMMDD format */
    OptionAdd(&opt,"ed",'t',&edtestr);  /* End date in YYYYMMDD format */
    OptionAdd(&opt,"ex",'t',&exstr);    /* Use interval with extent HH:MM */

    OptionAdd(&opt,"tl",'i',&tlen); /* Ignore scan flag, use scan length of tl seconds */
    OptionAdd(&opt,"i",'i',&avlen); /* Time interval to store in each grid record in seconds */

    OptionAdd(&opt,"cn",'t',&chnstr);           /* Process data from stereo channel a or b */
    OptionAdd(&opt,"cn_fix",'t',&chnstr_fix);   /* User-defined channel number for output only */
    OptionAdd(&opt,"ebm",'t',&bmstr);           /* Comma separated list of beams to exclude */
    OptionAdd(&opt,"minrng",'i',&minrng);       /* Exclude data from gates lower than minrng */
    OptionAdd(&opt,"maxrng",'i',&maxrng);       /* Exclude data from gates higher than maxrng */
    OptionAdd(&opt,"minsrng",'d',&minsrng);     /* Exclude data from slant ranges lower than minsrng */
    OptionAdd(&opt,"maxsrng",'d',&maxsrng);     /* Exclude data from slant ranges higher than maxsrng */

    OptionAdd(&opt,"fwgt",'i',&mode);   /* Filter weighting mode */

    OptionAdd(&opt,"pmax",'d',&max[1]);  /* Exclude data with power greater than pmax */
    OptionAdd(&opt,"vmax",'d',&max[0]);  /* Exclude data with velocity greater than vmax */
    OptionAdd(&opt,"wmax",'d',&max[2]);  /* Exclude data with width greater than wmax */
    OptionAdd(&opt,"vemax",'d',&max[3]); /* Exclude data with verror greater than vemax */

    OptionAdd(&opt,"pmin",'d',&min[1]);  /* Exclude data with power less than pmin */
    OptionAdd(&opt,"vmin",'d',&min[0]);  /* Exclude data with velocity less than vmin */
    OptionAdd(&opt,"wmin",'d',&min[2]);  /* Exclude data with width less than wmin */
    OptionAdd(&opt,"vemin",'d',&min[3]); /* Exclude data with verror less than vemin */

    OptionAdd(&opt,"alt",'d',&alt);     /* Altitude at which mapping is done [km] */

    OptionAdd(&opt,"fmax",'i',&fmax);   /* Maximum allowed frequency variation [Hz] */

    OptionAdd(&opt,"nav",'x',&bxcar); /* Do not perform boxcar median filtering */
    OptionAdd(&opt,"nlm",'x',&limit); /* Do not exclude data because it exceeds limits */
    OptionAdd(&opt,"nb",'x',&bflg);   /* Do not exclude data based on operating parameters */
    OptionAdd(&opt,"ns",'x',&nsflg);  /* Apply scan flag limit (ie exclude data with scan flag = -1) */
    OptionAdd(&opt,"xtd",'x',&xtd);   /* Write extended output that includes power and width */
    OptionAdd(&opt,"isort",'x',&isort); /* If median filtering, sort parameters independent of velocity */

    OptionAdd(&opt,"ion",'x',&ionflg);  /* Exclude data marked as ground scatter */
    OptionAdd(&opt,"gs",'x',&gsflg);    /* Exclude data marked as iono scatter */
    OptionAdd(&opt,"both",'x',&bthflg); /* Do not exclude data based on scatter flag */

    OptionAdd(&opt,"inertial",'x',&iflg); /* Create grid file in inertial reference frame */

    OptionAdd(&opt,"chisham",'x',&chisham);     /* Map data using Chisham virtual height model */
    OptionAdd(&opt,"old_aacgm",'x',&old_aacgm); /* Map data using old AACGM coefficients rather than v2 */

    OptionAdd(&opt,"fit",'x',&fitflg);   /* Input file is in the fit format */
    OptionAdd(&opt,"cfit",'x',&cfitflg); /* Input file is in the cfit format */
    /* Process command line options */
    farg=OptionProcess(1,argc,argv,&opt,rst_opterr);

    /* If command line option not recognized then print error and exit */
    if (farg==-1) {
        exit(-1);
    }

    /* If 'help' set then print help message */
    if (help==1) {
        OptionPrintInfo(stdout,hlpstr);
        exit(0);
    }

    /* If 'option' set then print all command line options */
    if (option==1) {
        OptionDump(stdout,&opt);
        exit(0);
    }

    if (version==1) {
        OptionVersion(stdout);
        exit(0);
    }

    if (farg==argc) {
        OptionPrintInfo(stderr,errstr);
        exit(-1);
    }

    /* If 'cn' set then determine Stereo channel, either A or B */
    if (chnstr != NULL) channel = set_stereo_channel(chnstr);
    
    /* If 'cn_fix' set then determine appropriate channel for output file */
    if (chnstr_fix != NULL) channel = set_fix_channel(chnstr_fix);

    if (bmstr !=NULL)  parse_ebeam(bmstr);

    if (exstr   != NULL) extime = TimeStrToSOD(exstr);
    if (stmestr != NULL) stime  = TimeStrToSOD(stmestr);
    if (etmestr != NULL) etime  = TimeStrToSOD(etmestr);
    if (sdtestr != NULL) sdate  = TimeStrToEpoch(sdtestr);
    if (edtestr != NULL) edate  = TimeStrToEpoch(edtestr);

    if (mode>0) mode--;

    /* Inverse the variables -KTS 20150127  */
    bxcar=!bxcar;
    bflg=!bflg;
    limit=!limit;

    /* Set GridTable ground scatter flag according to command line options */
    grid->gsct=1;
    if (gsflg) grid->gsct=0;
    if (ionflg) grid->gsct=1;
    if (bthflg) grid->gsct=2;

    if (vb) vbuf=vstr;

    /* Set GridTable channel number according to command line options */
    if (channel_fix !=-1) grid->chn=channel_fix;
    else if (channel !=-1) grid->chn=channel;
    else grid->chn=0;

    /* Store the velocity, power, width, and velocity error bounding threshold
     * values in GridTable only if they are applied by FilterBound */ 
    if (bflg) {
        for (i=0;i<4;i++) {
            grid->min[i]=min[i];
            grid->max[i]=max[i];
        }
    }

    /* If median filtering is going to be applied the initialize nbox so that 3
     * consecutive scans will go into each iteration of boxcar median filter,
     * otherwise set nbox to 1 for operation on only a single scan */
    if (bxcar) nbox=3;
    else nbox=1;

    /* If median filtering is going to be applied then this is the pointer to
     * the RadarScan structure which will contain the output of the filter */
    out=dst;

    /* If not explicity working from cfit files then treat input as fit or fitacf */
    if (cfitflg==0) fitflg=1;


    index=0;
    num=0;

    /* Loop over number of input files */
    for (fnum=farg;fnum<argc;fnum++) {

        fprintf(stderr,"Opening file:%s\n",argv[fnum]);

        if (fitflg) {
            /* Input file is in fit or fitacf format */
            // Read old fit file format 
            if (old) {
                /* Open the fit file for reading */
                oldfitfp=OldFitOpen(argv[fnum],NULL);

                /* Verify that the fit file was properly opened */
                if (oldfitfp==NULL) {
                    fprintf(stderr,"Error: %s\n", strerror(errno));
                    continue;
                }

                /* Read first available radar scan in fit file (will use scan
                 * flag if tlen not provided) */
                s=OldFitReadRadarScan(oldfitfp,&state,src[index],prm,fit,
                                    tlen,syncflg,channel);

                /* Close fit file if scan not properly read */
                if (s !=0) {
                    OldFitClose(oldfitfp);
                    continue;
                }
            } else {
                /* Open the fitacf file for reading */
                fitfp=fopen(argv[fnum],"r");

                /* Verify that the fitacf file was properly opened */
                if (fitfp==NULL) {
                    fprintf(stderr,"Error: %s\n", strerror(errno));
                    continue;
                }
                /* Read first available radar scan in fitacf file (will use scan
                 * flag if tlen not provided) */
                s=FitFreadRadarScan(fitfp,&state,src[index],prm,fit,
                                    tlen,syncflg,channel);
                
                /* Close fitacf file if scan not properly read */
                if (s !=0) {
                    fclose(fitfp);
                    continue;
                }
            }
        } else {
            /* Input file is in cfit format */

            /* Open the cfit file for reading */
            cfitfp=CFitOpen(argv[fnum]);

            /* Verify that the cfit file was properly opened */
            if (cfitfp==NULL) {
                fprintf(stderr,"File not found.\n");
                continue;
            }

            /* Read first available radar scan in cfit file (will use scan
             * flag if tlen not provided) */
            s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                                syncflg,channel);

            /* Close cfit file if scan not properly read */
            if (s !=0) {
                CFitClose(cfitfp);
                continue;
            }
        }
        /* If either start time or date provided as input then determine it */
        if ( found_record == 0 ) {
            if ((stime !=-1) || (sdate !=-1)) {
                /* we must skip the start of the files */
                int yr,mo,dy,hr,mt;
                double sc;
                tmp_stime=stime;
                /* If start time not provided then use time of first record
                 * in fit file */
                if (stime==-1) stime= ( (int) src[0]->st_time % (24*3600));

                /* If start date not provided then use date of first record
                 * in fit file, otherwise use provided sdate */
                if (sdate==-1) stime+=src[0]->st_time -
                                        ( (int) src[0]->st_time % (24*3600));
                else stime+=sdate;

                /* If median filter is going to be applied then we need to load data
                 * prior to the usuals start time, so stime needs to be adjusted */
                if (bxcar==1) {
                    /* subtract one src */
                    if (tlen !=0) stime-=tlen;
                    else stime-=15+src[0]->ed_time-src[0]->st_time; /* pad to make sure */
                }

                /* Calculate year, month, day, hour, minute, and second of 
                 * grid start time (needed to load AACGM_v2 coefficients) */
                TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
                /* Search for index of corresponding record in input file given
                 * grid start time */
                if (fitflg) {
                    /* Input file is in fit or fitacf format */

                    if (old) 
                        s=OldFitSeek(oldfitfp,yr,mo,dy,hr,mt,sc,NULL);
                    else 
                        s=FitFseek(fitfp,yr,mo,dy,hr,mt,sc,NULL,inx);
                    /* If a matching record could not be found then exit*/ 
                    if (s ==-1) {
                        fprintf(stderr,"File ends before requested start time, ignoring this file.\n");
                        stime = tmp_stime;
                        continue;
                    }
                    found_record = 1;
                    /* If using scan flag instead of tlen then continue to read
                     * fit records until reaching the beginning of a new scan */
                    if (tlen==0) {
                        if (old) {
                            while ((s=OldFitRead(oldfitfp,prm,fit)) !=-1) {
                                if (abs(prm->scan)==1) break;
                            }
                        } else {
                            while ((s=FitFread(fitfp,prm,fit)) !=-1) {
                                if (abs(prm->scan)==1) break;
                            }
                        }
                    } else state=0;

                    /* Read the first full scan of data from open fit or fitacf file
                     * corresponding to grid start date and time */
                    if (old) s=OldFitReadRadarScan(oldfitfp,&state,src[0],prm,fit,
                                        tlen,syncflg,channel);
                    else s=FitFreadRadarScan(fitfp,&state,src[0],prm,fit,
                                        tlen,syncflg,channel);

                } else {
                    /* Input file is in cfit format */

                    s=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL,NULL);

                    /* If a matching record could not be found then exit */
                    if (s ==-1) {
                        fprintf(stderr,"File does not contain the requested interval, ignoring this file.\n");
                        stime = tmp_stime;
                        continue;
                    }
                    found_record = 1;
                    /* If using scan flag instead of tlen then continue to read
                     * cfit records until reaching the beginning of a new scan */
                    if (tlen==0) {
                        while ((s=CFitRead(cfitfp,cfit)) !=-1) {
                            if (cfit->scan==1) break;
                        }
                    } else state=0;

                    /* Read the first full scan of data from open cfit file
                     * corresponding to grid start date and time */
                    s=CFitReadRadarScan(cfitfp,&state,src[0],cfit,tlen,syncflg,channel);

                }
                
            } 
            else {
                stime=src[0]->st_time;   // If start date and time not provided
                found_record = 1;        // then use time of first record in
                                         // input file
            }
        }
        /* If end time provided then determine end date */
        if (found_record == 1){
            if (etime !=-1){

            /* If end date not provided then use date of first record
             * in input file */
            if (edate==-1) etime+=src[0]->st_time -
                            ( (int) src[0]->st_time % (24*3600));
            else etime+=edate;
            }
            
            /* If time extent provided then use that to calculate end time */
            if (extime !=0) etime=stime+extime;

            /* If end time is set and median filtering is to be applied then need
             * to load data after the usual end time, so etime must be adjusted */
            if ((etime !=-1) && (bxcar==1)) {
                if (tlen !=0) etime+=tlen;
                else etime+=15+src[0]->ed_time-src[0]->st_time;
            }

            found_record = 2;
        }


        /* Calculate year, month, day, hour, minute, and second of 
         * grid start time (needed to load AACGM_v2 coefficients) */
        TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);

        /* Load AACGM coefficients */
        if (old_aacgm) AACGMInit(yr);
        else AACGM_v2_SetDateTime(yr,mo,dy,0,0,0);

        /* This value tracks the number of radar scans which have been
         * loaded for gridding */
        num++;

        /* Continue gridding until input scan data is beyond end of gridding time
         * or end of input file is reached */
        do {

            /* Exclude scatter in beams listed in ebm */
            RadarScanResetBeam(src[index],ebmno,ebm);

            /* If 'ns' option set then exclude data where scan flag = -1 */
            if (nsflg) exclude_outofscan(src[index]);

            /* Exclude scatter in range gates below minrng or beyond maxrng,
             * or below minsrng or beyond maxsrng */
            exclude_range(src[index],minrng,maxrng,minsrng,maxsrng);

            /* Exclude either ground or ionospheric scatter based on gsct flag */
            FilterBoundType(src[index],grid->gsct);

            /* Exclude scatter outside velocity, power, spectral width, and
             * velocity error boundaries */
            if (bflg) FilterBound(15,src[index],min,max);

            /* If enough radar scans have been loaded and the 'nlm' (no limit)
             * option has not been set, then check to make sure there has not
             * been a change in distance to first range, range separation, or
             * transmit frequency greater than fmax between the center and
             * adjacent scans in the boxcar median filter */
            if ((num>=nbox) && (limit==1) && (mode !=-1))
                    chk=FilterCheckOps(nbox,src,fmax);
            else chk=0;

            /* If the operations check succeeded and enough scans have been
             * loaded then proceed with the filtering and gridding */
            if ((chk==0) && (num>=nbox)) {

                /* Apply the boxcar median filter to the radar scans included
                 * in the src array */
                if (mode !=-1) FilterRadarScan(mode,nbox,index,src,dst,15,isort);
                else out=src[index];

                /* Calculate year, month, day, hour, minute and second of
                 * output scan start time */
                TimeEpochToYMDHMS(out->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

                /* Load the appropriate radar hardware information for the day
                 * and time of the radar scan (only done once) */
                if (site==NULL) {
                    radar=RadarGetRadar(network,out->stid);
                    if (radar==NULL) {
                        fprintf(stderr,"Failed to get radar information.\n");
                        exit(-1);
                    }
                    site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
                }

                /* Test whether gridded data should be written to a file; if so
                 * returns weighted average velocity, power, and width values
                 * for each grid cell */
                s=GridTableTest(grid,out);

                /* If no errors were returned by GridTableTest and the beginning
                 * of the grid record is equal to or after stime then write
                 * grid data to output file */
                if ((s==1) && (grid->st_time>=stime)) {

                    /* Write grid record to either grd or grdmap format file */
                    if (old) OldGridTableFwrite(stdout,grid,vbuf,xtd);
                    else GridTableFwrite(stdout,grid,vbuf,xtd);

                    if (vbuf !=NULL) fprintf(stderr,"Storing:%s\n",vbuf);

                }

                /* Map radar scan data in structure pointed to by 'out' to an
                 * equi-area grid in magnetic coordinates, storing the output
                 * in the grid GridTable structure */
                s=GridTableMap(grid,out,site,avlen,iflg,alt,chisham,old_aacgm);

                /* Return an error if mapping failed */
                if (s !=0) {
                    fprintf(stderr,"Error mapping beams.\n");
                    break;
                }

            }

            /* If median filtering, increment index of the 3-element RadarScan
             * structure */
            if (bxcar) index++;

            /* If median filtering, reset src (RadarScan) structure index if it
             * exceeds 2 */
            if (index>2) index=0;

            /* Read next radar scan from input file */
            if (fitflg) {
                if (old) s=OldFitReadRadarScan(oldfitfp,&state,src[index],
                                                prm,fit,tlen,syncflg,channel);
                else s=FitFreadRadarScan(fitfp,&state,src[index],
                                                prm,fit,tlen,syncflg,channel);
            } else
                s=CFitReadRadarScan(cfitfp,&state,src[index],cfit,tlen,
                                    syncflg,channel);

            /* If scan data is beyond end of gridding time then break out of loop */
            if ((etime !=-1) && (src[index]->st_time>etime)) break;

            /* Update the number of scans read in this big while loop */
            num++;

        } while (s!=-1);
        /* If scan data is beyond end of gridding time then break out of loop */
        if ((etime !=-1) && (src[index]->st_time>etime)) break;

        /* Close the input file */
        if (fitflg) {
            if (old) OldFitClose(oldfitfp);
            else fclose(fitfp);
        } else CFitClose(cfitfp);


    }

    return 0;

}
