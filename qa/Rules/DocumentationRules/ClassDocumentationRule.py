from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class ClassDocumentationRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        classBriefDescription = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('detaileddescription')[-1]
        
        description = ""
        para = classBriefDescription.getElementsByTagName('para')
        if(para):
          description=  para[0].firstChild.nodeValue
        
        if(description == ""):
          self.MarkedList.append("<item><class>" + str(className) + "</class></item>")
        
        return self.MarkedList
                  
