/* Testing and computing the relations metric(s) 
 *
 * Copyright (c) 2022, Michael Robinson
 */

/* A relation is represented via an array of columns, 
 * each of which is a packed bitfield.  
 * Columns end on word boundaries in the bitfield.
 */
typedef struct{
  int column_count;
  int row_count;
  unsigned *bitfield;
}relation;

/* Compute the distance between two relations */
int rel_metric( relation *r1, relation *r2 );

/* Print out a relation */
int print_relation( relation *r1 );
