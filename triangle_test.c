/* Testing and computing the relations metric(s) 
 *
 * Copyright (c) 2022, Michael Robinson
 */

#include <stdio.h>
#include <malloc.h>
#include "relmetric.h"

#define DEBUG 0
#define TEST_CASES 1
#define MAX_COLS 4
#define MAX_ROWS 4

int main(){
  relation r1, r2, r3;
  int d12, d13, d23;
  int i, fail, rows_per_unsighed;
    
  /* Preallocate relations */
  rows_per_unsigned = 8*sizeof(unsigned);
  r1.bitfield = malloc(sizeof(unsigned)*MAX_COLS*(MAX_ROWS/rows_per_unsigned+1));
  if( r1.bitfield == NULL ){
    return;
  }
  r2.bitfield = malloc(sizeof(unsigned)*MAX_COLS*(MAX_ROWS/rows_per_unsigned+1));
  if( r2.bitfield == NULL ){
    free(r1.bitfield);
    return;
  }
  r3.bitfield = malloc(sizeof(unsigned)*MAX_COLS*(MAX_ROWS/rows_per_unsigned+1));
  if( r3.bitfield == NULL ){
    free(r1.bitfield);
    free(r2.bitfield);
    return;
  }

  for(i = 0; i < TEST_CASES; i ++){
    /* Randomly size relations */
    r1.row_count = (rand()%MAX_ROWS)+1;
    r2.row_count = (rand()%MAX_ROWS)+1;
    r3.row_count = (rand()%MAX_ROWS)+1;
    r1.column_count = (rand()%MAX_COLS)+1;
    r2.column_count = (rand()%MAX_COLS)+1;
    r3.column_count = (rand()%MAX_COLS)+1;

    /* Randomly populate relations */
    for( j = 0; j < MAX_COLS*(MAX_ROWS/rows_per_unsigned+1); j ++ ){
      r1.bitfield[j] = rand();
      r2.bitfield[j] = rand();
      r3.bitfield[j] = rand();
    }

    /* Compute distances */
    d12 = rel_metric( &r1, &r2 );
    d13 = rel_metric( &r1, &r3 );
    d23 = rel_metric( &r2, &r3 );

    /* Test for triangle inequality */
#if DEBUG
    printf("Metrics: 1-2: %d, 1-3: %d, 2-3: %d\n", d12, d13, d23);
#endif
    fail = 0;
    if( d13 > (d12 + d23)){
      printf("Violation 1-3: %d > (%d + %d)\n", d13, d12, d23);
      fail = 1;
    }
    if (d12 > (d13 + d23)){
      printf("Violation 1-2: %d > (%d + %d)\n", d12, d13, d23);
      fail = 2;
    }
    if (d23 > (d12 + d13)){
      printf("Violation 2-3: %d > (%d + %d)\n", d23, d12, d13);
      fail = 3;
    }
    if( fail ){
      printf("Relation 1:\n");
      print_relation( &r1 );
      
      printf("\nRelation 2:\n");
      print_relation( &r2 );

      printf("\nRelation 3:\n");
      print_relation( &r3 );
    }
  }
  
  /* Free relations */
  free(r1.bitfield);
  free(r2.bitfield);
  free(r3.bitfield);
}
