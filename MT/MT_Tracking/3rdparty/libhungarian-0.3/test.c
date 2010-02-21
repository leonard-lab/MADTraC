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

#define USAGE "Usage: ./test [-m <m>] [-n <n>] [-f <fname>]"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/* one of the following will define PATH_MAX */
#include <limits.h> 
#include <sys/param.h>

#include "hungarian.h"

#ifndef max
  #define max(a, b) ((a) > (b) ? (a) : (b))
#endif

// problem dimensions
int m,n;

// input data file
char input_fname[PATH_MAX];

void parse_args(int argc, char** argv);

int
main(int argc, char** argv)
{
  hungarian_t prob;

  /*
  int r[4][4] =  {{  363, 626, 376,  46  },
                  {  802, 993, 784, 953  },
                  {  673,  23, 823, 207  },
                  {  380, 451, 520, 646  }};
                  */
  int* r;
  
  m = n = 4;

  parse_args(argc,argv);

  if(!strlen(input_fname))
    r = make_random_r(m,n);
  else 
    r = make_r_from_ORlib(input_fname,&m,&n);

  if(!r)
  {
    puts("got NULL input");
    exit(-1);
  }

  hungarian_init(&prob,(int*)r,m,n,HUNGARIAN_MIN);
  hungarian_print_rating(&prob);
  hungarian_solve(&prob);
  hungarian_print_assignment(&prob);

  printf("\nfeasible? %s\n", 
         (hungarian_check_feasibility(&prob)) ? "yes" : "no");
  printf("benefit: %d\n\n", hungarian_benefit(&prob));

  hungarian_fini(&prob);
  free(r);
  return(0);
}

void
parse_args(int argc, char** argv)
{
  int i;
  // parse command-line args
  for(i=1; i<argc; i++)
  {
    if(!strcmp(argv[i],"-m"))
    {
      if(++i < argc)
      {
        m = atoi(argv[i]);
      }
      else
      {
        puts(USAGE);
        exit(-1);
      }
    }
    else if(!strcmp(argv[i],"-n"))
    {
      if(++i < argc)
      {
        n = atoi(argv[i]);
      }
      else
      {
        puts(USAGE);
        exit(-1);
      }
    }
    else if(!strcmp(argv[i],"-f"))
    {
      if(++i < argc)
      {
        strncpy(input_fname,argv[i],sizeof(input_fname));
      }
      else
      {
        puts(USAGE);
        exit(-1);
      }
    }
    else
    {
      puts(USAGE);
      exit(-1);
    }
  }
}

