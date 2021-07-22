
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int i, num=10, vals[]={1, 0, 2, 2, 0, 0, 0, 0, 0, 2}, *ismax, nmax;

  int int_argrelmax(int num, int vals[], int order, int clip, int *ismax);
  
  ismax = (int *)calloc(num, sizeof(int));
  nmax  = int_argrelmax(num, vals, 2, 1, ismax);

  for(i=0; i<num; i++)
    printf("TEST ARG REL MAX: %d/%d %d %d\n", i, num, vals[i], ismax[i]);
  
  return(1);
}
