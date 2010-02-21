/*
 *  C Implementation of Kuhn's Hungarian Method
 *  Copyright (C) 2003  Brian Gerkey <gerkey@robotics.usc.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * A C implementation of the Hungarian method for solving Optimal Assignment
 * Problems.  Theoretically runs in O(mn^2) time for an m X n problem; this 
 * implementation is certainly not as fast as it could be.
 *
 * $Id: hungarian.c,v 1.10 2003/03/14 22:07:42 gerkey Exp $
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "hungarian.h"

/*
 * the two main internal routines return this type, telling us what to do next
 */
typedef enum
{
  HUNGARIAN_ERROR,
  HUNGARIAN_ROUTINE_ONE,
  HUNGARIAN_ROUTINE_TWO,
  HUNGARIAN_DONE
} hungarian_code_t;

// the Q matrix is filled with instances of this type
typedef enum
{
  HUNGARIAN_ZERO,
  HUNGARIAN_ONE,
  HUNGARIAN_STAR
} hungarian_q_t;


// some internal routines
void hungarian_make_cover(hungarian_t* prob);
void hungarian_build_q(hungarian_t* prob);
void hungarian_add_stars(hungarian_t* prob);
hungarian_code_t hungarian_routine_one(hungarian_t* prob);
hungarian_code_t hungarian_routine_two(hungarian_t* prob);

/*
 * initialize the given object as an mXn problem.  allocates storage, which
 * should be freed with hungarian_fini()
 */
void hungarian_init(hungarian_t* prob, int* r, size_t m, size_t n, int mode)
{
  int i,j;
  // did we get a good object pointer?
  assert(prob);

  // we can't work with matrices that have more rows than columns.
  //
  // TODO: automatically transpose such matrices
  assert(m<=n);

  // init the struct
  prob->m = m;
  prob->n = n;
  assert(prob->r = (int**)calloc(m,sizeof(int*)));
  assert(prob->q = (int**)calloc(m,sizeof(int*)));
  prob->mode = mode;
  prob->maxutil = 0;
  for(i=0;i<m;i++)
  {
    assert(prob->r[i] = (int*)calloc(n,sizeof(int)));
    assert(prob->q[i] = (int*)calloc(n,sizeof(int)));
    for(j=0;j<n;j++)
    {
      prob->r[i][j] = r[i*n+j];
      if(prob->r[i][j] > prob->maxutil)
        prob->maxutil = prob->r[i][j];
    }
  }
  // if we're going to minimize, rather than maximize, we need to subtract
  // each utility from the maximum.  this operation will be reversed before
  // computing the benefit.
  if(mode == HUNGARIAN_MIN)
  {
    for(i=0;i<m;i++)
    {
      for(j=0;j<n;j++)
        prob->r[i][j] = prob->maxutil - prob->r[i][j];
    }
  }

  assert(prob->a = (int*)calloc(m,sizeof(int)));
  assert(prob->u = (int*)calloc(m,sizeof(int)));
  assert(prob->v = (int*)calloc(n,sizeof(int)));
  assert(prob->seq.i = (int*)calloc(m*n,sizeof(int)));
  assert(prob->seq.j = (int*)calloc(m*n,sizeof(int)));
  assert(prob->ess_rows = (int*)calloc(m,sizeof(int)));
  assert(prob->ess_cols = (int*)calloc(n,sizeof(int)));
}

/*
 * frees storage associated with the given problem object.  you must have
 * called hungarian_init() first.
 */
void hungarian_fini(hungarian_t* prob)
{
  int i;
  assert(prob);
  for(i=0;i<prob->m;i++)
  {
    free(prob->q[i]);
    free(prob->r[i]);
  }
  free(prob->q);
  free(prob->r);
  free(prob->a);
  free(prob->u);
  free(prob->v);
  free(prob->seq.i);
  free(prob->seq.j);
  free(prob->ess_rows);
  free(prob->ess_cols);
}

/*
 * make an initial cover
 */
void hungarian_make_cover(hungarian_t* prob)
{
  int i,j;
  int* row_max; 
  int* col_max; 

  assert(prob);
  assert(row_max = (int*)calloc(prob->m,sizeof(int)));
  assert(col_max = (int*)calloc(prob->n,sizeof(int)));

  prob->row_total = prob->col_total = 0;

  // find the max in each row (col) and sum them
  for(i=0;i<prob->m;i++)
  {
    for(j=0;j<prob->n;j++)
    {
      if(prob->r[i][j] > row_max[i])
        row_max[i] = prob->r[i][j];
    }
    prob->row_total += row_max[i];
  }

  for(j=0;j<prob->n;j++)
  {
    for(i=0;i<prob->m;i++)
    {
      if(prob->r[i][j] > col_max[j])
        col_max[j] = prob->r[i][j];
    }
    prob->col_total += col_max[j];
  }

  // make u and v into an initial cover, based on row and col totals
  if(prob->row_total <= prob->col_total)
    memcpy(prob->u,row_max,sizeof(int)*prob->m);
  else
    memcpy(prob->v,col_max,sizeof(int)*prob->n);

  free(row_max);
  free(col_max);
}

