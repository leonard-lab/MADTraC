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

#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32  /* mmap and unistd.h not available on win32 (DTS 1/19/10) */
#include <sys/mman.h> // for mmap
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
 * routines to make assignment problems, e.g., randomly or from stored data
 */

#define MAXUTIL 1000

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
  for(i=0;i<m;i++)
  {
    for(j=0;j<n;j++)
    {
      r[i*n+j] = 1+(rand() % MAXUTIL);
    }
  }
  return(r);
}

#ifndef _WIN32  /* mmap not available on win32 (DTS 1/19/10) */
int*
make_r_from_ORlib(char* fname, int* m, int* n)
{
  int fd;
  int size;
  char* filebuf;
  int fileidx,tmpfileidx;
  int* r;
  int filesize;
  int i;

  if((fd = open(fname,O_RDONLY)) < 0)
  {
    perror("open()");
    return(NULL);
  }

  // find out how big the file is - we need to mmap that many bytes
  filesize = lseek(fd, 0, SEEK_END);

  // map the input file into memory, so we can read it
  if((filebuf = (char*)mmap(0, filesize, PROT_READ, 
                            MAP_PRIVATE, fd, (off_t)0)) == MAP_FAILED)
  {
    perror("mmap()");
    return(NULL);
  }

  close(fd); // can close fd once mapped

  // get the size of the problem
  if(sscanf(filebuf,"%d%n",&size,&fileidx) != 1)
  {
    puts("failed to get problem size from file");
    return(NULL);
  }

  *m = *n = size;

  assert(r = malloc(sizeof(int)*size*size));

  i=0;
  while(fileidx < filesize)
  {
    // read the next value, grabbing another line if necessary
    if(sscanf(filebuf+fileidx,"%d%n", r+i, &tmpfileidx) != 1)
      break;
    fileidx += tmpfileidx;
    i++;
  }
  return(r);
}
#endif
