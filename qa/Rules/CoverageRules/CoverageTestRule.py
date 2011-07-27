from xml.dom import minidom as xd
import re, os, time
from Rules.AbstractRule import AbstractRule

class CoverageTestRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        path = self.FullPathInputFile[0:self.FullPathInputFile.rfind("/")]
        x = re.compile(self.ParameterList[0])
        if(re.match(x, str(className))):
            pass
        else:
            #if the test is present do anything
            testIsPresent = False
            dirList=os.listdir(path)
            reg = "\Aclass.*\.xml";
            y = re.compile(reg);
            for fname in dirList:
                if(testIsPresent == True):
                    break;
                if(re.match(y, str(fname)) == None):
                    pass
                else:
                    #print className + " " + fname
                    dom = xd.parse(path + "/" + fname)
                    classTestName = dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
                    #remove namespace
                    classNameOrig = className[className.rfind(":")+1:]
                    
                    if(classTestName == (str(classNameOrig) + "Test")):
                        testIsPresent = True
                        #print str(classNameOrig), classTestName 
                        
            if(testIsPresent):
                pass
            else:
                #print str(className)
                self.MarkedList.append("<item><class>" + (str(className)) + "</class></item>")
                
        return self.MarkedList
                  