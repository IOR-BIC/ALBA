from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule
import os, time

class UselessIncludesRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.DictionaryList = []

    def execute(self):
        self.dom = xd.parse(self.FullPathInputFile)
        className = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('compoundname')[0].firstChild.nodeValue
        baseClassName = ""
        try:
            baseClassName = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('basecompoundref')[0].firstChild.nodeValue
            try:
                baseClassName = baseClassName[baseClassName.index("::")+2:]
            except:
                pass
        except:
            pass
        refInclude = ""    
        try:    
            refInclude = self.dom.getElementsByTagName('compounddef')[0].getElementsByTagName('includes')[0].attributes["refid"].value
        except:
            pass
            
        f = open("./Rules/ClassStructureRules/" + self.ParameterList[0], 'r')
        lines = f.readlines()
        for line in lines:
            self.DictionaryList.append(line.replace("\n","").replace("\r",""))
        
        directory =  os.path.dirname(self.FullPathInputFile)
        
        classToIcludeList = []
        if(refInclude != ""):
            xmlFile = os.path.join(directory, refInclude + ".xml")
            extDom = xd.parse(xmlFile)
            includeList = extDom.getElementsByTagName('compounddef')[0].getElementsByTagName('includes')
            classToIcludeList = [i.firstChild.nodeValue.split('.')[0] for i in includeList]
        
        #print len(classToIcludeList)
        dict = {}
        for item in classToIcludeList:
            dict[item] = False
            #print className, item
            if(item == baseClassName or (item + ".h") in self.DictionaryList):
                dict[item] = True
                #print item , self.DictionaryList
                continue
                
            members = self.dom.getElementsByTagName('memberdef')
            for member in members:
              attrs = member.attributes
              if(attrs["kind"].value  == "variable"):
                #check type
                typeVariable = None
                try:
                  typeVariable = member.getElementsByTagName('definition')[0].firstChild.nodeValue
                except:
                    pass
                #check
                if(typeVariable != None and item in typeVariable and not((item + "*") in typeVariable) and not((item + " *") in typeVariable)):
                  #include correct! exit from for loop
                  dict[item] = True
                  continue
                else:
                  pass
              elif(attrs.has_key("inline") and attrs["inline"].value  == "yes"):
                returnValue = ""
                params = ""
                try:
                    definition = member.getElementsByTagName('definition')[0].firstChild.nodeValue
                except:
                    pass
                try:
                    params = member.getElementsByTagName('argsstring')[0].firstChild.nodeValue
                except:
                    pass                  

                if(definition != None and params != None and item in definition or item in params):
                  dict[item] = True
                  continue
                else:
                  pass
        # search all the member which are not pointers
        # search inline functions in which there is the class
        
        for i in dict.keys():
            if(dict[i] == False):
                self.MarkedList.append("<item>\n"\
                                        + "  <class>" +  str(className) + "</class>\n"\
                                        + "  <include>" + i + ".h</include>\n"\
                                        + "</item>")
        return self.MarkedList
                  