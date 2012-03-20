#-----------------------------------------------------------------------------
# Heuristic for coverage calculation 
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import coverageScript
import os
import sys, string
import unittest

import urllib, urllib2


class coverageScriptTest(unittest.TestCase):
      
    
    def testCountClassesMedical(self):
        cc = coverageScript.coverageCalculator()
        cc.DoxygenClassIndexWebAdress = "http://doc.openmaf.org/medical/classes.html"
	numClasses = cc.CountClassess()
	print str(cc.DoxygenClassIndexWebAdress) + " has " + str(numClasses) + " classes"
	
    
    def testCountTests(self):
	cc = coverageScript.coverageCalculator()
	numTests = cc.CountTests()
	print os.getcwd() + " has " + str(numTests) + " tests"
	
	
    def testGenerateLog(self):
	print os.getcwd()
	cc = coverageScript.coverageCalculator()
        cc.DoxygenClassIndexWebAdress = "http://doc.openmaf.org/classes.html"
	cc.WriteLog()
    
    def UrlLister(self):
	usock = urllib.urlopen("http://195.250.34.10:8080/parabuild/index.htm")
	parser = coverageScript.URLLister()
	parser.feed(usock.read())
	parser.close()
	usock.close()
	for url in parser.urls: print url
    
if __name__ == '__main__':
    unittest.main()
    
