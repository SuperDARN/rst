/* maketsg.c
   ==========
   Author: R.J.Barnes
   
Copyright (c) 2009 The Johns Hopkins University/Applied Physics Laboratory

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

#include "tsg.h"




#define S_bit 0x01
#define A_bit 0x08
#define R_bit 0x02
#define X_bit 0x04
#define P_bit 0x10

#define Null_code 0x00
#define S_code S_bit
#define A_code A_bit
#define AS_code (A_code | S_bit)
#define AR_code (A_bit | R_bit)
#define ARS_code (AR_code | S_bit)
#define ARX_code (AR_code | X_bit)
#define ARXS_code (ARX_code | S_bit)

#define TEN_MICROSEC (10/CLOCK_PERIOD)
#define END_DELAY (10000/CLOCK_PERIOD)	/* time from last pulse to end of tm seq */
#define R_to_X_min 60/CLOCK_PERIOD	/* 60 microsec minimum spacing between AR and ARX */
#define A_to_R_min 10/CLOCK_PERIOD/* 10 microsec minimum spacing between A and AR */

#define MIN2(x,y)  (((x) <(y)) ? (x) : (y))

#define MIN3(a,b,c)  (((a) < MIN2( (b), (c))) ? (a) : (MIN2( (b), (c))))

/*	The method of creating a timing sequence is to set up a "state" machine
	The following table defines the allowed states. */

enum TSGstates {
	Null,	/* all bits 0 */
	S_state,	/* sample only */
	A1,		/* turn on attenuators before pulse */
	AS1,	/* turn on attenuators + take sample */
	AR1,	/* turn off receiver before pulse */
	ARS1,	/* turn off receiver + take sample */
	ARX,	/* transmit */
	ARXS,	/* transmit and sample */
	AR2,	/* turn off transmit */
	ARS2,	/* turn off transmit + take sample */
	A2,		/* turn on receiver */
	AS2,	/* turn on receiver + take sample */
	END_state	/* exit */
};

/*      For each state, we define the following quantities: 1) the control code
	that is sent to the digital I/O for the state, 2) the maximum duration
	the state can last, 3) the 3 possible transitions from the present
	state to the next.  The 3 possibilities correspond to the conditions:
	time_to_next_sample_state_change < time_to_next_pulse_state_change
	Note: the duration of a state may be less than the maximum, since a
	particular pulse state may be interrupted by a sample.  */

struct TSGsdef {
	 int rfvect;		/* code bits asserted when in this state */
	 int duration;	        /* maximum duration of this state */
	 int next_state[3];	/* possible transitions */
} states[13] = 
        /* Null */   {{Null_code, 0, {S_state,  AS1,     A1}},
	/* S_st */   {S_code,    0,  {Null,     A1,      AS1}},
	/* A1   */   {A_code,	 0,  {AS1,      ARS1,    AR1}},
	/* AS1  */   {AS_code,	 0,  {A1,       AR1,     ARS1}},
	/* AR1  */   {AR_code,	 0,  {ARS1,     ARXS,    ARX}},
	/* ARS1 */   {ARS_code,	 0,  {AR1,      ARX,     ARXS}},
	/* ARX  */   {ARX_code,	 0,  {ARXS,     ARS2,    AR2}},
	/* ARXS */   {ARXS_code, 0,  {ARX,      AR2,     ARS2}},
	/* AR2	*/   {AR_code,	 0,  {ARS2,     AS2,     A2}},
	/* ARS2 */   {ARS_code,	 0,  {AR2,      A2,      AS2}},
	/* A2   */   {A_code,	 0,  {AS2,      S_state, Null}},
	/* AS2  */   {AS_code,	 0,  {A2,       Null,    S_state}},
        /* end  */   {Null_code, 1,  {Null,     Null,    Null}}};


struct TSGcnt {
  int last;
  int n_smp;
};

