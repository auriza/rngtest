#!/usr/bin/env python
import subprocess
import cgi, cgitb

form  = cgi.FieldStorage()
infile = form.getvalue('infile')

print "Content-Type: text/html"
print
print "<html><body>"
print "<h2>RNG Test</h2>"

print """\
<form method="GET" action="">
<p>Input file:<br><input name="infile"></p>
<p><input type="submit" value="Go" ></p>
</form>
"""

print "Input file: " + infile

print "<pre>"
output = subprocess.check_output(['./main', infile])
print output
print "</pre>"

print "</body></html>"
