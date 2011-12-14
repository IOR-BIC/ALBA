from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class FileNamingRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.DictionaryList = []
        self.DictionaryBaseClassList = []

    def execute(self):
        f = open("./Rules/FileNamingRules/" + self.ParameterList[0], 'r')
        lines = f.readlines()
        for line in lines:
            self.DictionaryList.append(line.replace("\n","").replace("\r",""))
            
        fBase = open("./Rules/FileNamingRules/" + self.ParameterList[1], 'r')
        linesBase = fBase.readlines()
        for lineBase in linesBase:
            self.DictionaryBaseClassList.append(lineBase.replace("\n","").replace("\r",""))

        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue

        
        if(self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref') == None \
           or len(self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref')) == 0):
          for prefix in self.DictionaryList:
            x = re.compile("^"+ prefix  +"[A-Z].*")
            cname = className
            if("::" in className) :
              cname = className[className.index("::")+2:]
            
            if(re.match(x, str(cname))):
              #print "OK " , cname
              return self.MarkedList
          #print "***NO without Base *** " , cname
          self.MarkedList.append("<item><class>" + str(className) + "</class></item>")
          return self.MarkedList
        
        result = False
        goodPrefix = ""
        for prefix in self.DictionaryList:
          x = re.compile("^"+ prefix  +"[A-Z].*")
          if(re.match(x, str(className))):
            result = True
            goodPrefix = prefix;
            break;
        if(result == False):
          #print "***NO 1 with base*** " , className
          self.MarkedList.append("<item><class>" + str(className) + "</class></item>")
          return self.MarkedList
        
        for skipName in self.DictionaryBaseClassList:
          if(skipName == str(className)):
            return self.MarkedList
		
        baseClassName = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref')[0].firstChild.nodeValue
        
        ## inheritance rule
        x2 = re.compile("^"+ baseClassName  +".*") #baseClassName or goodPrefix
        if(re.match(x2, str(className))):
            ##print "OK " , className , baseClassName
            return self.MarkedList
        
        ##print "***NO 2*** " , className , baseClassName              
        self.MarkedList.append("<item><class>" + str(className) + "</class></item>")
        return self.MarkedList
        