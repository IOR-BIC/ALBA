from xml.dom import minidom as xd
import re, os, time
from Rules.AbstractRule import AbstractRule

class OneClassPerFileDeclarationRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        classCurrentName = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        #print classCurrentName
        try:
            inclusionFile = self.dom.getElementsByTagName('includes')[0].firstChild.nodeValue
        except:
            print "***********  Include file is not present **************"
            return self.MarkedList
        
        path = self.FullPathInputFile[0:self.FullPathInputFile.rfind("/")]
        
        dirList=os.listdir(path)
        reg = "\Aclass.*\.xml";
        y = re.compile(reg);
        sameInclusionFileClasses = []
        for fname in dirList:
            if(re.match(y, str(fname)) == None):
                pass
            else:
                #print className + " " + fname
                dom = xd.parse(path + "/" + fname)
                #print fname
                className = dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
                if(className == classCurrentName):
                    continue
                
                #remove namespace
                className = className[className.rfind(":")+1:]
                
                try:
                    inclusionFileToCheck = dom.getElementsByTagName('includes')[0].firstChild.nodeValue
                    if(inclusionFileToCheck == inclusionFile):
                       if(className != inclusionFileToCheck[:-2]):
                           sameInclusionFileClasses.append(className);
                except:
                    print "***********  Include file is not present **************"
        
        
            
        if(len(sameInclusionFileClasses) > 0) :
            self.MarkedList.append("<item>\n"\
                                   + "  <class>" +  str(classCurrentName) + "</class>\n"\
                                   + "  <otherClasses>" + " ".join(sameInclusionFileClasses) + "</otherClasses>\n"\
                                   + " <includeFile>" + inclusionFile + "</includeFile>" \
                                   + "</item>")
        
        return self.MarkedList
                  