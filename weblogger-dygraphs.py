import glob, time, os, mod_python, numpy, sys
rootdir = "/var/www/logger/"
wwwroot = "/logger/"
logger = "/home/pi/logger.py"

def is_running():
  if not os.path.isfile(rootdir+"logger.pid"): return 0
  f=open(rootdir+"logger.pid",'r')
  pid=f.readline()
  f.close()
  if not os.path.isfile("/proc/%s/cmdline"%pid): return 0
  f=open("/proc/%s/cmdline"%pid,'r')
  cmd = f.readline().split('\x00')
  if len(cmd)<2: return 0
  if not (cmd[0]=='python' and cmd[1].split('/')[-1]=='logger.py'): return 0
  return int(pid)


def index(req):
  s="<html><body>"
  if is_running():
    s+="<h2>Logger is running</h2>"
    s+='<form method="get" action="'+wwwroot+"weblogger.py/stop"+ '"> <input type="submit" value="STOP"></form><hr>'
  else:
    s+="<h2>Logger is stopped</h2>"
    s+='<form method="get" action="'+wwwroot+"weblogger.py/start"+ '"><input type="text" name="period"><input type="submit" value="START"></form><hr>'
  dat = glob.glob(rootdir+"*.dat")
  dat.sort(reverse=1)
  for f in dat:
    fn = f.split('/')[-1]
    s+="<a href=%s>%s</a>  <a href='%s' style='color:green'>download</a>  <a href='%s' style='color:red;'>delete</a><hr>"%(wwwroot+"weblogger.py/view?fn=%s"%fn, fn, wwwroot+fn,wwwroot+"weblogger.py/delete?fn=%s"%fn)
  return s+"</body></html>"

def start(req,period):
  if not is_running():
    dat = rootdir+"%s.dat"%time.strftime("%Y-%m-%d_%H:%M:%S")
    lockf = rootdir+"logger.pid"
    log = rootdir+"logger.log"
#    cmdlogger = "python "+logger+" -t "+period+" -p "+lockf+" -o "+dat
#    cmd = "(echo `date` START; echo "+cmdlogger+"; "+cmdlogger+"; echo `date` STOP) >> "+log+" 2>&1 &"
    os.system(reduce(lambda a,b: a+" "+b, (logger,'-t',period,'-p',lockf,'-o',dat,'-l',log,'-d'),'python'))
  time.sleep(1)
  mod_python.util.redirect(req,wwwroot+"weblogger.py")

def stop(req):
  pid = is_running()
  if pid: os.kill(pid,15)
  time.sleep(1)
  mod_python.util.redirect(req,wwwroot+"weblogger.py")

def view(req,fn):
  d = open(rootdir+fn)
  ncol = len(d.readline().split(' '))
  d.close()
  s="""
<html>
  <head>
  <script type="text/javascript" src="../dygraph-combined.js"></script>
  </head>
  <body>
  """
  s += "<b>units per gram: </b>"
  s += reduce(lambda a,b: a+b, map(lambda i: '%d:<input type="text" size="5" id="scale%d" value="1"> '%(i,i), range(1,ncol)))
  s+= '<input type="button" value="Apply" onClick="scale_data();">'
  s+='<div id="gdiv" style="width: 100%; height: 100%;"></div>'
  s+="""
    <script type="text/javascript">
      g2 = new Dygraph(document.getElementById("gdiv"),
      "%s", // path to CSV file
      { delimiter:" ", 
        labels:['time'%s],
/*        axes:{
          y:{
            valueFormatter: function(y, opts, series_name) {
              var m = y/100;
              return m;
            }
          }
        }
*/
      }
      );
    </script>
  </body>
</html>
  """%(wwwroot+fn, reduce(lambda a,b: a+b, map(lambda i: ",'%d'"%i, range(1,ncol))))
  return s

def delete(req,fn):
  if os.path.isfile(rootdir+fn): os.unlink(rootdir+fn)
  mod_python.util.redirect(req,wwwroot+"weblogger.py")

