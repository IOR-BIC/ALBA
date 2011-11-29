from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class FunctionNamingRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
    
        members = self.dom.getElementsByTagName('memberdef')
        for member in members:
          attrs = member.attributes
          if(attrs["kind"].value  == self.ParameterList[0]):
            type = None
            for memberChild in member.childNodes:
              if(memberChild.nodeName == "type" and memberChild.firstChild):
                type = str(memberChild.firstChild.nodeValue)
              if(memberChild.nodeName == "name"):
                x = re.compile(self.ParameterList[1])
                if(type != None and re.match(x, str(memberChild.firstChild.nodeValue))):
                  #print className, memberChild.firstChild.nodeValue
                  if(memberChild.firstChild.nodeValue[:8] != "operator"):
                    line = member.getElementsByTagName('location')[0].attributes["line"].value
                    #self.MarkedList.append((str(className))+"::"+memberChild.firstChild.nodeValue+ " " + line)
                    self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <function>" +memberChild.firstChild.nodeValue + "</function>\n"\
                                        + "  <line>" + line + "</line>\n"\
                                        + "</item>")
     
        
        return self.MarkedList
                  