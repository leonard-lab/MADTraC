#!/usr/local/bin/python

from math import *

def linreg(p):
    """ Returns coefficients to the regression line "y=ax+b" """
    """ Args: list of point pairs [(x1, y2), (x2, y2), ...]"""

    from math import sqrt

    N = len(p)
    Sx = Sy = Sxx = Syy = Sxy = 0.0
    for (x, y) in p:
        Sx = Sx + x
        Sy = Sy + y
        Sxx = Sxx + x*x
        Syy = Syy + y*y
        Sxy = Sxy + x*y
    det = Sxx * N - Sx * Sx
    a, b = (Sxy * N - Sy * Sx)/det, (Sxx * Sy - Sx * Sxy)/det

    meanerror = residual = 0.0
    for (x, y) in p:
        meanerror = meanerror + (y - Sy/N)**2
        residual = residual + (y - a * x - b)**2
    RR = 1 - residual/meanerror
    ss = residual / (N-2)
    Var_a, Var_b = ss * N / det, ss * Sxx / det

    return (a, b, Var_a, Var_b)

p = [(100, 0.046824),
     (150,0.087366),
     (200,0.181394),
     (250,0.308348),
     (300,0.525682),
     (350,0.741806),
     (400,1.048643),
     (450,1.662104),
     (500,3.387051),
     (550,3.554838),
     (600,4.374748),
     (650,9.634283),
     (700,34.190948),
     (750,22.968609),
     (800,78.130386),
     (850,142.174940),
     (900,438.810002)]

logp = []
for (x,y) in p:
  logp.append((log(x),log(y)))

(a,b,va,vb) = linreg(logp)

print 'm: ' + `a`
print 'c: ' + `e**b`