void hungarian_build_q(hungarian_t* prob)
{
  int i,j;
  assert(prob);

  for(i=0;i<prob->m;i++)
  {
    for(j=0;j<prob->n;j++)
    {
      if((prob->u[i] + prob->v[j]) == prob->r[i][j])
      {
        if(prob->q[i][j] == HUNGARIAN_ZERO)
          prob->q[i][j] = HUNGARIAN_ONE;
      }
      else
        prob->q[i][j] = HUNGARIAN_ZERO;
    }
  }
}

void hungarian_add_stars(hungarian_t* prob)
{
  int i,j,k;

  assert(prob);

  if(prob->row_total <= prob->col_total)
  {
    for(i=0;i<prob->m;i++)
    {
      for(j=0;j<prob->n;j++)
      {
        if(prob->q[i][j] == HUNGARIAN_ONE)
        {
          for(k=0;k<prob->m;k++)
          {
            if((k!=i) && (prob->q[k][j] == HUNGARIAN_STAR))
              break;
          }
          if(k==prob->m)
          {
            prob->q[i][j] = HUNGARIAN_STAR;
            break;
          }
        }
      }
    }
  }
  else
  {
    for(j=0;j<prob->n;j++)
    {
      for(i=0;i<prob->m;i++)
      {
        if(prob->q[i][j] == HUNGARIAN_ONE)
        {
          for(k=0;k<prob->n;k++)
          {
            if((k!=j) && (prob->q[i][k] == HUNGARIAN_STAR))
              break;
          }
          if(k==prob->n)
          {
            prob->q[i][j] = HUNGARIAN_STAR;
            break;
          }
        }
      }
    }
  }
}

/*
 * Kuhn's Routine I
 */
hungarian_code_t hungarian_routine_one(hungarian_t* prob)
{
  int i,j,k;
  char jumpcase1,jumpprelim;

  assert(prob);
  prob->seq.k=0;
  for(i=0;i<prob->m;i++)
    prob->ess_rows[i]=0;

  j=0;
  // look for a 1* in each column
  while(j<prob->n)
  {
    i=0;
    while(i<prob->m)
    {
      if(prob->q[i][j] == HUNGARIAN_STAR)
      {
        // found a 1*; next column
        break;
      }
      i++;
    }
    if(i<prob->m)
    {
      // found a 1*; next column
      j++;
    }
    else
    {
      // didn't find a 1*; column j is eligible; search it for a 1
      i=0;
      while(i<prob->m)
      {
        if(prob->q[i][j] == HUNGARIAN_ONE)
        {
          // found a 1 (i,j); start recording
          prob->seq.i[prob->seq.k] = i;
          prob->seq.j[prob->seq.k] = j;
          prob->seq.k++;
          // CASE 1
          jumpprelim=0;
          while(!jumpprelim)
          {
            // look in row ik for a 1*
            j=0;
            jumpcase1=0;
            while(j<prob->n && !jumpcase1)
            {
              if(prob->q[i][j] == HUNGARIAN_STAR)
              {
                // CASE 2
                i=0;
                while(!jumpcase1)
                {
                  // found a 1* in (ik,jk); search col jk for a 1
                  while(i<prob->m)
                  {
                    if(prob->q[i][j] == HUNGARIAN_ONE)
                    {
                      // test i for distinctness
                      for(k=0;k<prob->seq.k;k++)
                      {
                        if(prob->seq.i[k] == i)
                          break;
                      }
                      if(k<prob->seq.k)
                      {
                        // i is not distinct
                        i++;
                        continue;
                      }
                      else
                      {
                        // i is distinct; record and back to Case 1
                        prob->seq.i[prob->seq.k] = prob->seq.i[prob->seq.k-1];
                        prob->seq.j[prob->seq.k] = j;
                        prob->seq.k++;
                        prob->seq.i[prob->seq.k] = i;
                        prob->seq.j[prob->seq.k] = j;
                        prob->seq.k++;

                        jumpcase1=1;
                        break;
                      }
                    }
                    else
                      i++;
                  }
                  if(i>=prob->m)
                  {
                    // didn't find a 1 in col jk; row ik is essential
                    prob->ess_rows[prob->seq.i[prob->seq.k-1]] = 1;
                    
                    // delete last two elts of sequence
                    j=prob->seq.j[prob->seq.k-1];
                    i=prob->seq.i[prob->seq.k-1]+1;

                    if(prob->seq.k > 1)
                    {
                      // back to Case 2
                      prob->seq.k-=2;
                      continue;
                    }
                    else
                    {
                      // k==1; back to prelim search for 1 in (i1+1,j0)
                      prob->seq.k--;
                      jumpcase1=jumpprelim=1;
                      break;
                    }
                  }
                }
              }
              else
                j++;
            }
            if(j>=prob->n)
            {
              // didn't find a 1* in row ik; toggle and Alternative Ia
              for(;prob->seq.k > 0;prob->seq.k--)
              {
                if(prob->q[prob->seq.i[prob->seq.k-1]][prob->seq.j[prob->seq.k-1]]
                   == HUNGARIAN_ONE)
                {
                  prob->q[prob->seq.i[prob->seq.k-1]][prob->seq.j[prob->seq.k-1]] 
                          = HUNGARIAN_STAR;
                }
                else
                {
                  prob->q[prob->seq.i[prob->seq.k-1]][prob->seq.j[prob->seq.k-1]] 
                          = HUNGARIAN_ONE;
                }
              }
              // Alternative Ia
              return(HUNGARIAN_ROUTINE_ONE);
            }
          }
        }
        else
          i++;
      }
      // didn't find a 1 in col j; next col
      j++;
    }
  }
  // out of cols; Alternative Ib
  // determine ess cols
  for(j=0;j<prob->n;j++)
  {
    prob->ess_cols[j]=0;
    for(i=0;i<prob->m;i++)
    {
      if(prob->q[i][j] == HUNGARIAN_STAR && !prob->ess_rows[i])
      {
        prob->ess_cols[j] = 1;
        break;
      }
    }
  }
  return(HUNGARIAN_ROUTINE_TWO);
}


