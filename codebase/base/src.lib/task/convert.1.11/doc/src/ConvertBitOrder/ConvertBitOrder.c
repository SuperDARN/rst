/* ConvertBitOrder.c
   ================= 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {

  
  if (ConvertBitOrder() !=0) fprintf(stdout,"Little Endian machine.\n");
  else fprintf(stdout,"Big Endian machine.\n");

  return 0;
}
   

