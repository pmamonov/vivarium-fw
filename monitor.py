import sys
import matplotlib
matplotlib.use('TkAgg')
from matplotlib.pyplot import plot,draw,figure,cla,xlim
from time import sleep
from numpy import loadtxt

if len(sys.argv) < 2:
  print "USAGE: %s file.dat"%sys.argv[0]
  sys.exit(1)

f=figure()
f.show()

while 1:
  d = loadtxt(sys.argv[1])
  cla()
  map(lambda i: plot(d[-100:,0],d[-100:,i]), range(1,d.shape[1]))
  xlim(d[-100,0],d[-1,0])
  draw()
  sleep(1)
