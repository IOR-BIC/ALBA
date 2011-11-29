from xml.dom import minidom as xd
import re, os, time
from Rules.AbstractRule import AbstractRule

class OneClassPerFileDeclarationRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.TempFile = os.path.join(self.TempDir,"OneClassPerFileDeclarationTemp.txt")
        
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
                        inclusionFile = dom.getElementsByTagName('includes')[0].firstChild.nodeValue
                        
                    except:
                        pass
                    if(classCurrentName and inclusionFile):
                        f.write(classCurrentName+"#"+inclusionFile+"\n")
                        
            f.close()
            
          
        f = open(self.TempFile,"r")
        lines = f.readlines()
        
        
        self.dom = xd.parse(self.FullPathInputFile)
        classCurrentName = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        try:
            inclusionFile = self.dom.getElementsByTagName('includes')[0].firstChild.nodeValue
        except:
            return self.MarkedList
        
        sameInclusionFileClasses = []
        for line in lines:
            couple = line.split("#")
            
            classNameToCheck = couple[0].replace("\n","")
            inclusionFileToCheck = couple[1].replace("\n","")
            
            #print classNameToCheck + " # " + inclusionFileToCheck
            
            if(classNameToCheck != classCurrentName):
                if(inclusionFileToCheck == inclusionFile):
                    sameInclusionFileClasses.append(classNameToCheck)
                else:
                    print "INCLUSIONI DIVERSE : " +inclusionFileToCheck + " # " +inclusionFile
        
        if(len(sameInclusionFileClasses) > 0) :
            self.MarkedList.append("<item>\n"\
                                   + "  <class>" +  str(classCurrentName) + "</class>\n"\
                                   + "  <otherClasses>" + " ".join(sameInclusionFileClasses) + "</otherClasses>\n"\
                                   + " <includeFile>" + inclusionFile + "</includeFile>" \
                                   + "</item>")
        
        f.close()
        
        return self.MarkedList
                  