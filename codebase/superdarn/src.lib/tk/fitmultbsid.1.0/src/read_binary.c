/* read_binary.c
   =============
   Author Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "fitmultbsid.h"

int FitMultBSIDHeaderDecode(struct DataMap *ptr, struct FitMultBSID *mult_scan)
{
  int i, status;
  struct DataMapScalar *sptr;

  for(status = 0, i = 0; i < ptr->snum; i++)
    {
      sptr = ptr->scl[i];

      if((strcmp(sptr->name, "fitmultbsid.version.major") == 0)
	 && (sptr->type == DATAINT))
	mult_scan->version.major = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "fitmultbsid.version.minor") == 0)
	 && (sptr->type == DATAINT))
	mult_scan->version.minor = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "stid") == 0) && (sptr->type == DATASHORT))
	mult_scan->stid = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "num_scans") == 0) && (sptr->type == DATAINT))
	mult_scan->num_scans = *(sptr->data.iptr);
      else
	{
	  fprintf(stderr, "unexpected line reached, not header data\n");
	  status = -1;
	}
    }

  return(status);
}

int FitBSIDScanDecode(struct DataMap *ptr, struct FitBSIDScan *scan)
{
  int i, status, snum, ibm;
  struct DataMapScalar *sptr;

  /* Decode the number of beams in this scan */
  for(status = 0, i = 0; i < ptr->snum; i++)
    {
      sptr = ptr->scl[i];

      if((strcmp(sptr->name, "num_bms") == 0) && (sptr->type == DATAINT))
	scan->num_bms = *(sptr->data.iptr);
      else
	{
	  fprintf(stderr, "unexpected line reached, not scan header data\n");
	  status = -1;
	}
    }

  /* Initialize the beam data */
  scan->bm = (struct FitBSIDBeam *)malloc(sizeof(struct FitBSIDBeam)
					  * scan->num_bms);
  memset(scan->bm, 0, sizeof(struct FitBSIDBeam));

  /* Decode the beam data for this scan */
  for(ibm = 0; ibm < scan->num_bms && status >= 0; ibm++)
    status = FitBSIDBeamDecode(ptr, &scan->bm[ibm]);

  scan->st_time = scan->bm[0].time;
  scan->ed_time = scan->bm[scan->num_bms-1].time;

  return(status);
}

