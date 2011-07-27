import sys
import os
import time
from datetime import datetime
import glob
import lxml.etree as etree
from lxml.etree import XSLT,fromstring

currentPathScript = os.path.split(os.path.realpath(__file__))[0]

try:
    sys.path.append(os.path.realpath(os.path.join(currentPathScript,"..","..")))
    from qa import mafPath
except ImportError:
    import mafPath


modulesDir = mafPath.mafSourcesDir
qaDir = mafPath.mafQADir

def usage():
    print "Usage:  python cppcheckPublish.py"

def publishReport():
    extScriptDir = os.getcwd()
    os.chdir(qaDir)
    baseDir = os.getcwd()
    qaResultsDir = os.path.join(baseDir,"QAResults")
    htmlDir = os.path.join(baseDir,"QAResults","html")
    xmlDir = os.path.join(baseDir,"QAResults","xml")
    
    CPPCheckStaticAnalisysDir = os.path.join(qaResultsDir, "externalcppcheck") #need to parametrize this?
    reportFile = os.path.join(CPPCheckStaticAnalisysDir, "report.xml");
    
    try:
       filename = reportFile
       print "Formatting in HTML " + filename
       #with lxml parse the file
       f = open(filename)
       xml = fromstring(str(f.read()))

       #with lxml create html
       absPathXslt = os.path.join(currentPathScript,"cppcheck.xslt")
       
       fileXslt = open(absPathXslt, 'r') 
       #print xsltH + headString + centerString + str(fileXslt.read()) + tailString + xsltT
       xsl=  fromstring(fileXslt.read().replace("@@@_PUBLISH_DATE_@@@", str( datetime.now().date())))

       style = XSLT(xsl)
       result = style.apply(xml)
           
       #print htmlDir + filename + ".html"
       html = open(os.path.join(CPPCheckStaticAnalisysDir, "report.html"), 'w')
       print >> html , style.tostring(result)
       
       #create introduction for cppcheck
       headString = "".join(open(os.path.join(htmlDir,  "Styles", "head.temp")))
       headString = headString.replace("@@@_PUBLISH_DATE_@@@", str( datetime.now().date()))
       centerString = "".join(open(os.path.join(htmlDir,"Styles", "center.temp")))
       tailString = "".join(open(os.path.join(htmlDir, "Styles", "tail.temp")))
       
       #modify headstring adding cppcheck stylesheet, and substitute relative path
       pos = headString.find("</head>")-1
       headStringNew = headString
                    
                                
       headStringNew = headString[:pos] + "<link rel=\"stylesheet\" type=\"text/css\" href=\"../html/Styles/gcov.css\">\n" + headString[pos:]
       headStringNew = headStringNew.replace("href=\"Styles/", "href=\"../html/Styles/")
       headStringNew = headStringNew.replace("<li><a href=\"index.html\">Introduction</a></li>", "<li><a href=\"../html/index.html\">Introduction</a></li>")
       headStringNew = headStringNew.replace("@@@_EXTERNAL_TOOLS_REPORT_@@@", "") #remove placeholder
       headStringNew = headStringNew.replace("Rule Scripts", "Static Analysis")
       fileIndex = open(os.path.join(CPPCheckStaticAnalisysDir, "index.html"), 'w');
       fileIndex.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">")
        
       fileIndex.write(headStringNew)
       ruleLinksStringIndex = "<li><a href=\"" + "./report.html\">" + "Cppcheck Report" + "</a></li>\n"
        
       fileIndex.write(ruleLinksStringIndex)
       fileIndex.write(centerString)
       fileIndex.write("""
       <h1> Static Analysis Using CPPCHECK</h1>
       Cppcheck is an analysis tool for C/C++ code. <br>
       Unlike C/C++ compilers and many other analysis tools, it don't detect syntax errors. <br>
       Cppcheck only detects the types of bugs that the compilers normally fail to detect. The goal is no false positives.<br>
       Currently the CPPCHECK version used is the 1.4.7, and the check has been executed on the root directory of the MAF framework.<br>
       <br>Some features:<br>
        
       <ul><li>Out of bounds checking
        <li>Check the code for each class</li>
        <li>Checking exception safety</li>
        <li>Memory leaks checking</li>
        <li>Warn if obsolete functions are used</li>
        <li>Check for invalid usage of STL</li>
        <li>Check for uninitialized variables and unused functions</li>
       </ul>
       <br>
       <h2> <a href="./report.html">Go to Report table</a> </h2>
       <br>
       """)
       fileIndex.write(tailString)
       fileIndex.close()
         
    except Exception, e:
        print "Problem when publishing cppcheck, error:" , e

    print "******* PUBLISH cppcheck report *********"
    os.chdir(extScriptDir) 

    

if __name__ == "__main__":
    publishReport()
