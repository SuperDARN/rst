/* fitacfclientgui.c
   =================
   Author: E.G.Thomas
 
Copyright (C) 2022  Evan G. Thomas

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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include <ncurses.h>
#include <signal.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "fitcnx.h"

#include "errstr.h"
#include "hlpstr.h"



struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fitacfclientgui --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  int i,j,arg;
  int nrng=75;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int min_beam=100;
  int max_beam=-100;

  unsigned char colorflg=0;
  double nlevels=5;
  double smin=0;
  double smax=0;
  int val=0;
  int start=0;

  unsigned char pwrflg=0;
  double pmin=0;
  double pmax=40;

  unsigned char velflg=0;
  double vmin=-500;
  double vmax=500;

  unsigned char widflg=0;
  double wmin=0;
  double wmax=250;

  unsigned char elvflg=0;
  double emin=0;
  double emax=40;

  int sock;
  int remote_port=0;
  char host[256];
  int flag,status;
  struct RadarParm *prm;
  struct FitData *fit;

  int c=0;

  prm=RadarParmMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"nrange",'i',&nrng);

  OptionAdd(&opt,"color",'x',&colorflg);
  OptionAdd(&opt,"p",'x',&pwrflg);
  OptionAdd(&opt,"pmin",'d',&pmin);
  OptionAdd(&opt,"pmax",'d',&pmax);
  OptionAdd(&opt,"v",'x',&velflg);
  OptionAdd(&opt,"vmin",'d',&vmin);
  OptionAdd(&opt,"vmax",'d',&vmax);
  OptionAdd(&opt,"w",'x',&widflg);
  OptionAdd(&opt,"wmin",'d',&wmin);
  OptionAdd(&opt,"wmax",'d',&wmax);
  OptionAdd(&opt,"e",'x',&elvflg);
  OptionAdd(&opt,"emin",'d',&emin);
  OptionAdd(&opt,"emax",'d',&emax);

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

  if (argc-arg<2) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

  strcpy(host,argv[argc-2]);
  remote_port=atoi(argv[argc-1]);

  sock=ConnexOpen(host,remote_port,NULL); 

  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }

  /* Initialize new screen */
  initscr();

  signal(SIGWINCH, NULL);

  /* Make getch a non-blocking call */
  nodelay(stdscr,TRUE);

  cbreak();
  noecho();

  /* Hide the cursor */
  curs_set(0);

  /* Initialize colors */
  if (colorflg) {
    if (has_colors() == FALSE) {
      endwin();
      fprintf(stderr,"No color support!\n");
      exit(1);
    }

    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);

    init_pair(7, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(8, COLOR_BLUE, COLOR_BLUE);
    init_pair(9, COLOR_GREEN, COLOR_GREEN);
    init_pair(10, COLOR_CYAN, COLOR_CYAN);
    init_pair(11, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(12, COLOR_RED, COLOR_RED);

    if ((!pwrflg) && (!velflg) && (!widflg) && (!elvflg)) pwrflg=1;

    if (pwrflg) {
      smin=pmin;
      smax=pmax;
    } else if (velflg) {
      smin=vmin;
      smax=vmax;
    } else if (widflg) {
      smin=wmin;
      smax=wmax;
    } else if (elvflg) {
      smin=emin;
      smax=emax;
    }
  }

  do {

    /* Check for key press to exit */
    c = getch();
    if (colorflg) {
      if (c == 'p') {
        pwrflg=1;
        velflg=0;
        widflg=0;
        elvflg=0;
        smin=pmin;
        smax=pmax;
      } else if (c == 'v') {
        pwrflg=0;
        velflg=1;
        widflg=0;
        elvflg=0;
        smin=vmin;
        smax=vmax;
      } else if (c == 'w') {
        pwrflg=0;
        velflg=0;
        widflg=1;
        elvflg=0;
        smin=wmin;
        smax=wmax;
      } else if (c == 'e') {
        pwrflg=0;
        velflg=0;
        widflg=0;
        elvflg=1;
        smin=emin;
        smax=emax;
      } else if (c != ERR) break;
    } else if (c != ERR) break;

    status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);

    if (status==-1) break;

    if (flag !=-1) {
      /* Print date/time and radar operating parameters */
      move(0, 0);
      clrtoeol();
      printw("%04d-%02d-%02d %02d:%02d:%02d\n",
             prm->time.yr,prm->time.mo,prm->time.dy,
             prm->time.hr,prm->time.mt,prm->time.sc);
      clrtoeol();
      printw("stid  = %3d  cpid  = %d  channel = %d\n", prm->stid,prm->cp,prm->channel);
      clrtoeol();
      printw("bmnum = %3d  bmazm = %.2f  xcf = %d\n", prm->bmnum,prm->bmazm,prm->xcf);
      clrtoeol();
      printw("intt  = %3.1f  nave  = %3d  tfreq = %d\n",
             prm->intt.sc+prm->intt.us/1.0e6,prm->nave,prm->tfreq);
      clrtoeol();
      printw("frang = %3d  nrang = %3d\n", prm->frang,prm->nrang);
      clrtoeol();
      printw("rsep  = %3d  noise.search = %g\n", prm->rsep,prm->noise.search);
      clrtoeol();
      printw("scan  = %3d  noise.mean   = %g\n", prm->scan,prm->noise.mean);
      clrtoeol();
      printw("mppul = %3d  mpinc = %d\n", prm->mppul,prm->mpinc);
      clrtoeol();
      printw("origin.code = %d\n", prm->origin.code);

      if (prm->origin.time != NULL) {
        clrtoeol();
        printw("origin.time = %s\n",prm->origin.time);
      }
      if (prm->origin.command !=NULL) {
        clrtoeol();
        printw("origin.command = %s\n\n",prm->origin.command);
      }

      /* Draw beam and gate labels */
      move(12, 0);
      printw("B\\G 0         10        20        30        40        50        60        70\n");

      if (colorflg) {
        if (prm->bmnum < min_beam) min_beam = prm->bmnum;
        if (prm->bmnum > max_beam) max_beam = prm->bmnum;
        for (i=min_beam;i<max_beam+1; i++) {
          move(i+13, 0);
          printw("%02d:",i);
        }
      }

      /* Draw each range gate for beam */
      move(prm->bmnum+13, 0);
      clrtoeol();
      if (colorflg) attron(COLOR_PAIR(6));
      printw("%02d: ",prm->bmnum);
      if (colorflg) attroff(COLOR_PAIR(6));
      for (i=0;i<nrng; i++) {
        if (fit->rng[i].qflg == 1) {
          if (colorflg) {
            if (pwrflg)      val = (int)((fit->rng[i].p_l-smin)/(smax-smin)*nlevels)+1;
            else if (velflg) val = (int)((fit->rng[i].v-smin)/(smax-smin)*nlevels)+1;
            else if (widflg) val = (int)((fit->rng[i].w_l-smin)/(smax-smin)*nlevels)+1;
            else if (elvflg) val = (int)((fit->elv[i].normal-smin)/(smax-smin)*nlevels)+1;

            if (val < 1) val=1;
            if (val > nlevels+1) val=nlevels+1;
            attron(COLOR_PAIR(val));
          }
          
          if (fit->rng[i].gsct != 0) printw("g");
          else                       printw("i");

          if (colorflg) attroff(COLOR_PAIR(val));
        } else {
          printw("-");
        }
      }
      printw("\n");

      /* Draw a color bar */
      if (colorflg) {
        move(11, nrng+4);
        if (pwrflg)      printw("Pow [dB]");
        else if (velflg) printw("Vel [m/s]");
        else if (widflg) printw("Wid [m/s]");
        else if (elvflg) printw("Elv [deg]");
        start=12;
        for (j=12;j>6;j--) {
          attron(COLOR_PAIR(j));
          for (i=start;i<start+2;i++) {
            move(i, nrng+5);
            printw(" ");
          }
          attroff(COLOR_PAIR(j));
          move(i-1, nrng+7);
          clrtoeol();
          printw("%d",(int)((j-7)*(smax-smin)/nlevels+smin));
          start=start+2;
        }
      }

      refresh();

    }

  } while(1);

  endwin();

  return 0;
}
