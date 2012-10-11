/* shell.c
   =======
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <ncurses.h>
#include <sys/types.h>


#include "rtypes.h"

#include "radarshell.h"

#define TRUE 1
#define FALSE 0

#define DELIM  " \n"

#define PROMPT ">"
#define MAX_INPUT_LINE 256
  	
int shell(int num,size_t size,size_t *offset,char *buffer) {
 /* check for user interruptions */
   int status;
   int input_fd;
   int wait=TRUE;
   
   char input_line[MAX_INPUT_LINE];
   char parse_line[MAX_INPUT_LINE];
   char *variable=NULL;
   char *command=NULL;
   char *value=NULL;
   int n,c;  

   struct RShellBuffer buf;
   struct RShellTable rstable;
   struct RShellEntry *ptr;
       
   input_fd = fileno(stdin);
   
   tcflush(input_fd,TCIFLUSH);

   rstable.num=0;
   rstable.ptr=NULL;

   buf.num=num;
   buf.len=size;
   buf.off=offset;
   buf.buf=buffer;

   n=0; 
   while(n<buf.num) {
     RadarShellAdd(&rstable,RShellBufferRead(&buf,n),
		   *((int *) RShellBufferRead(&buf,n+1)),
		   RShellBufferRead(&buf,n+2));
     n+=3;
   }

   while (wait) {
     fprintf(stdout,PROMPT);
     input_line[0] = 0;

     /* get a command line */

     fgets(input_line,MAX_INPUT_LINE,stdin); 
	 strcpy(parse_line,input_line); 
     command=strtok(parse_line,DELIM);
	 
	 if (command !=NULL) {
	   if (strcmp(command,"go")==0) wait=0; /* return to control program */
	   else if (strcmp(command,"show")==0) { /* list variables */
		 variable=strtok(NULL,DELIM);
		 if (variable !=NULL) { /* list specific variable */
           while (variable !=NULL) {
	     ptr=RadarShellFind(&rstable,variable);
             if (ptr==NULL) continue;
             fprintf(stdout,"%s=",ptr->name);
             switch (ptr->type) {
             case var_SHORT:
	       fprintf(stdout,"%d\n",*((short int *) ptr->data)); 
	        break;
             case var_INT:
               fprintf(stdout,"%d\n",*((int *) ptr->data));
               break;              
	     case var_LONG:
               fprintf(stdout,"%d\n",*((int *) ptr->data));
	       break;
             case var_FLOAT:
               fprintf(stdout,"%f\n",*((float *) ptr->data));
               break;
             case var_DOUBLE:
               fprintf(stdout,"%f\n",*((double *) ptr->data));
               break;
             case var_STRING:
               fprintf(stdout,"%s\n",(char *) ptr->data);
               break;
             default:
               break;
	     }
             variable=strtok(NULL,DELIM);
	   }		   
	 } else { /* list all */
		   for (c=0;c<rstable.num;c++) {

  		   ptr=RadarShellRead(&rstable,c);
		   fprintf(stdout,"%s=",ptr->name);
		   switch (ptr->type) {
		   case var_SHORT:
		     fprintf(stdout,"%d\n",*((short int *) ptr->data));
		     break;
		   case var_INT:
		     fprintf(stdout,"%d\n",*((int *) ptr->data));
		     break;
		   case var_LONG:
		     fprintf(stdout,"%d\n",*((int *) ptr->data));
		     break;
		   case var_FLOAT:
		     fprintf(stdout,"%f\n",*((float *) ptr->data));
		     break;
		   case var_DOUBLE:
		     fprintf(stdout,"%f\n",*((double *) ptr->data));
                     break;
		   case var_STRING:
		     fprintf(stdout,"%s\n",(char *) ptr->data);
		     break;
                   default:
		     break;

		   }   
                 if ((c % 20)==0) { /* wait for a key press */
                   int x;
                   x=getch();
                 }

	      }
	   }  
       } else if (strchr(input_line,'=') !=NULL) { /* assign variables */      
         int i;
         strcpy(parse_line,input_line); 
         status=((variable=strtok(parse_line," =\n")) !=NULL);
         status=(status) ? ((value=strtok(NULL,"\n")) !=NULL) : FALSE;
         /* remove trailing spaces from the variable name */
         for (i=0;(variable[i] != ' ') && (variable[i] != 0);i++);
         variable[i]=0;
         for (i=0;(value[i] ==' ') && (value[i] !=0);i++);
         if (i>0) value+=i;

         /* strip the leading spaces of the value argument */

         if (status) { /* do the assignment */
	   ptr=RadarShellFind(&rstable,variable);
	   if (ptr==NULL) continue;
           switch (ptr->type) {
	   case var_SHORT:
	     *((short int *) ptr->data)=atoi(value);
             break;
	   case var_INT:
             *((int *) ptr->data)=atol(value);
             break;
           case var_LONG:
	     *((int *) ptr->data)=atol(value);
             break;
	   case var_FLOAT:
	      *((float *) ptr->data)=atof(value);
              break;
	   case var_DOUBLE:
	      *((double *) ptr->data)=atof(value);;
              break;	
	   case var_STRING:
	     strncpy( (char *) (ptr->data),value,STR_MAX);		       
	      break;  		 
	    default :
              break;
	   }
	 }
	   }
	 }
   }

   RadarShellFree(&rstable);		
   return 0;
} 		  

 


 
