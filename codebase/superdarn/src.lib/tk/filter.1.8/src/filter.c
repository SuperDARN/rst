/* filter.c
   ========
   Author: R.J.Barnes
   Comments: E.G.Thomas (2016)
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
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "scandata.h"
#include "filter.h"

int FilterCmpVel(const void *x,const void *y) {
  struct RadarCell *a,*b;
  a=*((struct RadarCell **) x);
  b=*((struct RadarCell **) y);

  if (a->v<b->v) return -1;
  if (a->v>b->v) return 1;
  return 0;
}

int FilterCmpPwr(const void *x,const void *y) {
  struct RadarCell *a,*b;
  a=*((struct RadarCell **) x);
  b=*((struct RadarCell **) y);
  if (a->p_l<b->p_l) return -1;
  if (a->p_l>b->p_l) return 1;
  return 0;
}

int FilterCmpWdt(const void *x,const void *y) {
  struct RadarCell *a,*b;
  a=*((struct RadarCell **) x);
  b=*((struct RadarCell **) y);
  if (a->w_l<b->w_l) return -1;
  if (a->w_l>b->w_l) return 1;
  return 0;
}



/**
 * Performs median filtering on a sequence of radar scan data structures. The
 * filter operates on each range-beam cell, as a 3x3x3 grid of the neighboring
 * cells in both time and space is extracted from the input data and a weighted
 * sum of the number of cells containing scatter is calculated. If this sum is
 * below a certain threshold then the output cell is consiered empty and the
 * filter continues to the next range-beam cell. If this sum exceeds the
 * threshold then the median value of each parameter (velocity, power, spectral
 * width) from the 3x3x3 input grid is assigned to the center cell of the output
 * scan data. The errors assocaited with the parameters are calculated from the
 * standard deviation of the input parameters. Returns zero if successful.
 **/