/*
 * Kuhn's Routine II
 */
hungarian_code_t hungarian_routine_two(hungarian_t* prob)
{
  int i,j,d,m,dtmp;
  int oldsum,newsum;

  assert(prob);

  oldsum = 0;
  for(i=0;i<prob->m;i++)
    oldsum += prob->u[i];
  for(j=0;j<prob->n;j++)
    oldsum += prob->v[j];

  d=0;
  for(i=0;i<prob->m;i++)
  {
    if(prob->ess_rows[i])
      continue;
    for(j=0;j<prob->n;j++)
    {
      if(prob->ess_cols[j])
        continue;
      dtmp = prob->u[i] + prob->v[j] - prob->r[i][j];
      if(dtmp<0)
      {
        printf("SUPERMOO: %d + %d < %d\n", 
               prob->u[i], prob->v[j], prob->r[i][j]);
      }
      if(!d || ((dtmp>0) && dtmp < d))
        d = dtmp;
    }
  }

  if(d<0)
    printf("MOO: %d < 0\n", d);

  //if(d<=0)
  if(!d)
    return(HUNGARIAN_DONE);
  else
  {
    // is there some u[i]==0?
    for(i=0;i<prob->m;i++)
    {
      if(!prob->u[i])
        break;
    }
    if(i < prob->m)
    {
      // CASE 2; some u[i] == 0; compute m as the min of d and inessential v[j]
      m = d;
      for(j=0;j<prob->n;j++)
      {
        if((!prob->ess_cols[j]) && (prob->v[j] < m))
          m = prob->v[j];
      }

      // adjust the cover
      for(i=0;i<prob->m;i++)
      {
        if(prob->ess_rows[i])
          prob->u[i] += m;
      }
      for(j=0;j<prob->n;j++)
      {
        if(!prob->ess_cols[j])
          prob->v[j] -= m;
      }
    }
    else
    {
      // CASE 1; all u[i] > 0; compute m as the min of d and inessential u[j]
      m = d;
      for(i=0;i<prob->m;i++)
      {
        if((!prob->ess_rows[i]) && (prob->u[i] < m))
          m = prob->u[i];
      }

      // adjust the cover
      for(i=0;i<prob->m;i++)
      {
        if(!prob->ess_rows[i])
          prob->u[i] -=m;
      }
      for(j=0;j<prob->n;j++)
      {
        if(prob->ess_cols[j])
          prob->v[j] += m;
      }
    }
  }

  newsum = 0;

  for(i=0;i<prob->m;i++)
  {
    for(j=0;j<prob->n;j++)
    {
      if(prob->u[i]+prob->v[j]<prob->r[i][j])
      {
        printf("SUPERMOO (%d,%d): %d + %d < %d\n", i,j,
               prob->u[i], prob->v[j], prob->r[i][j]);
      }
    }
  }

  // Alternative IIa; build new q and return to routine I
  hungarian_build_q(prob);
  return(HUNGARIAN_ROUTINE_ONE);
}


