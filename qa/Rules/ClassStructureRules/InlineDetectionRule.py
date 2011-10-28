from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class InlineDetectionRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        
        members = self.dom.getElementsByTagName('memberdef')
        for member in members:
          attrs = member.attributes
          if(attrs["kind"].value  == self.ParameterList[0] and attrs[self.ParameterList[1]].value  == "yes"):
            functionName = member.getElementsByTagName('name')[0].firstChild.nodeValue
            #self.MarkedList.append((str(className))+"-> "+functionName+ "<-")
            self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <function>" + functionName.replace('<','&lt;').replace('>','&gt;') + "</function>\n"\
                                        + "</item>")

        
        return self.MarkedList
                  