import glob, time, os, mod_python, numpy, sys, re
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
    s += '<form method="get" action="'+wwwroot+"weblogger.py/start"+ '">File name:<input type="text" name="filename"> Period, sec: <input type="text" name="period"><input type="submit" value="START"></form><hr>'
  dat = glob.glob(rootdir+"*.dat")
  dat.sort(reverse=1)
  for f in dat:
    fn = f.split('/')[-1]
    s+="<a href='%s' style='color:green'>%s</a>  <a href='%s' style='color:red;'>delete</a><hr>"%(wwwroot+fn, fn, wwwroot+"weblogger.py/delete?fn=%s"%fn)
  return s+"</body></html>"

def start(req,filename,period):
  if not is_running():
    filename = re.sub('[^a-zA-Z0-9_.-]', '_', filename.strip())
    dat = rootdir+"%s.dat" % filename
    if os.path.isfile(dat):
      return """<html><body>
<h2>ERROR: File already exists<hr>
<a href=%sweblogger.py>BACK</a></h2>
</body></html>
""" % wwwroot
    lockf = rootdir+"logger.pid"
    log = rootdir+"logger.log"
    os.system(reduce(lambda a,b: a+" "+b, (logger,'-t',period,'-p',lockf,'-o',dat,'-l',log,'-d'),'python'))
  time.sleep(1)
  mod_python.util.redirect(req,wwwroot+"weblogger.py")

def stop(req):
  pid = is_running()
  if pid: os.kill(pid,15)
  time.sleep(1)
  mod_python.util.redirect(req,wwwroot+"weblogger.py")

def view(req,fn):
  s="""
<html>
  <head>
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      google.setOnLoadCallback(drawChart);
      var data;
      var chart;
      var options;
      var scale=[];
      function drawChart() {
        data = google.visualization.arrayToDataTable([
  """
#  d = numpy.loadtxt(rootdir+fn)#[-1000:,:]
  d = open(rootdir+fn)
  ncol = len(d.readline().split(' '))
#  dt = float(d.readline().split(' ')[0])
  nr=2
  while d.readline(): nr +=1
  d.seek(0)
#  dn = int(10./dt)
  dn = int(nr/1e3)
  if dn==0: dn=1
#  if nr<=1000: dn=1
  s+="['time'"
  s+=reduce(lambda a,i: "%s,'%d'"%(a,i), range(1,ncol),'')
  s+=']'

  il=0
  for l in d: 
    if il%dn == 0: s += ",[%s]"%(l.strip().replace(' ',','))
#    if il>=100: break
    il+=1
  d.close()
#  for i in xrange(d.shape[0]):
#    s+='[%.2f'%d[i,0]
#    s+=reduce(lambda a,b: "%s,%d"%(a,b),d[i,1:],'')
#    s+=']'
#    if i<d.shape[0]-1: s+=',\n'
  s+="""
        ]);

        options = {
          title: 'ADC values',
//          explorer: {axis: 'horizontal', maxZoomIn: 0.01}
          explorer: {maxZoomIn: 0.01}
        };

        for (var i=1; i<data.getNumberOfColumns(); i++){scale.push(1);};

        chart = new google.visualization.LineChart(document.getElementById('chart_div'));
        chart.draw(data, options);
      }

      function scale_data(){
        nr = data.getNumberOfRows();
        for (var c=1; c<data.getNumberOfColumns(); c++){
          var s = parseFloat(document.getElementById("scale"+c.toString()).value);
          for (var i=0; i<nr; i++){
            data.setValue(i,c,scale[c-1]/s*data.getValue(i,c));
          };
          scale[c-1]=s;
        }
        chart.draw(data, options)
      }
    </script>
  </head>
  <body>
  """
  s += "<b>units per gram: </b>"
  s += reduce(lambda a,b: a+b, map(lambda i: '%d:<input type="text" size="5" id="scale%d" value="1"> '%(i,i), range(1,ncol)))
  s+= '<input type="button" value="Apply" onClick="scale_data();">'
  s+="""
    <div id="chart_div" style="width: 100%; height: 100%;"></div>
  </body>
</html>
  """
  return s

def delete(req,fn):
  if os.path.isfile(rootdir+fn): os.unlink(rootdir+fn)
  mod_python.util.redirect(req,wwwroot+"weblogger.py")