void TSGWrBuf(struct TSGbuf *tsg,
	       int code,int delay,struct TSGcnt *cnt) {
  while (delay > 255) {	
	tsg->code[tsg->len] = (char) code;
	tsg->rep[tsg->len] = (char) 255;
	tsg->len++;
	delay = delay - 255;
  }
  tsg->code[tsg->len] = (char) code;
  tsg->rep[tsg->len] = (char) delay;
  tsg->len++;  
  if ((code & S_bit) && (cnt->last==0)) cnt->n_smp++;
  cnt->last=code & S_bit;
  return;
}

void TSGFree(struct TSGbuf *ptr) {
  if (ptr->code !=NULL) free(ptr->code);
  if (ptr->rep !=NULL) free(ptr->rep);
  free(ptr);
}

struct TSGbuf *TSGMake(struct TSGprm *tsg,int *flg) {

  enum TSGstates state;
  int temp;
  int t2,t3,t4,t5,ttemp,smsp,lgfr,mpnc,txp,p_time,s_time,f_time=0;
  int phase_delay=0;
  int i, j,buf_len;
  int  N_sample;
  int delay, code, p_code;
  struct TSGcnt cnt;
  struct TSGbuf *buf=NULL;
  /*  int c,smdelay; */

  int scope=0x80;
  int noscope=0;

 
  *flg=TSG_OK;

  if ((tsg->frang < 0) || (tsg->rsep <= 0)) {  
    *flg=TSG_INV_RSEP;
	return NULL;
  }

  if ((tsg->txpl <= 0) && (tsg->smsep <= 0) && (tsg->rsep <= 0)) {
    *flg=TSG_NO_SMSEP; 
    return NULL; 
  }

  if ((tsg->mppul == 0) && (tsg->smsep == 0) && (tsg->rsep == 0)) {
    *flg=TSG_INV_MPPUL_SMSEP; 
	return NULL;
  }

  if (tsg->mppul > 0) {
     for (i=1;(i<tsg->mppul) && (tsg->pat[i]>tsg->pat[i-1]);i++);
     if (i<tsg->mppul) {
       *flg=TSG_INV_PAT;
       return NULL;
    }
  }

 if ((buf=malloc(sizeof(struct TSGbuf)))==NULL) return NULL;  
  cnt.last=0;
  cnt.n_smp=0;

  if (tsg->nbaud < 1) tsg ->nbaud = 1;
  
  /* calculate lag to first range and sample separation */
	
  temp = tsg->frang*20;	/* in order to keep integer arithmetic */
  tsg->lagfr = temp/3;   /* working right we have to force 2 steps */

  if (tsg->mppul !=0) {
    if (tsg->txpl !=0) tsg->smsep=tsg->txpl; 
    else if (tsg->smsep==0) {
	  tsg->smsep = (tsg->rsep*20)/3;
	  tsg->txpl = tsg->smsep;
    } else tsg->txpl=tsg->smsep;
  } 

  /* OK, if we are transmitting a phase encoded pulse, 
	 the real sample separation has to be the pulse length/nbaud */
		
  tsg->smsep = tsg->smsep/tsg->nbaud;
  tsg->mlag = (tsg->mppul == 0) ? 0 : tsg->pat[tsg->mppul - 1];
		
  /* make sure the sample separation is an even 
     multiple of the clock period */

  if (tsg->smsep % CLOCK_PERIOD != 0) {
    tsg->smsep = (tsg->smsep/CLOCK_PERIOD +1)*CLOCK_PERIOD;
    if (((tsg->smsep/CLOCK_PERIOD) % 2) != 0) 
      tsg->smsep = tsg->smsep + CLOCK_PERIOD;
  }	

