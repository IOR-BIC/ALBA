from xml.dom import minidom as xd
import re, os, time
from Rules.AbstractRule import AbstractRule

class CoverageTestRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.TempFile = os.path.join(self.TempDir,"CoverageTestTemp.txt")

    def execute(self):
        f = None
        if not(os.path.exists(self.TempFile)):
            f = open(self.TempFile,"w")
          
            reg = "\Aclass.*\.xml";
            y = re.compile(reg);
          
            path = os.path.dirname(self.FullPathInputFile)
            dirList=os.listdir(path)
            for fname in dirList:
                if(re.match(y, str(fname)) == None):
                    pass
                else:
                    dom = xd.parse(os.path.join(path, fname))
                    
                    classCurrentName = None
                    inclusionFile = None
                    try:
                        classCurrentName = dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
                    except:
                        pass
                    
                    if( classCurrentName[-4:] == "Test" ):
                        f.write(classCurrentName+"\n")
                        
            f.close()
            
          
        f = open(self.TempFile,"r")
        lines = f.readlines()
        
        self.dom = xd.parse(self.FullPathInputFile)
        testClassName = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        
        path = os.path.dirname(self.FullPathInputFile)
        x = re.compile(self.ParameterList[0])
        if(re.match(x, str(testClassName))):
            pass
        else:
            #if the test is present do anything
            testIsPresent = False
            dirList=os.listdir(path)

            for line in lines:
                className = line.replace("\n","")
                if((className) == (testClassName + "Test")):
                    testIsPresent = True
                    break
                
            if(testIsPresent):
                pass
            else:
                #print str(className)
                self.MarkedList.append("<item><class>" + (str(testClassName)) + "</class></item>")
                
                
        f.close()
        return self.MarkedList
                  