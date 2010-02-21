/*****************************************************************************\
**                                                                           **
** File: pyhungarian.h                                                       **
** Author: Dylan Shell                                     Date: Oct 25 2003 **
** Description: Python bindings for libhungarian, so that you can use Beef   **
** Gerkey's implementation of the hungarian method.                          **
**                                                                           **
\*****************************************************************************/


#ifndef _PYHUNGARIAN___H_
#define _PYHUNGARIAN___H_

#include "hungarian.h"
#include "Python.h"


typedef struct {
        PyObject_HEAD
        hungarian_t hungarian_obj;
        int *utility_matrix;
} HungObj;

#define PYDEBUG

#ifdef PYDEBUG
#include <stdio.h>
#define DEBUG_MSG1(x) fprintf(stderr,x)
#else
#define DEBUG_MSG1(x) 
#endif

#endif