/*
 * solve the given problem.  runs the Hungarian Method on the rating matrix
 * to produce optimal assignment, which is stored in the vector prob->a.
 * you must have called hungarian_init() first.
 */
void 
hungarian_solve(hungarian_t* prob)
{
  hungarian_code_t state = HUNGARIAN_ROUTINE_ONE;
  int i,j;

  assert(prob);

  bzero(prob->u,sizeof(int)*prob->m);
  bzero(prob->v,sizeof(int)*prob->n);
  prob->seq.k=0;
  bzero(prob->ess_rows,sizeof(int)*prob->m);
  bzero(prob->ess_cols,sizeof(int)*prob->n);
  for(i=0;i<prob->m;i++)
    bzero(prob->q[i],sizeof(int)*prob->n);

  hungarian_make_cover(prob);
  hungarian_build_q(prob);
  hungarian_add_stars(prob);

  while(state != HUNGARIAN_DONE)
  {
    if(state == HUNGARIAN_ROUTINE_ONE)
    {
      state = hungarian_routine_one(prob);
    }
    else
    {
      state = hungarian_routine_two(prob);
    }
  }

  // fill in the assignment vector
  for(i=0;i<prob->m;i++)
  {
    for(j=0;j<prob->n;j++)
    {
      if(prob->q[i][j] == HUNGARIAN_STAR)
      {
        prob->a[i] = j;
        break;
      }
    }
  }
}

/*
 * prints out the current state of the Q matrix.  also computes and prints
 * out the benefit from the current assignment.  mostly useful for
 * debugging.
 */
void hungarian_print_stars(hungarian_t* prob)
{
  int i,j;
  int benefit=0;
  assert(prob);
  puts("\nQ: ");
  for(i=0;i<prob->m;i++)
  {
    printf("  [ ");
    for(j=0;j<prob->n;j++)
    {
      if(prob->q[i][j] == HUNGARIAN_ZERO)
        printf("%4d ", 0);
      else if(prob->q[i][j] == HUNGARIAN_ONE)
        printf("%4d ", 1);
      else
      {
        printf("%3d%s ", 1,"*");
        if(prob->mode == HUNGARIAN_MIN)
          benefit += prob->maxutil - prob->r[i][j];
        else
          benefit += prob->r[i][j];
      }
    }
    puts(" ]");
  }
  printf("\nBenefit: %d\n\n", benefit);
}

/*
 * prints out the resultant assignment in a 0-1 matrix form.  you must have 
 * called hungarian_solve() first.
 */
void 
hungarian_print_assignment(hungarian_t* prob)
{
  int i,j;
  assert(prob);
  puts("\nA:");
  for(i=0;i<prob->m;i++)
  {
    printf("  [ ");
    for(j=0;j<prob->n;j++)
      printf("%4d ", (j==prob->a[i]) ? 1 : 0);
    printf(" ]\n");
  }
}

/*
 * prints out the rating matrix for the given problem.  you must have called
 * hungarian_solve() first.
 */
void 
hungarian_print_rating(hungarian_t* prob)
{
  int i,j;

  puts("\nR: ");
  for(i=0;i<prob->m;i++)
  {
    printf("  [ ");
    for(j=0;j<prob->n;j++)
    {
      printf("%4d ", prob->r[i][j]);
    }
    puts(" ]");
  }
}

/*
 * check whether an assigment is feasible.  returns 1 if the assigment is
 * feasible, 0 otherwise.  you must have called hungarian_solve() first.
 */
int
hungarian_check_feasibility(hungarian_t* prob)
{
  int i,j;
  char assigned;

  // check for over/under assigned rows
  for(i=0;i<prob->m;i++)
  {
    assigned=0;
    for(j=0;j<prob->n;j++)
    {
      if(prob->q[i][j] == HUNGARIAN_STAR)
      {
        if(assigned)
          return(0);
        else
          assigned=1;
      }
    }
    if((prob->m <= prob->n) && !assigned)
      return(0);
  }
  // check for over/under assigned cols
  for(j=0;j<prob->n;j++)
  {
    assigned=0;
    for(i=0;i<prob->m;i++)
    {
      if(prob->q[i][j] == HUNGARIAN_STAR)
      {
        if(assigned)
          return(0);
        else
          assigned=1;
      }
    }
    if((prob->n <= prob->m) && !assigned)
      return(0);
  }
  return(1);
}

/*
 * computes and returns the benefit from the assignment.  you must have
 * called hungarian_solve() first.
 */
int 
hungarian_benefit(hungarian_t* prob)
{
  int i;
  int benefit=0;
  assert(prob);
  for(i=0;i<prob->m;i++)
  {
    if(prob->mode == HUNGARIAN_MIN)
      benefit += prob->maxutil - prob->r[i][prob->a[i]];
    else
      benefit += prob->r[i][prob->a[i]];
  }

  return(benefit);
}
