#!/usr/bin/env python
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

""" Routine from gsatst.c
"""
import pact.pgs as pgs
pdb = pgs

import Numeric
from math import *

#def compute_2d_ac_mapping_2(x, y, kmax, lmax):
def compute_2d_ac_mapping_2(kmax, lmax):
    x = Numeric.zeros(kmax * lmax, Numeric.Float)
    y = Numeric.zeros(kmax * lmax, Numeric.Float)

    km = kmax - 1
    lm = lmax - 1

    jo = [0, 1, kmax + 1, kmax, 0]

    xmin = -5.0
    xmax =  5.0
    ymin = -5.0
    ymax =  5.0

    dx = (xmax - xmin) / float(kmax - 1.0)
    dy = (xmax - xmin) / float(kmax - 1.0)
    nc = km*lm

    tol = 1.0e-5

    ord = 2
    nd  = 2

    # setup the number of boundary parameters array 
    # bnp    = CMAKE_N(int, nd+1)
    # for (id = 1; id <= nd; id++):
    #    bnp[id] = ord
    
    bnp = [1]
    for id in range(nd):
        bnp.append(ord)

    # setup the number of cells array
    # bnc = CMAKE_N(int, nd+1)
    bnc = [kmax*lmax, 4*nc, nc]

    # allocate the boundary arrays
    # bnd    = CMAKE_N(long *, nd+1)
    # bnd[0] = NULL;
    # for (id = 1; id <= nd; id++):
    #    bnd[id] = CMAKE_N(long, ord*bnc[id])
    bnd = [None]
    for id in range(nd):
        bnd.append(Numeric.zeros(ord * bnc[id + 1], Numeric.Int32))

    # fill the zone list
    zone = bnd[2]
    ne   = 0
    for i in range(nc):
        zone[ord*i]     = ne
        zone[ord*i + 1] = ne + 3
        ne += 4

    # fill the edge list
#    for (k = 0; k < km; k++):
#        for (l = 0; l < lm; l++):
#	    i  = k + l*km
#            io = k + l*kmax
#            for (j = 0; j < 4; j++):
#                edge[ord*(4*i + j)]     = io + jo[j]
#                edge[ord*(4*i + j) + 1] = io + jo[j+1]
    edge = bnd[1]
    for k in range(km):
        for l in range(lm):
	    i  = k + l*km
            io = k + l*kmax
            for j in range(4):
                edge[ord*(4*i + j)]     = io + jo[j]
                edge[ord*(4*i + j) + 1] = io + jo[j+1]
           
    mt = pdb.mesh_topology(nd, bnp, bnc, bnd)
    print mt

    # fill the points
    for k in range(kmax):
        for l in range(lmax):
            i    = l*kmax + k
            x[i] = k*dx + xmin
            y[i] = l*dy + ymin

    #    f = CMAKE_N(REAL, nc)
    f = Numeric.zeros(nc, Numeric.Float)
    for i in range(nc):
        i1 = edge[ord*(4*i + 0)]
        i2 = edge[ord*(4*i + 1)]
        i3 = edge[ord*(4*i + 2)]
        i4 = edge[ord*(4*i + 3)]

        xa   = 0.25*(x[i1] + x[i2] + x[i3] + x[i4])
        ya   = 0.25*(y[i1] + y[i2] + y[i3] + y[i4])
        r    = sqrt(xa*xa + ya*ya)
        t    = 5.0*atan(ya/(xa + pdb.SMALL))
        r    = pow(r, 0.125)
        fv   = exp(-r)*(1.0 + 0.1*cos(t))
        f[i] = floor(fv/tol)*tol

    # put it all together
    SC_REAL_S = 'real'
    dom = pdb.make_ac_set("Dac", SC_REAL_S, pdb.FALSE, mt, nd, x, y)
#   ran = pdb.make_set("R", SC_REAL_S, pdb.FALSE, 1, bnc[2], 1, f)
    ran = pdb.make_set_1d("R", SC_REAL_S, pdb.FALSE, 1, bnc[2], 1, f)
#    h   = pdb.make_mapping("Dac->R", pdb.AC_S, dom, ran, pdb.Z_CENT, None)
    h   = pdb.mapping("Dac->R", pdb.AC_S, dom, ran, pdb.Z_CENT, None)

    return h


if __name__ == '__main__':
    print compute_2d_ac_mapping_2(10,10)
