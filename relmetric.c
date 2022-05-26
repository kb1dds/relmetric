/* Computing the relation metric 
 *
 * Copyright (c) 2022, Michael Robinson
 */

#include <stdio.h>
#include <malloc.h>
#include "relmetric.h"

#define DEBUG 0

/* Count the number of differences between two columns 
 * Input: c1, c2 = bitfields to compare
 *        row_count = number of rows in the columns 
 * Returns: difference count
 * Caution: does unpacking and pointer arithmetic 
 */
int column_diff(unsigned *c1, unsigned *c2, int row_count){
  int rows_per_unsigned, i, j, diff;
  unsigned *c1p, *c2p, current_diff;

  /* Establish pointers */
  c1p=c1;
  c2p=c2;
  diff = 0;

  /* Unpack whole words first */
  rows_per_unsigned = 8*sizeof(unsigned);
  for( i = 0; i < row_count/rows_per_unsigned; i++ ){
    current_diff = (*c1p ^ *c2p);

    for( j = 0; j < rows_per_unsigned; j ++ ){
      diff += current_diff & 0x01;
      current_diff >>= 1;
    }

    c1p++;
    c2p++;
  }

  /* Collect remaining rows */
  current_diff = (*c1p ^ *c2p);
  for( j = 0; j < row_count % rows_per_unsigned; j ++ ){
    diff += current_diff & 0x01;
    current_diff >>= 1;
  }
  return(diff);
}

/* Compare two specified columns between relations.
 * Match col1 in relation r1 with col2 in relation r2
 *
 * Input: r1,r2 = relations to compare
 *        col1, col2 = column indices to match
 * Returns: nonnegative : number of differences between columns
 *          -1 : col1 index is out of range
 *          -2 : col2 index is out of range
 *          -3 : relations have different number of rows
 */
int match_columns( relation *r1, relation *r2, int col1, int col2 ){
  int rows_per_unsigned, column_length;
  
  /* Bounds checking */
  if( r1->row_count != r2->row_count ) return(-3);
  if( col1 >= r1->column_count ) return(-1);
  if( col2 >= r2->column_count ) return(-2);

  /* Unpack column indices into bitfields */
  rows_per_unsigned=8*sizeof(unsigned);
  column_length = (r1->row_count/rows_per_unsigned);
  if(r1->row_count % rows_per_unsigned){
    column_length ++;
  }

#if DEBUG
  printf("(%x =?= %x) ",r1->bitfield[col1*column_length], r2->bitfield[col2*column_length]);
#endif

  /* Finally, compute the difference */
  return(column_diff(&r1->bitfield[col1*column_length],
		     &r2->bitfield[col2*column_length],
		     r1->row_count));
}

/* Compute the weight of a column function col_matches r1->r2 */
int weight( relation *r1, relation *r2, int *col_matches ){
  int i, *col_used, use_count, diff;

  /* Allocate scratchpad */ 
  col_used=malloc(sizeof(int)*r2->column_count);
  if( col_used == NULL )
    return -1;

  /* Clear image of the match in r2 */
  for( i = 0; i < r2->column_count; i ++ ){
    col_used[i]=0;
  }

  /* Apply current match */
  diff = 0;
  for( i = 0; i < r1->column_count; i ++ ){
    diff += match_columns(r1,r2,i,col_matches[i]);
#if DEBUG
    printf("%d->%d:%d ", i, col_matches[i], match_columns(r1,r2,i,col_matches[i]));
#endif
    col_used[col_matches[i]] = 1;
  }
  
  /* Count how many columns are in the image */
  for( i = 0, use_count = 0; i < r2->column_count; i ++ ){
    use_count += col_used[i];
  }

#if DEBUG
  printf("Diff=%d ", diff);
#endif
  
  /* Apply penalty for unmatched columns */
  diff += (r2->column_count-use_count)*r2->row_count;
#if DEBUG
  printf("Penalty=%d ",(r2->column_count-use_count)*r2->row_count);
#endif
  
  free(col_used);
  return(diff);
}

/* Determine the minimum weight r1->r2 */
int min_weight( relation *r1, relation *r2 ){
  int *col_matches, i, wt, min_wt;

  /* Allocate scratchpad */
  col_matches=malloc(sizeof(int)*r1->column_count);
  if( col_matches == NULL )
    return -1;

  /* Initialize column matches */
  for( i = 0; i < r1->column_count; i ++ ){
    col_matches[i] = 0;
  }

  /* Initialize worst case */
  min_wt = r2->column_count*r2->row_count;

  /* Lots of looping! */
  i = 0;
  while( i != r1->column_count ){
#if DEBUG
    for( i = 0; i < r1->column_count; i ++ ){
      printf("%d ", col_matches[i]);
    }
#endif
    
    /* Use this match to compute weight */
    wt = weight( r1, r2, col_matches );

    if( wt < min_wt ){
      min_wt = wt;
    }
#if DEBUG
    printf(" Weight=%d\n",wt);
#endif
    
    /* Proceed to next match */
    for( i = 0; i < r1->column_count; i ++ ){
      col_matches[i] ++;
      if( col_matches[i] < r2->column_count ){
	/* No further incrementing needed */
	break;
      }
      /* Actually, we need to carry to the next step! */
      col_matches[i] = 0;
    }
  }

  free(col_matches);

  return(min_wt);
}

/* Compute the distance between two relations */
int rel_metric( relation *r1, relation *r2 ){
  int w1, w2;

  w1 = min_weight(r1,r2);
  w2 = min_weight(r2,r1);

  if( w1 > w2 )
    return(w1);
  return(w2);
}

/* Print out a relation */
int print_relation( relation *r1 ){
  int i, j, k;
  int rows_per_unsigned, column_length;
  unsigned col_field;

  /* Storage length of each column */
  rows_per_unsigned = 8*sizeof(unsigned);
  column_length = (r1->row_count/rows_per_unsigned);
  if(r1->row_count % rows_per_unsigned){
    column_length ++;
  }  

  for( i = 0; i < r1->row_count; i ++ ){
    for( j = 0; j < r1->column_count; j ++){
      col_field = r1->bitfield[column_length*j+i/rows_per_unsigned];
      for( k = 0; k < i; k ++ ){
	col_field >>= 1;
      }
      printf("%c",(col_field & 0x01)?'1':'0');
    }
    printf("\n");
  }
}