int FitBSIDBeamDecode(struct DataMap *ptr, struct FitBSIDBeam *bm)
{
  int i, j, yr=-1, mo=-1, dy=-1, hr=-1, mt=-1, *slist=NULL, *fov=NULL;

  char **vstr=NULL;

  struct DataMapScalar *sptr;
  struct DataMapArray *aptr;

  /* Decode the number of beams in this scan */
  for(i = 0; i < ptr->snum; i++)
    {
      sptr = ptr->scl[i];

      if((strcmp(sptr->name, "cpid") == 0) && (sptr->type == DATAINT))
	bm->cpid = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "bm") == 0) && (sptr->type == DATASHORT))
	bm->bm = *(sptr->data.sptr);
      else if((strcmp(sptr->name, "bmazm") == 0) && (sptr->type == DATAFLOAT))
	bm->bmazm = *(sptr->data.fptr);
      else if((strcmp(sptr->name, "yr") == 0) && (sptr->type == DATAINT))
	yr = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "mo") == 0) && (sptr->type == DATAINT))
	mo = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "dy") == 0) && (sptr->type == DATAINT))
	dy = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "hr") == 0) && (sptr->type == DATAINT))
	hr = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "mt") == 0) && (sptr->type == DATAINT))
	mt = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "sc") == 0) && (sptr->type == DATAINT))
	bm->intt.sc = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "us") == 0) && (sptr->type == DATAINT))
	bm->intt.us = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "nave") == 0) && (sptr->type == DATAINT))
	bm->nave = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "frang") == 0) && (sptr->type == DATAINT))
	bm->frang = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "rsep") == 0) && (sptr->type == DATAINT))
	bm->rsep = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "rxrise") == 0) && (sptr->type == DATAINT))
	bm->rxrise = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "freq") == 0) && (sptr->type == DATAINT))
	bm->freq = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "noise") == 0) && (sptr->type == DATAINT))
	bm->noise = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "atten") == 0) && (sptr->type == DATAINT))
	bm->atten = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "channel") == 0) && (sptr->type == DATAINT))
	bm->channel = *(sptr->data.iptr);
      else if((strcmp(sptr->name, "nrang") == 0) && (sptr->type == DATAINT))
	bm->nrang = *(sptr->data.iptr);
      else
	{
	  fprintf(stderr, "unexpected line reached, not beam data\n");
	  return(-1);
	}
    }

  /* Set the time */
  if(yr > -1 && mo > -1 && dy > -1 && hr > -1 && mt > -1)
    bm->time = TimeYMDHMSToEpoch(yr, mo, dy, hr, mt, 0.0);
  else
    {
      fprintf(stderr, "unable to read all time variables\n");
      return(-1);
    }

  /* Initialize the range gate data */
  bm->sct       = (unsigned char *)calloc(bm->nrang, sizeof(unsigned char));
  bm->rng       = (struct RadarCell *)calloc(bm->nrang,
					     sizeof(struct RadarCell));
  bm->med_rng   = (struct RadarCell *)calloc(bm->nrang,
					     sizeof(struct RadarCell));
  bm->rng_flgs  = (struct CellBSIDFlgs *)calloc(bm->nrang,
						sizeof(struct CellBSIDFlgs));
  bm->front_loc = (struct CellBSIDLoc *)calloc(bm->nrang,
					       sizeof(struct CellBSIDLoc));
  bm->back_loc  = (struct CellBSIDLoc *)calloc(bm->nrang,
					       sizeof(struct CellBSIDLoc));
  bm->front_elv = (struct FitElv *)calloc(bm->nrang, sizeof(struct FitElv));
  bm->back_elv  = (struct FitElv *)calloc(bm->nrang, sizeof(struct FitElv));

  /* Decode the range gate data for this scan */
  for(i = 0; i < ptr->anum && slist == NULL && fov == NULL; i++)
    {
      aptr = ptr->arr[i];

      if((strcmp(aptr->name, "slist") == 0) && (aptr->type == DATASHORT)
	 && (aptr->dim == 1))
	{
	  slist = malloc(sizeof(int) * aptr->rng[0]);
	  if(slist == NULL) break;
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      slist[j] = aptr->data.sptr[j];
	      bm->sct[slist[j]] = 1;
	    }
	}
      else if((strcmp(aptr->name, "fov") == 0) && (aptr->type == DATASHORT)
	 && (aptr->dim == 1))
	{
	  fov = malloc(sizeof(int) * aptr->rng[0]);
	  if(fov == NULL) break;
	  for(j = 0; j < aptr->rng[0]; j++) fov[j] = aptr->data.sptr[j];
	}
    }

  if(slist == NULL || fov == NULL) return(-1);

  /* Decode the range gate dependant data for this scan */
  for(i = 0; i < ptr->anum; i++)
    {
      aptr = ptr->arr[i];
  
      if((strcmp(aptr->name, "gsct") == 0) && (aptr->type == DATASHORT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].gsct = aptr->data.sptr[j];
	}
      else if((strcmp(aptr->name, "fov") == 0) && (aptr->type == DATASHORT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng_flgs[slist[j]].fov = aptr->data.sptr[j];
	}
      else if((strcmp(aptr->name, "fov_past") == 0) && (aptr->type == DATASHORT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng_flgs[slist[j]].fov_past = aptr->data.sptr[j];
	}
      else if((strcmp(aptr->name, "pwr0") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].p_0 = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "pwr0_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].p_0_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "p_l") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].p_l = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "p_l_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].p_l_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "w_l") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].w_l = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "w_l_e") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].w_l_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "v") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].v = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "v_e") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].v_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "phi0") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].phi0 = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "phi0_e") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng[slist[j]].phi0_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "elv") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      bm->rng[slist[j]].elv = aptr->data.fptr[j];

	      if(fov[j] == 1) bm->front_elv[j].normal = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_elv[j].normal = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "elv_low") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1) bm->front_elv[j].low = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_elv[j].low = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "elv_high") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1) bm->front_elv[j].high = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_elv[j].high = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "vh") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1) bm->front_loc[j].vh = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_loc[j].vh = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "vh_e") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1) bm->front_loc[j].vh_e = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_loc[j].vh_e = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "hop") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1) bm->front_loc[j].hop = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_loc[j].hop = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "dist") == 0) && (aptr->type == DATAFLOAT)
	      && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1) bm->front_loc[j].dist = aptr->data.fptr[j];
	      else if(fov[j] == -1) bm->back_loc[j].dist = aptr->data.fptr[j];
	    }
	}
      else if((strcmp(aptr->name, "vh_m") == 0) && (aptr->type == DATASTRING)
	      && (aptr->dim == 1))
	{
	  vstr = ((char **) aptr->data.vptr);

	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1)
		memcpy(bm->front_loc[j].vh_m, vstr[j], 2);
	      else if(fov[j] == -1)
		memcpy(bm->back_loc[j].vh_m, vstr[j], 2);
	    }
	}
      else if((strcmp(aptr->name, "region") == 0) && (aptr->type == DATASTRING)
	      && (aptr->dim == 1))
	{
	  vstr = ((char **) aptr->data.vptr);

	  for(j = 0; j < aptr->rng[0]; j++)
	    {
	      if(fov[j] == 1)
		memcpy(bm->front_loc[j].region, vstr[j], 2);
	      else if(fov[j] == -1)
		memcpy(bm->back_loc[j].region, vstr[j], 2);
	    }
	}
      else if((strcmp(aptr->name, "med_gsct") == 0) && (aptr->type == DATASHORT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].gsct = aptr->data.sptr[j];
	}
      else if((strcmp(aptr->name, "med_pwr0") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].p_0 = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_pwr0_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].p_0_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_p_l") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].p_l = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_p_l_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].p_l_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_w_l") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].w_l = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_w_l_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].w_l_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_v") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].v = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_v_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].v_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_phi0") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].phi0 = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_phi0_e") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].phi0_e = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "med_elv") == 0) && (aptr->type == DATAFLOAT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->med_rng[slist[j]].elv = aptr->data.fptr[j];
	}
      else if((strcmp(aptr->name, "grpflg") == 0) && (aptr->type == DATASHORT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng_flgs[slist[j]].grpflg = aptr->data.sptr[j];
	}
      else if((strcmp(aptr->name, "grpnum") == 0) && (aptr->type == DATAINT)
	 && (aptr->dim == 1))
	{
	  for(j = 0; j < aptr->rng[0]; j++)
	    bm->rng_flgs[slist[j]].grpnum = aptr->data.iptr[j];
	}
      else if((strcmp(aptr->name, "grpid") == 0) && (aptr->type == DATASTRING)
	 && (aptr->dim == 1))
	{
	  vstr = ((char **) aptr->data.vptr);
	  for(j = 0; j < aptr->rng[0]; j++)
	    memcpy(bm->rng_flgs[slist[j]].grpid, vstr[j], 2);
	}
    }

  /* Free the pointers */
  free(slist);
  free(fov);

  return(0);
}

