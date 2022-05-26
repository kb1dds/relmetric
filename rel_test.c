/* Testing and computing the relations metric(s) 
 *
 * Copyright (c) 2022, Michael Robinson
 */

#include <stdio.h>
#include <malloc.h>
#include "relmetric.h"

#define DEBUG 0

int main(){
  relation r1, r2;

  r1.row_count=32;
  r2.row_count=32;
  r1.column_count=2;
  r2.column_count=3;
  r1.bitfield=malloc(sizeof(unsigned)*2);
  r2.bitfield=malloc(sizeof(unsigned)*3);
 
  r1.bitfield[0]=0x3000000f;
  r2.bitfield[0]=0x100f0f3f;
  r1.bitfield[1]=0x00000000;
  r2.bitfield[1]=0x0000000f;
  r2.bitfield[2]=0x00000010;
  
  printf("metric = %d\n",rel_metric(&r1,&r2));
  
  free(r1.bitfield);
  free(r2.bitfield);
}
