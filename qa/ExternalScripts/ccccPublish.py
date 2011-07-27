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
    print "Usage:  python ccccPublish.py"

def publishReport():
    extScriptDir = os.getcwd()
    os.chdir(qaDir)
    baseDir = os.getcwd()
    qaResultsDir = os.path.join(baseDir, "QAResults")
    htmlDir = os.path.join(baseDir, "QAResults","html")
    xmlDir = os.path.join(baseDir, "QAResults", "xml")
    
    xmlList=os.listdir(xmlDir)
    htmlList=[file.replace(".xml", ".html") for file in os.listdir(xmlDir)]
    
    ccccExternalDir = qaResultsDir + "/externalcccc" #need to parametrize this?
    covList = os.listdir(ccccExternalDir)
    
    if(os.path.exists(htmlDir) == False):
        print "Html Results Dir doesn't exist %s" % htmlDir
        exit()

    if(os.path.exists(qaResultsDir) == False):
        print "cccc directory results doesn't exist %s" % ccccExternalDir
        exit()


    """if(os.path.exists(os.path.join(ccccExternalDir , "index.html"))):
            print "************************* Warning ******************************"
            print "cccc Report already Formatted, please remove directory and rerun Publishing script"
            print "************************* ------- ******************************"
            return      
    """
    stylePath = os.path.join(htmlDir, "Styles")
    headString = "".join(open(stylePath + "/head.temp"))
    headString = headString.replace("@@@_PUBLISH_DATE_@@@", str( datetime.now().date()))
    centerString = "".join(open(stylePath + "/center.temp"))
    tailString = "".join(open(stylePath + "/tail.temp"))
        
    #write here format
    xmlReports = [item for item in os.listdir(ccccExternalDir)  if (os.path.splitext( item )[1] == ".xml")]
    ccccLinks = [(os.path.splitext(item)[0] + ".html") for item in xmlReports]
    
    ruleLinksStringIndex = ""
    for link in ccccLinks:
        ruleLinksStringIndex = ruleLinksStringIndex + "<li><a href=\"" + link + "\">" + link[:-9] + "</a></li>\n"
    
    try:
        for reportFile in xmlReports:
            #for each file apply stylesheet
            htmlName = os.path.join(ccccExternalDir, os.path.splitext( reportFile )[0] + ".html")
            print "Formatting in HTML " + htmlName
            #with lxml parse the file
            f = open(os.path.join(ccccExternalDir,reportFile))
            xml = fromstring(str(f.read()))

            #with lxml create html
            absPathXslt = os.path.join(currentPathScript,"cccc.xslt")
       
            fileXslt = open(absPathXslt, 'r') 
            #print xsltH + headString + centerString + str(fileXslt.read()) + tailString + xsltT
            xsl = fromstring(fileXslt.read().replace("@@@_PUBLISH_DATE_@@@", str( datetime.now().date())).replace(" @@@_EXTERNAL_TOOLS_LINKS_@@@",ruleLinksStringIndex).replace("@@@_EXTERNAL_TOOLS_REPORT_@@@", ""))

            style = XSLT(xsl)
            result = style.apply(xml)
           
            #print htmlDir + filename + ".html"
            html = open(os.path.join(ccccExternalDir, htmlName), 'w')
            print >> html , style.tostring(result)    
    
    except Exception, e:
        print "Problem when publishing cccc , error:" , e

    fileIndex = open(ccccExternalDir + "/index.html", 'w');
    fileIndex.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">")
    headStringIndex = headString.replace("href=\"Styles/", "href=\"../html/Styles/")
    headStringIndex = headStringIndex.replace("<li><a href=\"index.html\">Introduction</a></li>", "<li><a href=\"../html/index.html\">Introduction</a></li>")
    headStringIndex = headStringIndex.replace("@@@_EXTERNAL_TOOLS_REPORT_@@@", "") #remove placeholder
    headStringIndex = headStringIndex.replace("Rule Scripts", "Cyclomatic Complexity")
        
    fileIndex.write(headStringIndex)
        
    fileIndex.write(ruleLinksStringIndex)
    fileIndex.write(centerString)
    
    fileIndex.write("""
    <h1> About cccc </h1>
    CCCC - C and C++ Code Counter<br>
    A free software tool for measurement of source code related metrics by Tim Littlefair.<br>
    
    Cyclomatic complexity (or conditional complexity) is a software metric (measurement).<br> 
    It was developed by Thomas J. McCabe, Sr. in 1976 and is used to indicate the complexity of a program.<br>
     It directly measures the number of linearly independent paths through a program's source code.<br> 
     The concept, although not the method, is somewhat similar to that of general text complexity measured by the Flesch-Kincaid Readability Test.<br>
     The cyclomatic complexity of a section of source code is the count of the number of linearly independent paths through the source code.<br> For instance, if the source code contained no decision points such as IF statements or FOR loops, the complexity would be 1, since there is only a single path through the code. If the code had a single IF statement containing a single condition there would be two paths through the code, one path where the IF statement is evaluated as TRUE and one path where the IF statement is evaluated as FALSE.<br><br>
Mathematically, the cyclomatic complexity of a structured programis defined with reference to a directed graph containing the basic blocks of the program, with an edge between two basic blocks if control may pass from the first to the second (the control flow graph of the program). The complexity is then defined as:<br><br>
     M = E ? N + 2P
    """)
    fileIndex.write(tailString)
    fileIndex.close()

    print "******* PUBLISH cccc value *********"
    os.chdir(extScriptDir) 

    

if __name__ == "__main__":
    if(str(sys.platform).lower() == "win32"):
        print "Script not valid on Windows"
        exit()

    disclaimer = """
    ************************************************************
    This script has been written for publishing inside mafQA site
    the results of LCOV Coverage.
    It will process all the .html file in order to create coherent
    html pages.
    ************************************************************\n
    """

    #print disclaimer
    #time.sleep(5)
    
    import subprocess as sp
    result = sp.Popen(["which", "cccc"], stdout=sp.PIPE).communicate()[0]
    found = result.find("cccc")
    if(found == -1):
        print "cccc does not exist -", sys.argv[0]
        print os.environ
        print os.getuid()
        exit()

    publishReport()
