#!/usr/bin/python
import os, sys, serial, time, argparse, glob, random, signal, ctypes
dev_pat = "/dev/LOGGER*"

libc = ctypes.cdll.LoadLibrary("libc.so.6")
mask = '\x00' * 17 # 16 byte empty mask + null terminator 
libc.sigprocmask(2, mask, None)

def connect(dev = None, ntry=-1):
  if dev == None:
    while 1:
      dev = dev_find()
      if dev: break
      else:
        print "Device not found"
        time.sleep(1)
      if ntry > 0: ntry-=1
      if ntry ==0: raise NameError, "Device not found"
  sr = serial.Serial(port=dev, timeout=0.5, baudrate=115200)
  while len(sr.read(1))>0: pass
  return sr
 
def dev_find():
  d = glob.glob(dev_pat)
  if not len(d): return None # raise NameError, "Device not found"
  return d[-1] # Only one device is supported

class FakeSerial:
  def __init__(self, *args):
    pass

  def readline(self):
    return reduce(lambda a,b: a+b, map(lambda n: "%d "%n, map(lambda i: random.randint(0,4096), range(4))))

  def close(self):
    pass

  def write(self,s):
    pass

prs = argparse.ArgumentParser(description="")
prs.add_argument('-p', dest="lockf", metavar='FILENAME', type=str, help='lock file', default='logger.pid')
prs.add_argument('-t', dest='period', metavar='N', type=float, help='data collection period',default=1.0)
prs.add_argument('-o', dest="out", metavar='FILE', type=str, help='output file',default='logger.dat')
prs.add_argument('-f', dest="dev", metavar='/dev/ttyX', type=str, help='device file')
prs.add_argument('-d', dest="daemon",  action="store_true", help='daemonize')
prs.add_argument('-l', dest="log", metavar='FILE', type=str, help='log file',default='logger.log')
args = prs.parse_args()

def bye(sig,fr):
  print "%s Logger stopped"%time.strftime("%Y-%m-%d_%H:%M:%S")
  sys.exit(0)


if args.daemon:
  if os.fork() > 0:
    sys.exit(0)
  os.setsid()
  if os.fork() > 0: sys.exit(0)
  os.umask(0)
  import resource		# Resource usage information.
  maxfd = resource.getrlimit(resource.RLIMIT_NOFILE)[1]
  if (maxfd == resource.RLIM_INFINITY):
    maxfd = 1024
  for fd in range(0, maxfd):
    try:
      os.close(fd)
    except OSError:	# ERROR, fd wasn't open to begin with (ignored)
      pass

signal.signal(signal.SIGTERM,bye)

sys.stdin = open('/dev/null', 'r')
sys.stdout = open(args.log, 'a+',0)
sys.stderr = open(args.log, 'a+',0)


lck = open(args.lockf, 'w')
lck.write(str(os.getpid()))
lck.close()
out = open(args.out, 'w')
#dev = args.dev if args.dev else dev_find()
sr = connect(args.dev, ntry=1)
#while len(sr.read(1))>0: pass
#sr = FakeSerial()


tstart=time.time()
print "%s Logger started"%time.strftime("%Y-%m-%d_%H:%M:%S")
while True:
  try:
    sr.flushInput()
    sr.write("get\n")
    s = sr.readline()
  except (serial.serialutil.SerialException, serial.termios.error) as err:
    print "%s Error communicating to device: %s"%(time.strftime("%Y-%m-%d_%H:%M:%S"),str(err))
    sr.close()
    time.sleep(1)
#      sys.stdout.flush()
    sr = connect() #serial.Serial(port=dev_find(), timeout=0.5)
#    while len(sr.read(1))>0: pass
    print "%s Reconnected to device"%time.strftime("%Y-%m-%d_%H:%M:%S")
  out.write("%.2f"%(time.time()-tstart))
  out.write(reduce(lambda a,b: a+b, map(lambda v: " %s"%v, s.split()))+"\n")
  out.flush()
#  sys.stdout.flush()
  time.sleep(args.period)
