#-----------------------------------------------------------------------------
# Heuristic for coverage calculation 
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import sys, string
import shutil
import sets
import os
import os.path
from xml.dom import minidom
from xml.dom import Node
import urllib2, sys, BaseHTTPServer
import re
import urlparse
from sgmllib import SGMLParser

class URLLister(SGMLParser):
	def reset(self):
		SGMLParser.reset(self)
		self.urls = []

	def start_a(self, attrs):
		href = [v for k, v in attrs if k=='href']
		if href:
			self.urls.extend(href)

#if __name__ == "__main__":
	#import urllib
	#usock = urllib.urlopen("http://diveintopython.org/")
	#parser = URLLister()
	#parser.feed(usock.read())
	#parser.close()
	#usock.close()
	#for url in parser.urls: print url


class coverageCalculator:

    def __init__(self):
        self.DoxygenClassIndexWebAdress = "None"
        self.ParabuildDashboardWebAdress = "None"
	self.DashboardName = "None"
        
    def __parseAddress(self, input):
            if input[:7] != "http://":
                    if input.find("://") != -1:
                            print "Error: Cannot retrive URL, address must be HTTP"
                            sys.exit(1)
                    else:
                            input = "http://" + input
    
            return input
    
    def __retrieveWebPage(self, address):
            try:
                    web_handle = urllib2.urlopen(address)
            except urllib2.HTTPError, e:
                    error_desc = BaseHTTPServer.BaseHTTPRequestHandler.responses[e.code][0]
                    #print "Cannot retrieve URL: " + str(e.code) + ": " + error_desc
                    print "Cannot retrieve URL: HTTP Error Code", e.code
                    sys.exit(1)
            except urllib2.URLError, e:
                    print "Cannot retrieve URL: " + e.reason[1]
                    sys.exit(1)
            except:
                    print "Cannot retrieve URL: unknown error"
                    sys.exit(1)
            return web_handle
    
    def CountClassess(self):
            match_set = sets.Set()
    
            address = self.__parseAddress(self.DoxygenClassIndexWebAdress)
            website_handle = self.__retrieveWebPage(address)
            website_text = website_handle.read()
            
            matches = re.findall('class="el" href="class', website_text)
            
            # print matches
            # print len(matches)
	    return len(matches)
        
    #def CountClassess(self):
        #match_set = sets.Set()
        
        #address = self.__parseAddress(self.ParabuildDashboardWebAdress)
        #website_handle = self.__retrieveWebPage(address)
        #website_text = website_handle.read()
        
        ##lp = LinkParser(website_text)
        ##links = lp.asAbsolute(self.ParabuildDashboardWebAdress)        
        ### print links
        
       ##         # print website_text
        #matches = sre.findall('ALBA_ITK_VC71D</a>', website_text)
        
        ## print matches
        ## print len(matches)
        #return len(matches)
            
    
    def CountTests(self):	
	
	dirToParse = ['Testing','vtkALBA']
	startDir = os.getcwd()    
	# print startDir
	os.chdir(os.pardir)
	rootDir = os.getcwd()
	dirList = os.listdir(rootDir)
	# Count tests in dir 	
	
	# print dirList
	assert(dirList)
	
	
	# for every testing directory
	
	# Count number of tests
	dir_count, file_count=0, 0
	filesList = []
	for root, dirs, files in os.walk('.'):
		dir_count += len(dirs)
		file_count += len(files)

		#print 'Found', dir_count, 'sub-directories in cwd'
		#print 'Found', file_count, 'files in cwd'
		#print 'Found', dir_count + file_count, 'files & sub-directories in cwd'
		filesList.append(files)
	
	# search tests
	assert(isinstance(files, list))
	
	text = str(filesList)
	# print text
	
	matches = re.findall("Test.h",text)
            
	# print matches
 	return len(matches)

    def WriteLog(self):
	    
	coverageLog = open('coverageLog.txt', 'w')
	coverageLog.writelines("COVERAGE INDEX: \n\n")

	numClasses = self.CountClassess()
	coverageLog.write("Number of Classes: ")
	coverageLog.write(str(numClasses))
	coverageLog.write('\n')
	coverageLog.write('\n')
	numTests = self.CountTests()
	coverageLog.write("Number of Unit Tests (Takes into consideration also non executed tests: TO BE FIXED!!): ")
	coverageLog.write(str(numTests))
	coverageLog.write('\n')
	coverageLog.write('\n')
        coverageIndex = float(numTests)/float(numClasses)
	coverageLog.write("Coverage Index = NumUnitTests / NumClasses = ")
	coverageLog.write(str(coverageIndex))
	coverageLog.write('\n')
	coverageLog.close()
	
def run(doxygenClassIndexWebAdress):                                            
        cc = coverageCalculator()
	cc.DoxygenClassIndexWebAdress = doxygenClassIndexWebAdress
        cc.WriteLog()
	
    
def main():
    args = sys.argv[1:]
    if len(args) < 1:
        print 'usage: python.exe DoxygenClassIndexWebAdress'
        sys.exit(-1)
    run(args[0])

if __name__ == '__main__':
    main()
