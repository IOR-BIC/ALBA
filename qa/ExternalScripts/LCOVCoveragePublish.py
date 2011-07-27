import sys
import os
import time
from datetime import datetime
import glob

currentPathScript = os.path.split(os.path.realpath(__file__))[0]

try:
    sys.path.append(os.path.realpath(os.path.join(currentPathScript,"..","..")))
    from qa import mafPath
except ImportError:
    import mafPath

def usage():
    print "Usage:  python LCOVCoveragePublish.py"

def publishReport():
    extScriptDir = os.getcwd()
    os.chdir(mafPath.mafQADir)
    baseDir = os.getcwd()
    qaResultsDir = os.path.join(baseDir,"QAResults")
    htmlDir = os.path.join(baseDir,"QAResults", "html")
    xmlDir = os.path.join(baseDir, "QAResults", "xml")
    
    xmlList=os.listdir(xmlDir)
    htmlList=[file.replace(".xml", ".html") for file in os.listdir(xmlDir)]
    
    LCOVExternalCoverageDir = os.path.join(qaResultsDir,"externalLCOVCoverage")
    covList = os.listdir(LCOVExternalCoverageDir)
    
    if(os.path.exists(htmlDir) == False):
        print "Html Results Dir doesn't exist %s" % htmlDir
        exit()

    if(os.path.exists(qaResultsDir) == False):
        print "LCOV external directory results doesn't exist %s" % LCOVExternalCoverageDir
        exit()

    try:
        headString = "".join(open(os.path.join(htmlDir,"Styles", "head.temp")))
        headString = headString.replace("@@@_PUBLISH_DATE_@@@", str( datetime.now().date()))
        centerString = "".join(open(os.path.join(htmlDir, "Styles", "center.temp")))
        tailString = "".join(open(os.path.join(htmlDir, "Styles" , "tail.temp")))
        #fileList = []
        
        if(os.path.exists(os.path.join(LCOVExternalCoverageDir, "index.html"))):
            print "************************* Warning ******************************"
            print "LCOV Report already Formatted, please remove directory and rerun Publishing script"
            print "************************* ------- ******************************"
            return      
                
        #loop over all html files and extract text between <body> </body>
        for root, subFolders, files in os.walk(LCOVExternalCoverageDir):
            for item in files:
                if(os.path.splitext( item )[-1] == ".html"):
                    fullPath = os.path.join(root,item)
                    #fileList.append(fullPath)
                    f = open(fullPath , 'r')
                    text = "".join(f.readlines())
                    checkIfAlreadyFormatted = False

                    innerFileHtml = False    
                    if(item.find(".cpp.") >= 0  or item.find(".h.")>=0):
                        innerFileHtml = True

                    #print item , innerFileHtml

                    if(innerFileHtml == True):
                        f.close()
                        continue
                    
                    bodyStart = str(text).find("<body>") + len("<body>")
                    bodyEnd = str(text).find("</body>")
                    text = text[bodyStart:bodyEnd]
                    f.close()

                    #check if the current directory contains gcov.css
                    moduleCoverageDirectory = False
                    
                    if(os.path.exists(os.path.join(root,"gcov.css"))):
                        moduleCoverageDirectory = True
                    
                    #modify headstring adding LCOV stylesheet, and substitute relative path
                    pos = headString.find("</head>")-1
                    headStringNew = headString
                    
                    ruleLinksString = ""
                                        
                    if(moduleCoverageDirectory == True):
                        headStringNew = headString[:pos] + "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../html/Styles/gcov.css\">\n" + headString[pos:]
                        headStringNew = headStringNew.replace("href=\"Styles/", "href=\"../../html/Styles/")
                        headStringNew = headStringNew.replace("<li><a href=\"index.html\">Introduction</a></li>", "<li><a href=\"../../html/index.html\">Introduction</a></li>")
                        for link in covList:
                            ruleLinksString = ruleLinksString + "<li><a href=\"../" + link + "/index.html\">" + link[:-8] + "</a></li>\n"
                    else:
                        headStringNew = headString[:pos] + "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../../../html/Styles/gcov.css\">\n" + headString[pos:]
                        headStringNew = headStringNew.replace("href=\"Styles/", "href=\"../../../../html/Styles/");
                        headStringNew = headStringNew.replace("<li><a href=\"index.html\">Introduction</a></li>", "<li><a href=\"../../../../html/index.html\">Introduction</a></li>")
                        for link in covList:
                            ruleLinksString = ruleLinksString + "<li><a href=\"../../../" + link + "/index.html\">" + link[:-8] + "</a></li>\n"

                    #modify strings according to the path of the script, and remove placeholder
                    headStringNew = headStringNew.replace("@@@_EXTERNAL_TOOLS_REPORT_@@@", "") #remove placeholder
                    headStringNew = headStringNew.replace("Rule Scripts", "coverage modules")
                    
                           
                    #modify body modifying some table parameters
                    text = text.replace("<table border=0 cellspacing=0 cellpadding=1>", "<table border=0 cellspacing=0 cellpadding=1 width=102>")
                    
                    #write into the same file
                    f = open(fullPath , 'w')
                    f.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">")
                    f.write(headStringNew)
                    f.write(ruleLinksString)
                    f.write(centerString)
                    f.write(text)
                    f.write(tailString)
                    f.close()
        #print "\n".join(fileList)
        #create index file 
        
        
        fileIndex = open(LCOVExternalCoverageDir + "/index.html", 'w');
        fileIndex.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">")
        headStringIndex = headString.replace("href=\"Styles/", "href=\"../html/Styles/")
        headStringIndex = headStringIndex.replace("<li><a href=\"index.html\">Introduction</a></li>", "<li><a href=\"../html/index.html\">Introduction</a></li>")
        headStringIndex = headStringIndex.replace("@@@_EXTERNAL_TOOLS_REPORT_@@@", "") #remove placeholder
        headStringIndex = headStringIndex.replace("Rule Scripts", "coverage modules")
        
        fileIndex.write(headStringIndex)
        ruleLinksStringIndex = ""
        for link in covList:
            ruleLinksStringIndex = ruleLinksStringIndex + "<li><a href=\"" + link + "/index.html\">" + link[:-8] + "</a></li>\n"
        
        fileIndex.write(ruleLinksStringIndex)
        fileIndex.write(centerString)
        fileIndex.write("""
        <h1> About LCOV </h1>
        LCOV is a graphical front-end for GCC's coverage testing tool gcov. <br> 
        It collects gcov data for multiple source files and creates HTML pages containing the source code annotated with coverage information. <br>
        It also adds overview pages for easy navigation within the file structure. <br>
        LCOV supports statement, function and branch coverage measurement. <br>
        The generic workflow used is:<br>
        
        <ul><li><h2>Resetting counters</h2><br>
        lcov --zerocounters</li>
        <li><h2>Capturing the current coverage state to a file</h2><br>
        lcov --capture --output-file myMafModule.info</li>
        <li><h2>Getting HTML output</h2><br>
        genhtml myMafModule.info</li>
        </ul>
        """)
        fileIndex.write(tailString)
        fileIndex.close()

    
    except Exception, e:
        print "Problem when publishing LCOV coverage, error:" , e

    print "******* PUBLISH LCOV coverage *********"
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
    result = sp.Popen(["which", "lcov"], stdout=sp.PIPE).communicate()[0]
    found = result.find("lcov")
    if(found == -1):
        print "lcov does not exist -", sys.argv[0]
        print os.environ
        print os.getuid()
        exit()

    publishReport()
