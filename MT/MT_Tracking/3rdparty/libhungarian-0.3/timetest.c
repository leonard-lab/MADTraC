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
 * a test/example program for the C implementation of the Hungarian method
 */

#define USAGE "Usage: ./test [-m <m>] [-n <n>]"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>

#include "hungarian.h"

#define MAXUTIL 1000

#ifndef max
  #define max(a, b) ((a) > (b) ? (a) : (b))
#endif

int* make_random_r(size_t m, size_t n);

int
main(int argc, char** argv)
{
  struct timeval start, end;
  double runtime;
  hungarian_t prob;
  int* r;
  int i;
  int m,n;
  int min = 10;
  int max = 900;
  int step = 50;
  
  printf("# hungarian runtimes (in seconds) populations in range(%d,%d,%d)\n",
         min,max,step);
  i=0;
  while(i < 10)
  {
    m = n = min;

    while(n <= max)
    {
      r = make_random_r(m,n);

      hungarian_init(&prob,(int*)r,m,n,HUNGARIAN_MAX);

      gettimeofday(&start,NULL);
      hungarian_solve(&prob);
      gettimeofday(&end,NULL);

      hungarian_fini(&prob);
      free(r);

      runtime = (end.tv_sec + end.tv_usec/1000000.0) -
              (start.tv_sec + start.tv_usec/1000000.0);

      printf("%lf ", runtime);
      fflush(NULL);

      n = m += step;
    }
    puts("");
    i++;
  }
  return(0);
}

/*
 * makes and returns a pointer to an mXn rating matrix with values uniformly 
 * distributed between 1 and MAXUTIL
 *
 * allocates storage, which the caller should free().
 */
int*
make_random_r(size_t m, size_t n)
{
  int i,j;
  int* r;
  time_t curr;
  assert(r = malloc(sizeof(int)*m*n));
  curr = time(NULL);
  srand(curr);
  //puts("\nINPUT: ");
  for(i=0;i<m;i++)
  {
    //printf("  [ ");
    for(j=0;j<n;j++)
    {
      r[i*n+j] = 1+(rand() % MAXUTIL);
      //printf("%4d ", r[i*n+j]);
    }
    //puts(" ]");
  }
  return(r);
}

