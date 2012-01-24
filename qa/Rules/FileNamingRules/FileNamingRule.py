from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class FileNamingRule(AbstractRule):
  def __init__(self):
    AbstractRule.__init__(self)
    self.DictionaryList = []
    self.DictionaryBaseClassList = []
    self.DictionaryExceptionBaseClassList = []

  def execute(self):
      
    self.dom = xd.parse(self.FullPathInputFile)
    
    f = open("./Rules/FileNamingRules/" + self.ParameterList[0], 'r')
    lines = f.readlines()
    for line in lines:
      self.DictionaryList.append(line.replace("\n","").replace("\r",""))
        
    fBase = open("./Rules/FileNamingRules/" + self.ParameterList[1], 'r')
    linesBase = fBase.readlines()
    for lineBase in linesBase:
      self.DictionaryBaseClassList.append(lineBase.replace("\n","").replace("\r",""))
        
    fBaseException = open("./Rules/FileNamingRules/" + self.ParameterList[2], 'r')
    linesBaseException = fBaseException.readlines()
    for lineBaseException in linesBaseException:
      self.DictionaryExceptionBaseClassList.append(lineBaseException.replace("\n","").replace("\r",""))
        
    
    hasBaseClassException = False
    if len(self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref')):
      nod1 = self.dom.getElementsByTagName('compounddef')[0]
      nod2 = nod1.getElementsByTagName('basecompoundref')[0]
      baseClassName = nod2.firstChild.nodeValue
      for baseClassException in self.DictionaryExceptionBaseClassList:
        if(baseClassException == str(baseClassName)):
          hasBaseClassException = True
          break

    className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
    
    if(self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref') == None \
      or len(self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref')) == 0 \
      or len(self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref')) > 1 or hasBaseClassException):
      #multi hierarchies, hierarchies shouldn't be apply - his base class is an excpetion and hierarchies should be apply
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
    
    
    goodPrefix = ""
    iStart = 1
    iEnd = -1
    for skipName in self.DictionaryBaseClassList:
      x2 = re.compile("^"+ skipName  +".*")
      if(skipName == str(className)):
        return self.MarkedList
      elif(re.match(x2, str(className)) and (iEnd-iStart)<(re.match(x2, str(className)).end()-re.match(x2, str(className)).start())):
        goodPrefix = skipName;
        
    ## inheritance rule
    if(goodPrefix == ""):
      x2 = re.compile("^"+ baseClassName  +".*")
      if(re.match(x2, str(className))):
        return self.MarkedList
    else:
      x2 = re.compile("^"+ goodPrefix  +".*")
      if(re.match(x2, str(className))):
          return self.MarkedList
    
    self.MarkedList.append("<item><class>" + str(className) + "</class></item>")
    return self.MarkedList
        