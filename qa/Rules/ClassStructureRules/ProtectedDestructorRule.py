from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule

class ProtectedDestructorRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        foundDestructor = False
        if(className[:3] == "maf"):
            members = self.dom.getElementsByTagName('memberdef')
            for member in members:
                attrs = member.attributes
                if(attrs["kind"].value  == self.ParameterList[0]):
                    functionName = member.getElementsByTagName('name')[0].firstChild.nodeValue
                    tilde = "~"
                    if(tilde in functionName):
                        foundDestructor = True
                        if(attrs["prot"].value  != "protected"):
                            self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <function>" + attrs["prot"].value + "</function>\n"\
                                        + "</item>")
                            return self.MarkedList

        if(foundDestructor == False):
            self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <function> Default </function>\n"\
                                        + "</item>")

        return self.MarkedList

                  
