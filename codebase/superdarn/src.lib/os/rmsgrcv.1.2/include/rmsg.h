/* rmsg.h
   =====
   Author: R.J.Barnes
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

/*
 $Log: rmsg.h,v $
 Revision 1.4  2008/03/15 15:39:46  code
 Added offset into the samples for IQ data to handle stereo.

 Revision 1.3  2008/03/14 21:56:40  code
 Added IQS_TYPE.

 Revision 1.2  2008/03/14 20:26:50  code
 Added the raw I&Q data type.

 Revision 1.1  2004/07/12 22:36:36  barnes
 Initial revision

*/

#ifndef _RMSG_H
#define _RMSG_H

#define PRM_TYPE 0
#define FIT_TYPE 1
#define RAW_TYPE 2
#define NME_TYPE 3

/*  type for the IQ data 
 *
 *  IQ_TYPE = IQ data block 
 *  IQS_TYPE = name of the shared memory object containing the samples.
 *  IQO_TYPE = offset into the shared memory to take samples.
 */

#define BADTR_TYPE 254

#define IQ_TYPE 255
#define IQS_TYPE 256
#define IQO_TYPE 257



#define MAX_BUF 32

#define TASK_OPEN	'O'
#define TASK_CLOSE	'C'	
#define TASK_QUIT	'Q'		
#define TASK_DATA   'd'
#define TASK_RESET  'r'

#define TASK_OK	 0	
#define TASK_ERR	'R'	
#define UNKNOWN_TYPE	0x7f

struct RMsgData {
  int type;
  int tag;
  size_t size;
  int index;
};
   
struct RMsgBlock {
  int num;
  size_t tsize;
  struct RMsgData data[MAX_BUF];
  unsigned char *ptr[MAX_BUF];
};

#endif
