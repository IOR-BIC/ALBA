from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class PythonClassDocumentationRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        classDetailedDescription = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('detaileddescription')[-1]
        
        description = ""
        verbatim = ""
        
        l = classDetailedDescription.getElementsByTagName('para')
        if(len(l) == 0):
          self.MarkedList.append("<item><class>" + str(className.split("::")[-1]) + "</class></item>")
          return self.MarkedList
        
        verbatim = classDetailedDescription.getElementsByTagName('para')[0].getElementsByTagName('verbatim')
        if(verbatim):
          description=  verbatim[0].firstChild.nodeValue
          
        if(description == ""):
          self.MarkedList.append("<item><class>" + str(className.split("::")[-1]) + "</class></item>")
                    
        return self.MarkedList
                  
