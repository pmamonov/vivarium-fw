#!/usr/bin/python
import sys, serial, time

sr = serial.Serial(port=sys.argv[1], timeout=0.1)
while True:
  sr.write("get\n")
  s=""
  while True:
    s+=sr.read()
    if s[-1] == '\n':
      print "%.2f  %s"%(time.time(), s),
      sys.stdout.flush()
      break
  time.sleep(1.0)