int ReadFitMultBSIDBin(FILE *fp, struct FitMultBSID *mult_scan)
{
  int fid, status, iscan;

  struct FitBSIDScan *scan, *prev;
  struct DataMap *ptr = NULL;

  /* Get the file ID and set (or return) the status */
  if((fid = fileno(fp)) == -1) return(-1);

  status = 0;

  /* Read in the binary data mapping structure */
  if((ptr = DataMapRead(fid)) == NULL) return(-1);

  /* Initialize the output structure */
  if(mult_scan == NULL) mult_scan = FitMultBSIDMake();

  /* Decode the header */
  status = FitMultBSIDHeaderDecode(ptr, mult_scan);

  if(status != 0)
    {
      DataMapFree(ptr);
      return(status);
    }

  /* Cycle through all the scans, decoding the binary data */
  scan = (struct FitBSIDScan *)malloc(sizeof(struct FitBSIDScan));
  mult_scan->scan_ptr = scan;
  prev = (struct FitBSIDScan *)(NULL);

  for(iscan = 0; iscan < mult_scan->num_scans && status >= 0; iscan++)
    {
      status = FitBSIDScanDecode(ptr, scan);

      scan->next_scan = (struct FitBSIDScan *)
	malloc(sizeof(struct FitBSIDScan));
      prev            = scan;
      scan            = scan->next_scan;
      scan->prev_scan = prev;
      prev->next_scan = scan;
    }

  /* Close out the linked structure */
  scan                = (struct FitBSIDScan *)(NULL);
  prev->next_scan     = scan;
  mult_scan->last_ptr = prev;

  /* Check the number of scans and set the start and end time */
  if(iscan != mult_scan->num_scans) status = -1;
  else
    {
      mult_scan->st_time = mult_scan->scan_ptr->st_time;
      mult_scan->ed_time = prev->ed_time;
    }

  return(status);
}
