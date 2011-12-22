from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class ClassMethodsDocumentationRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        
        members = self.dom.getElementsByTagName('memberdef')
        for member in members:
          attrs = member.attributes
          if(attrs["kind"].value  == self.ParameterList[0]):
            functionName = member.getElementsByTagName('name')[0].firstChild.nodeValue
            detailedDescription = member.getElementsByTagName('detaileddescription')[0]
            para = detailedDescription.getElementsByTagName('para')
            if(para == []):
              #self.MarkedList.append((str(className))+"-> "+functionName+ "<-")
              self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <function>" + functionName.replace("<","&lt;").replace(">","&gt;") + "</function>\n"\
                                        + "</item>")

        
        return self.MarkedList
                  