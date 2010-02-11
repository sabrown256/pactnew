c
c B test problem
c
c Source Version: 3.0
c Software Release #: LLNL-CODE-422942
c

geometry slab

part high-density
side  0.0  0.0   4  0
side 10.0  0.0   0  2
side 10.0  4.0  -4  0
side  0.0  4.0   0 -2

part low-density
side 10.0  0.0   4  0
side 20.0  0.0   0  2
side 20.0  4.0  -4  0
side 10.0  4.0   0 -2

material high-density 0.10 0 1.009 1.0 500.0
material low-density  0.01 0 1.009 1.0 500.0

package global

start-time 0.0
stop-time 1.0e-8

parameter 4 0.2    rig for 5 cycles
parameter 5 0.2
parameter 6 0.2

package lag
parameter 12 0.5   rig for 2 subcycles
parameter 13 0.5

specify te from tsrc1.src interpolate

graph tm(t, rx=10.0, ry=2.0)
graph rho(t, rx=10.0, ry=2.0)
graph vx(t, rx=10.0, ry=2.0)

graph tm(t, rx, ry)
graph rho(t, rx, ry)
graph {vx, vy}(t, rx, ry)

make
dump
c turn mesh on
c turn boundary on
c view
end