  if (tsg->mppul>0) {
  	if ((tsg->mpinc) < (tsg->smsep)) {
      free(buf);
      *flg=TSG_INV_MPINC_SMSEP; 
      return NULL;
  	}
  	if (tsg->lagfr % tsg->smsep != 0) {
      free(buf);
      *flg=TSG_INV_LAGFR_SMSEP; 
	  return NULL;
	}
  	if (tsg->mpinc % tsg->smsep != 0) {
      free(buf);
      *flg=TSG_INV_MPINC_SMSEP;   
	  return NULL;
  	}
  	if ( ((20L*(long)(tsg->mppul)*(tsg->txpl))/
         ( (long)(tsg->mlag)*(tsg->mpinc) +
		 (tsg->nrang)*(tsg->nbaud)*(tsg->smsep) + 
                 tsg->lagfr) ) >= 1L) {
      free(buf);
      *flg=TSG_INV_DUTY_CYCLE;  
	  return NULL;
    }
  } 

  /*    NOTE:  the pattern shown below does NOT include 
   *    the effect of phase encoding 
   */

  /*
                             <-----smsep ------>
  S____|____________________|___________________|________
        <t1> _______________________________ <t6>
  A_________|                               |____________
             <t2> _____________________ <t5>
  R______________|     <--txpl--->     |________________
                  <t3> ___________ <t4>
  X___________________|           |____________________

  */
  smsp = tsg->smsep/CLOCK_PERIOD;
  lgfr = tsg->lagfr/CLOCK_PERIOD;
  mpnc = tsg->mpinc/CLOCK_PERIOD;
  txp  = tsg->txpl/CLOCK_PERIOD;

  if ((smsp % 2) != 0) {
    free(buf);
    *flg=TSG_INV_ODD_SMSEP;
	return NULL;
  }

  /* the next line calculates the number of samples that are to be
     taken.  The range separation (in microseconds) may include 
     multiple samples if we are using phase coding, but the length
     of the transmitted pulse does not have to be the same as the
     range separation */

  N_sample = tsg->mlag * mpnc/smsp + 
             tsg->nrang*((tsg->rsep)*20)/(3*tsg->smsep);
  tsg->samples = N_sample;

  t3 = R_to_X_min;
  t2 = A_to_R_min;
  if (tsg->rtoxmin==0) t4 = t3;
  else t4=tsg->rtoxmin/CLOCK_PERIOD;
  t5 = t2+30/CLOCK_PERIOD;

  /*
  if (t5 > TEN_MICROSEC) {       
     steal 10 microsec from RA2 to A2 transition to give 
       to AXR2 to AR2 transition (if possible) 
    t4 = t4 + TEN_MICROSEC;
    t5 = t5 - TEN_MICROSEC;
  }
  */

  /* OK, we have now computed the various time delays 
   	 put the time delays into the state definition table */

  states[A1].duration = t2;
  states[AS1].duration = t2;
  states[AR1].duration = t3;
  states[ARS1].duration = t3;
  states[ARX].duration = txp;
  states[ARXS].duration = txp;
  states[AR2].duration = t4;
  states[ARS2].duration = t4;
  states[A2].duration = t5;
  states[AS2].duration = t5;
  smsp = smsp/2;		  /* sample event time is 1/2 the 
  				      	   sample separation */
  i = 0;
  j = 0;

  if (tsg->mppul == 0) {
    p_time = 32767;		/* no pulses - receive only */
    s_time = 1;
  } else {
    p_time = (tsg->pat)[i++]*mpnc + 1;  /* initialize time to next pulse 
                                           event */
    s_time = p_time + lgfr + t2 + t3 +txp/2; /* init time to next sample 
                                                event */
    f_time = 32727;  /* init. time to next baud */

    /* if we are using phase encoding, calculate 
       the time length of each baud */

    if (tsg->nbaud > 1) {
      phase_delay = txp/(tsg->nbaud);
      if (txp % (tsg->nbaud) != 0) {
        free(buf);
        *flg=TSG_INV_TXPL_BAUD; 
	    return NULL;
      }
      if (phase_delay==0) {
        free(buf);
	    *flg=TSG_INV_PHASE_DELAY; 
	    return NULL;
      }
    }
  }
 
  state = Null;	/* initial state is Null */
  buf->len = 0;

  buf_len = (tsg->mppul)*(6 + 2*tsg->nbaud) + 
            lgfr + 2*N_sample + 100; /*allocate extra space */

