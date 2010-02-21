#! /usr/bin/env python
import sys

sys.path += ['build/lib.linux-i686-2.2/']

import hungarian

A = [[1,2,3],[4,5,6],[7,8,9]]

h = hungarian.HungarianSolver(A)



