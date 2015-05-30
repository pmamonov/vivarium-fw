import mod_python, os

def index(req):
	return """
<html><body>
<h2><a href=halt.py/halt>HALT PILAB</a></h2>
</body></html>
"""

def halt(req):
	os.system("/sbin/halt -p")
	return """
<html><body>
<h2>BYE-BYE</h2>
</body></html>
"""