  if ((buf->code = malloc(2*buf_len)) == NULL) {
    free(buf);
    *flg=TSG_INV_MEMORY;
    return NULL;
  } 
     
  if ((buf->rep  = malloc(4*buf_len)) == NULL) {
    free(buf->code);
    free(buf);
    *flg=TSG_INV_MEMORY;
    return NULL;
  }


/*  code=states[state].rfvect | scope; 
  delay = MIN3(f_time,s_time,p_time); 
  delay=tsg->stdelay-delay;
  if (delay>0) TSGWrBuf(buf,code,delay,&cnt);*/
  code=scope;
  TSGWrBuf(buf,code,1,&cnt);
  delay=tsg->stdelay;

  code=states[state].rfvect;
  TSGWrBuf(buf,code, delay,&cnt);
  scope=noscope;
  /* now enter the main loop */
  while (cnt.n_smp < N_sample) {
    code = states[state].rfvect | scope;

    /* fudge to trigger the GC214TS card after the first tx */

    if ((tsg->gort==1) && (code & X_bit)) scope=noscope;

    /* check to see if we are transmitting. If we are, and if
       we are phase coding the pulse, then add the appropriate
       phase code bit to the output code. */

    if ((code & X_bit) && (tsg->nbaud > 1)) {
      if (f_time > txp) f_time = phase_delay;
      p_code = *((tsg->code) + (tsg->nbaud)*(i-1) + j);
      p_code = (p_code < 0) ? P_bit : 0;
      code = code | p_code;
    }
    else f_time = 32767;
    delay = MIN3(f_time,s_time,p_time); 
    /* delay =minimum time to next event */

    /* write the code and the delay out to the buffer */
  
    TSGWrBuf(buf,code, delay,&cnt);

    /* now determine the next state */
 
   if (f_time < s_time && f_time < p_time) {
      /* the next event is just a phase change */
      ++j;    /* increment the baud index */
    } else if (s_time < p_time) {
      state = states[state].next_state[0];
      if (f_time == s_time) ++j; /* increment phase index */
    } else if (s_time == p_time) {
      state = states[state].next_state[1];
      j = 0;
    } else {
      f_time = 32767;
      j=0;    /* re-initialize baud index */
      state = states[state].next_state[2];
    }

    /* calculate times to next events */
    s_time = s_time - delay;
    p_time = p_time - delay;
    f_time = f_time - delay;

    if (f_time <= 0) f_time = phase_delay;
    if (s_time <= 0) s_time = smsp;
    if (p_time <= 0) p_time = states[state].duration;

    if (p_time == 0) {		/* calculate time to next pulse */
      if (i < tsg->mppul) {
	    p_time = ((tsg->pat)[i] - (tsg->pat)[i-1])*mpnc
					- (t2+t3+t4+t5+txp);
  	    j = 0;
  	    i++;
      } else p_time = 32767;
    }
  }	
  /* Enter the final Null + delay of End_Delay from last pulse
  into the timing sequence.  NOTE:  if no pulses (i.e. a CLR FREQ sequence)
  then do not insert any extra delay */

  /* ttemp = END_DELAY - (32767 - p_time);  this is wrong */
 
  ttemp = END_DELAY;

  if ( (tsg->mppul == 0) || (ttemp <= 0)) ttemp = 1;
  TSGWrBuf(buf,Null_code, ttemp,&cnt);

  buf->code[buf->len-1]=buf->code[buf->len-1] | noscope;
  /*
  c=0;
  smdelay=0;
  while ((c<buf->len) && ((buf->code[c] & 0x01)==0)) {
    smdelay=smdelay+buf->rep[c];
    c++;
  }
  smdelay=smdelay*10;
  tsg->smdelay=smdelay/(tsg->rsep/3.0e-1*2)+0.5;
  */

  tsg->smdelay=tsg->lagfr/tsg->smsep; /* may break Wallops */
  return buf;
}