int FilterRadarScan(int mode, int depth, int inx, struct RadarScan **src,
                    struct RadarScan *dst, int prm) {

    int thresh[2] = {12,24};
    double us;
    int bmin,bmax,rmin,rmax,bbox,rbox;
    int i,w,n,x,y,z,c,f,bm,rng,mxbm=1;
    struct RadarBeam *b;
    struct RadarBeam **tmp;
    struct RadarBeam **bmptr[MAX_BEAM][FILTER_DEPTH];
    int bmcnt[MAX_BEAM][FILTER_DEPTH];

    int weight[FILTER_WIDTH][FILTER_HEIGHT][FILTER_DEPTH];
    struct RadarCell **cell=NULL;
    struct RadarCell **median=NULL;
    int cnum=0,cnt=0;
    int maxbeam=0;
    int maxrange=1000;
    double variance,mean,sigma;  

    /* If input filter depth greater than FILTER_DEPTH then set to 
     * FILTER_DEPTH */
    if (depth>FILTER_DEPTH) depth=FILTER_DEPTH;

    /* Loop over temporal dimension (depth) of median filter */
    for (i=0;i<depth;i++) {

        /* Loop over number of beams in each RadarScan structure */
        for (n=0;n<src[i]->num;n++) {

            /* Get current beam number from RadarBeam structure */
            bm=src[i]->bm[n].bm;

            /* Update largest beam number if necessary */
            if (bm>maxbeam) maxbeam=bm;

            /* Get number of range gates from RadarBeam structure */
            rng=src[i]->bm[n].nrang;

            /* Update largest number of range gates if necessary */
            if (rng<maxrange) maxrange=rng;

        }

    }

    /* Add one to get largest total number of beams in RadarScan structure */
    maxbeam++;

    /* Loop over temporal dimension of median filter */
    for (z=0;z<depth;z++) {

        /* Loop over range dimension of median filter */
        for (y=0;y<FILTER_HEIGHT;y++) {

            /* Loop over beam dimension of median filter */
            for (x=0;x<FILTER_WIDTH;x++) {

                /* Calculate weight of each cell in 3x3x3 filtering grid */
                f=(x !=0) & (x !=FILTER_WIDTH-1) & 
                  (y !=0) & (y !=FILTER_HEIGHT-1);
                w=1+f;
                f=(z !=0) & (z !=FILTER_DEPTH-1);
                weight[x][y][z]=w*(1+f);

            }

        }

    }

    /* Reset any information contained in dst RadarScan structure */
    RadarScanReset(dst);

    /* Loop over number of beams found from src RadarScan structures */
    for (bm=0;bm<maxbeam;bm++) {

        /* Add a new beam to the dst RadarScan structure */
        b=RadarScanAddBeam(dst,maxrange);

        /* Initialize the beam number of each beam to -1 */
        b->bm=-1;
        for (z=0;z<depth;z++)  {
            bmptr[bm][z]=NULL;
            bmcnt[bm][z]=0;
        }

    }

    /* Loop over temporal dimension of median filter */
    for (z=0;z<depth;z++) {

        /* Figure out if scan corresponds to previous, center, or next scan in filter */
        i=(inx-(depth-1)+z);
        if (i<0) i+=depth;

        /* Loop over number of beams in RadarScan structure */
        for (n=0;n<src[i]->num;n++) {

            /* Get current beam number from RadarBeam structure */
            bm=src[i]->bm[n].bm;

            /* Get index of current beam in bmptr structure */
            c=bmcnt[bm][z];
            if (bmptr[bm][z]==NULL) bmptr[bm][z]=malloc(sizeof(struct RadarBeam **));
            else {
                tmp=realloc(bmptr[bm][z],sizeof(struct RadarBeam **)*(c+1));
                if (tmp==NULL) break;
                bmptr[bm][z]=tmp;
            }

            /* Store pointer to current beam in bmptr structure */
            bmptr[bm][z][c]=&src[i]->bm[n];

            /* Update beam count for beam and temporal dimensions */
            bmcnt[bm][z]++;

            /* Update maximum beam count for beam and temporal dimensions */
            if (bmcnt[bm][z]>mxbm) mxbm=bmcnt[bm][z];

        }

        /* Break out of loop if something went wrong */
        if (n !=src[i]->num) break;

    }

    /* Error check to make sure previous loop worked */
    if (z !=depth) {
        for (z=0;z<depth;z++) {
            for (bm=0;bm<maxbeam;bm++) if (bmptr[bm][z] !=NULL) free(bmptr[bm][z]);
        }
        return -1;
    }

    /* Get index of 'center' scan in temporal dimension */
    i=inx-1;
    if (i<0) i+=depth;

    /* Load basic parameters from 'center' temporal scan */
    dst->stid=src[i]->stid;
    dst->version.major=src[i]->version.major;
    dst->version.minor=src[i]->version.minor;
    dst->st_time=src[i]->st_time;
    dst->ed_time=src[i]->ed_time;

    /* If 4-bit is set then use operating parameters from first
     * beam found on 'center' scan for the output RadarScan structure */
    if ((mode & 4)==4) {

        /* Loop over maximum number of beams in median filter */
        for (bm=0;bm<maxbeam;bm++) {

            /* If no beams were found for 'center' scan then continue */
            if (bmcnt[bm][depth/2]==0) continue;

            /* Get pointer to first beam found for 'center' scan */
            b=bmptr[bm][depth/2][0];

            /* Load radar operating parameters in RadarBeam structure */
            dst->bm[bm].bm=bm;
            dst->bm[bm].cpid=b->cpid;
            dst->bm[bm].time=b->time;
            dst->bm[bm].intt=b->intt;
            dst->bm[bm].nave=b->nave;
            dst->bm[bm].frang=b->frang;
            dst->bm[bm].rsep=b->rsep;
            dst->bm[bm].rxrise=b->rxrise;
            dst->bm[bm].freq=b->freq;
            dst->bm[bm].noise=b->noise;
            dst->bm[bm].atten=b->atten;
            dst->bm[bm].channel=b->channel;
            dst->bm[bm].nrang=maxrange;

        }

    } else {

        /* Loop over maximum number of beams in median filter */
        for (n=0;n<maxbeam;n++) {

            /* Initialize radar operating parameters in RadarBeam structure */
            dst->bm[n].cpid=-1;
            dst->bm[n].time=0;
            dst->bm[n].intt.sc=0;
            dst->bm[n].intt.us=0;
            dst->bm[n].frang=0;
            dst->bm[n].rsep=0;
            dst->bm[n].rxrise=0;
            dst->bm[n].freq=0;
            dst->bm[n].noise=0;
            dst->bm[n].atten=0;
            dst->bm[n].channel=-1;
            dst->bm[n].nrang=-1;

        }

        /* Loop over temporal dimension of median filter */
        for (z=0;z<depth;z++) {

            /* Loop over maximum number of beams in median filter */
            for (bm=0;bm<maxbeam;bm++) {

                /* If no beams were previously found then continue */
                if (bmcnt[bm][z]==0) continue;

                /* Set current beam number in RadarBeam structure */
                dst->bm[bm].bm=bm;

                /* Loop over number of beams found in median filter time/beam cell */
                for (c=0;c<bmcnt[bm][z];c++) {

                    /* Get pointer to current beam */
                    b=bmptr[bm][z][c];

                    /* If this is the first beam in time/beam cell then use it
                     * to set CPID value for RadarBeam structure */
                    if (dst->bm[bm].cpid==-1) dst->bm[bm].cpid=b->cpid;

                    /* Sum the operating parameters for later averaging across
                     * all beams in each time/beam cell */
                    dst->bm[bm].time+=b->time;
                    dst->bm[bm].intt.sc+=b->intt.sc;
                    dst->bm[bm].intt.us+=b->intt.us;
                    if (dst->bm[bm].intt.us>1e6) {
                        dst->bm[bm].intt.sc++;
                        dst->bm[bm].intt.us-=1e6;
                    }
                    dst->bm[bm].nave+=b->nave;
                    dst->bm[bm].frang+=b->frang;
                    dst->bm[bm].rsep+=b->rsep;
                    dst->bm[bm].rxrise+=b->rxrise;
                    dst->bm[bm].freq+=b->freq;
                    dst->bm[bm].noise+=b->noise;
                    dst->bm[bm].atten+=b->atten;
                    if (dst->bm[bm].channel==0) dst->bm[bm].channel=b->channel;

                    /* If this is the first beam in time/beam cell then use maxrange
                     * to set the number of range gates for RadarBeam structure */
                    if (dst->bm[bm].nrang==-1) dst->bm[bm].nrang=maxrange;

                }

            }

        }

        /* Loop over maximum number of beams in median filter */
        for (n=0;n<maxbeam;n++) {

            cnt=0;

            /* Loop over temporal dimension and count the total number of times
             * each beam was sampled */
            for (z=0;z<depth;z++) cnt+=bmcnt[n][z];

            /* If no beams were found the continue */
            if (cnt==0) continue; 

            /* Average the radar operating parameters across each beam sounding */
            dst->bm[n].time=dst->bm[n].time/cnt;
            dst->bm[n].intt.us=dst->bm[n].intt.us/cnt;
            us=(1e6*dst->bm[n].intt.sc)/cnt;
            dst->bm[n].intt.sc=dst->bm[n].intt.sc/cnt;
            us=us-1e6*dst->bm[n].intt.sc;
            dst->bm[n].intt.us+=us;
            dst->bm[n].nave=dst->bm[n].nave/cnt;
            dst->bm[n].frang=dst->bm[n].frang/cnt;
            dst->bm[n].rsep=dst->bm[n].rsep/cnt;
            dst->bm[n].rxrise=dst->bm[n].rxrise/cnt;
            dst->bm[n].freq=dst->bm[n].freq/cnt;
            dst->bm[n].noise=dst->bm[n].noise/cnt;
            dst->bm[n].atten=dst->bm[n].atten/cnt;

        }

    }

    /* Create empty RadarCell structure to store all velocity/power/width values
     * of data in 3x3x3 filter dimension */
    cell=malloc(sizeof(struct RadarCell *)*FILTER_WIDTH*
              FILTER_HEIGHT*depth*mxbm);

    /* Create empty RadarCell structure to store velocity/power/width values
     * which meet the 2 sigma criteria for each parameter */
    median=malloc(sizeof(struct RadarCell *)*FILTER_WIDTH*
              FILTER_HEIGHT*depth*mxbm);

    /* Loop over maximum number of beams in median filter */
    for (bm=0;bm<maxbeam;bm++) {

        /* Loop over maximum number of range gates in median filter */
        for (rng=0;rng<maxrange;rng++) {

            /* Set up the spatial 3x3 (Beam-x-Width) filtering boundaries */
            cnum=0;
            bmin=bm-FILTER_WIDTH/2;
            bbox=bm-FILTER_WIDTH/2;
            bmax=bm+FILTER_WIDTH/2;
            rmin=rng-FILTER_HEIGHT/2;
            rbox=rng-FILTER_HEIGHT/2;
            rmax=rng+FILTER_HEIGHT/2;

            /* Set lower beam boundary to zero when at edge of FOV */
            if (bmin<0) bmin=0;

            /* Set upper beam boundary to highest beam when at other edge of FOV */
            if (bmax>=maxbeam) bmax=maxbeam-1;

            /* Set lower range boundary to zero when at nearest edge of FOV */
            if (rmin<0) rmin=0;

            /* Set upper range boundary to furthest range gate when at other edge of FOV */
            if (rmax>=maxrange) rmax=maxrange-1;

            /* Initialize 'center' cell weight to zero */
            w=0;

            /* Loop over beam dimension */
            for (x=bmin;x<=bmax;x++) {

                /* Loop over range dimension */
                for (y=rmin;y<=rmax;y++) {

                    /* Loop over temporal dimension */
                    for (z=0;z<depth;z++) {

                        /* Loop over number of beams found in time/beam cell */
                        for (c=0;c<bmcnt[x][z];c++) {

                            /* Get pointer to current beam */
                            b=bmptr[x][z][c];

                            /* Add weight of current cell if scatter is present */
                            w+=weight[x-bbox][y-rbox][z]*b->sct[y]; 

                            /* Make sure we haven't exceeded the number of range
                             * gates (this seems unnecessary given above check) */
                            if (y>=b->nrang) continue;        

                            /* If there is scatter in the beam/range gate cell
                             * then load it into the cell RadarCell structure and
                             * update the total number of cells in the median filter
                             * containing scatter */ 
                            if (b->sct[y] !=0) {
                                cell[cnum]=&b->rng[y];
                                cnum++;
                            }

                        }

                    }

                } 

            }

            /* If no cells were found containing scatter then continue */
            if (cnum==0) continue;

            /* If the current beam is on the edge of the FOV then increase
             * its calculated weight by 50% (accounting for fewer possible
             * cells on those beams? what about near/far range edges?) */
            if ((bm==0) || (bm==maxbeam-1)) w=w*1.5;

            /* If the sum of the weights of the cells containing scatter don't
             * exceed the threshold (12 or 24) then continue */
            if (w<=thresh[mode % 2]) continue;

            /* If above threshold was exceeded then mark current beam/gate cell
             * in output RadarScan structure as having scatter and zero out
             * groundscatter flag, lambda power, spectral width, and velocity */
            dst->bm[bm].sct[rng]=1;
            dst->bm[bm].rng[rng].gsct=0;
            dst->bm[bm].rng[rng].p_l=0;
            dst->bm[bm].rng[rng].w_l=0; 
            dst->bm[bm].rng[rng].v=0;

            /* Bitwise and test of prm option to perform velocity median filtering */
            if (prm & 0x01) {

                mean=0;
                variance=0;
                sigma=0;
                cnt=0;

                /* Calculate the mean of the velocity values */
                for (c=0;c<cnum;c++) mean+=cell[c]->v;
                mean=mean/cnum;

                /* Calculate the variance of the velocity values */
                for (c=0;c<cnum;c++) 
                    variance+=(cell[c]->v-mean)*(cell[c]->v-mean);
                variance=variance/cnum;

                /* Calculate the standard deviation of the velocity values */
                if (variance>0) sigma=sqrt(variance);

                /* Loop over number of median filter cells for beam/gate */
                for (c=0;c<cnum;c++) {

                    /* If the velocity deviation from the mean is greater
                     * than 2 standard deviations the continue */
                    if (fabs(cell[c]->v-mean)>2*sigma) continue;

                    /* If the velocity mean difference is less than 2 sigma
                     * then load the cell values into the median RadarCell
                     * structure */
                    median[cnt]=cell[c]; 

                    /* Update the total number of cells with a velocity mean 
                     * difference of less than 2 sigma */
                    cnt++;

                }

                /* Sort velocity values in median Radarcell structure
                 * from most negative to most positive velocity */
                qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);

                /* Set current beam/gate velocity to the center of the above
                 * array sorted by velocity (ie the median) */
                dst->bm[bm].rng[rng].v=median[cnt/2]->v;

                /* Reset the mean and variance to zero */
                mean=0;
                variance=0;

                /* Calculate the mean of the sorted velocities */
                for (c=0;c<cnt;c++) mean+=median[c]->v;
                mean=mean/cnt;

                /* Calculate the variance of the sorted velocities */
                for (c=0;c<cnt;c++) 
                    variance+=(median[c]->v-mean)*(median[c]->v-mean);
                variance=variance/cnt;

                /* Calculate the standard deviation of the sorted velocities */
                if (variance>0) sigma=sqrt(variance);
                else sigma=0;

                /* Set the velocity error to the calculated standard deviation */
                dst->bm[bm].rng[rng].v_e=sigma;

            }

            /* Bitwise and test of prm option to perform power median filtering */
            if (prm & 0x02) {

                mean=0;
                variance=0;
                sigma=0;
                cnt=0;

                /* Calculate the mean of the lambda power values */
                for (c=0;c<cnum;c++) mean+=cell[c]->p_l;
                mean=mean/cnum;

                /* Calculate the variance of the lambda power values */
                for (c=0;c<cnum;c++) 
                    variance+=(cell[c]->p_l-mean)*(cell[c]->p_l-mean);
                variance=variance/cnum;

                /* Calculate the standard deviation of the lambda power values */
                if (variance>0) sigma=sqrt(variance);

                /* Loop over number of median filter cells for beam/gate */
                for (c=0;c<cnum;c++) {

                    /* If the lambda power deviation from the mean is greater
                     * than 2 standard deviations then continue */
                    if (fabs(cell[c]->p_l-mean)>2*sigma) continue;

                    /* If the lambda power mean difference is less than 2 sigma
                     * then load the cell values in the median RadarCell
                     * structure */
                    median[cnt]=cell[c]; 

                    /* Update the total number of cells with a power mean
                     * difference of less than 2 sigma */
                    cnt++;

                }

                /* Sort velocity values in median RadarCell structure
                 * from most negative to most positive velocity */
                qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);

                /* Set current beam/gate power to the center of the above
                 * array sorted by velocity (ie the median) */
                dst->bm[bm].rng[rng].p_l=median[cnt/2]->p_l;

                /* Reset the mean and variance to zero */
                mean=0;
                variance=0;

                /* Calculate the mean of the sorted lambda powers */
                for (c=0;c<cnt;c++) mean+=median[c]->p_l;
                mean=mean/cnt;

                /* Calculate the variance of the sorted lambda powers */
                for (c=0;c<cnt;c++) 
                    variance+=(median[c]->p_l-mean)*(median[c]->p_l-mean);
                variance=variance/cnt;

                /* Calculate the standard deviation of the sorted powers */
                if (variance>0) sigma=sqrt(variance);
                else sigma=0;

                /* Set the power error to the calculated standard deviation */
                dst->bm[bm].rng[rng].p_l_e=sigma;

            }

            /* Bitwise and test of prm option to perform width median filtering */
            if (prm & 0x04) {

                mean=0;
                variance=0;
                sigma=0;
                cnt=0;

                /* Calculate the mean of the spectral width values */
                for (c=0;c<cnum;c++) mean+=cell[c]->w_l;
                mean=mean/cnum;

                /* Calculate the variance of the spectral width values */
                for (c=0;c<cnum;c++) 
                    variance+=(cell[c]->w_l-mean)*(cell[c]->w_l-mean);
                variance=variance/cnum;

                /* Calculate the standard deviation of the spectral width values */
                if (variance>0) sigma=sqrt(variance);

                /* Loop over number of median filter cells for beam/gate */
                for (c=0;c<cnum;c++) {

                    /* If the spectral width deviation from the mean is greater
                     * than 2 standard deviations then continue */
                    if (fabs(cell[c]->w_l-mean)>2*sigma) continue;

                    /* If the spectral width mean difference is less than 2 sigma
                     * then load the cell values in to the median RadarCell
                     * structure */
                    median[cnt]=cell[c];

                    /* Update the total number of cells with a width mean
                     * difference of less than 2 sigma */
                    cnt++;

                }

                /* Sort velocity values in median Radarcell structure
                 * from most negative to most positive */
                qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);

                /* Set current beam/gate width to the center of the above
                 * array sorted by velocity (ie the median) */
                dst->bm[bm].rng[rng].w_l=median[cnt/2]->w_l;

                /* Reset the mean and variance to zero */
                mean=0;
                variance=0;

                /* Calculate the mean of the sorted spectral widths */
                for (c=0;c<cnt;c++) mean+=median[c]->w_l;
                mean=mean/cnt;

                /* Calculate the variance of the sorted spectral widths */
                for (c=0;c<cnt;c++) 
                    variance+=(median[c]->w_l-mean)*(median[c]->w_l-mean);
                variance=variance/cnt;

                /* Calculate the standard deviation of the sorted widths */
                if (variance>0) sigma=sqrt(variance);
                else sigma=0;

                /* Set the width error to the calculated standard deviation */
                dst->bm[bm].rng[rng].w_l_e=sigma;

            }

            /* Bitwise and test of prm option to perform lag0 power median filtering */
            if (prm & 0x08) {

                mean=0;
                variance=0;
                sigma=0;
                cnt=0;

                /* Calculate the mean of the lag0 power values */
                for (c=0;c<cnum;c++) mean+=cell[c]->p_0;
                mean=mean/cnum;

                /* Calculate the variance of the lag0 power values */
                for (c=0;c<cnum;c++) 
                    variance+=(cell[c]->p_0-mean)*(cell[c]->p_0-mean);
                variance=variance/cnum;

                /* Calculate the standard deviation of the lag0 power values */
                if (variance>0) sigma=sqrt(variance);

                /* Loop over number of median filter cells for beam/gate */
                for (c=0;c<cnum;c++) {

                    /* If the lag0 power deviation from the mean is greater
                    * than 2 standard deviations then continue */
                    if (fabs(cell[c]->p_0-mean)>2*sigma) continue;

                    /* if the lag0 power mean difference is less than 2 sigma
                     * then load the cell values into the median RadarCell
                     * structure */
                    median[cnt]=cell[c];

                    /* Update the total number of cells with a lag0 power mean
                     * difference of less than 2 sigma */
                    cnt++;

                }

                /* Sort velocity values in median RadarCell structure
                 * from most negative to most positive velocity */
                qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);

                /* Set current beam/gate lag0 power to the center of the above
                 * array sorted by velocity (ie the median) */
                dst->bm[bm].rng[rng].p_0=median[cnt/2]->p_0;

                /* Reset the mean and variance to zero */
                mean=0;
                variance=0;

                /* Calculate the mean of the sorted lag0 powers */
                for (c=0;c<cnt;c++) mean+=median[c]->p_0;
                mean=mean/cnt;

                /* Calculate the variance of the sorted lag0 powers */
                for (c=0;c<cnt;c++) 
                    variance+=(median[c]->p_0-mean)*(median[c]->p_0-mean);
                variance=variance/cnt;

                /* Calculate the standard deviation of the sorted lag0 powers */
                if (variance>0) sigma=sqrt(variance);
                else sigma=0;

                /* Set the lag0 power error to the calculated standard deviation */
                dst->bm[bm].rng[rng].p_0_e=sigma;

            }

        }

    }

    /* Free memory used for cell and median RadarCell structures */
    free(median);  
    free(cell);

    /* Free memory used for bmptr RadarBeam structures */
    for (x=0;x<depth;x++) {
        for (bm=0;bm<maxbeam;bm++) if (bmptr[bm][x] !=NULL) free(bmptr[bm][x]);
    }

    /* Return zero if successful */
    return 0;

}
