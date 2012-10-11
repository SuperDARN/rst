/* siteglobal.h
   ============ 
   Author: R.J.Barnes
*/
   

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.1-beta-18-gf704e97.txt for more information.
 
 
 
*/

#ifndef _SITEGLOBAL_H
#define _SITEGLOBAL_H

extern int sock;
extern char server[256];
extern int port;
extern int num_transmitters;
extern struct timeval tock;
extern struct ControlPRM rprm;
extern struct RosData rdata;
extern struct DataPRM dprm;
extern struct TRTimes badtrdat;
extern struct TXStatus txstatus;
extern struct SiteLibrary sitelib;
extern int *exit_flag;
extern int cancel_count;


#endif
