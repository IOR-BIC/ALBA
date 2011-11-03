from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class VariableNamingRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
    
        members = self.dom.getElementsByTagName('memberdef')
        for member in members:
          attrs = member.attributes
          if(attrs["kind"].value  == self.ParameterList[0]):
            for memberChild in member.childNodes:
              if(memberChild.nodeName == "type"):
                if(memberChild.firstChild == None):
                  break
              if(memberChild.nodeName == "name"):
                x = re.compile(self.ParameterList[1]) 
                if(re.match(x, str(memberChild.firstChild.nodeValue))):
                  line = member.getElementsByTagName('location')[0].attributes["line"].value
                  self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <variable>" +memberChild.firstChild.nodeValue + "</variable>\n"\
                                        + "  <line>" + line + "</line>\n"\
                                        + "</item>")
        
        return self.MarkedList