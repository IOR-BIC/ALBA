from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule
import os

class LicenseHeaderRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.DictionaryList = []

    def execute(self):
      file = open(self.FullPathInputFile)
      
      """if this line is present the the header is present"""
      license = self.ParameterList[0]
      
      count = -1
      licenseFound = False;
      headerFound = True;
      for line in file:
        count = count + 1
        if(count == 10):
            break #exit the cycle

        if (len(line) > 1 and line[1] == "*"):
            #search if license is present
            if(count == 7):
                if(license in line):
                    licenseFound = True
            pass
        else:
            headerFound = False
            break
                  
      file.close()
      
      if(headerFound == True and licenseFound == True):
          return self.MarkedList
      
      self.MarkedList.append("<item><file>"+(os.path.split(self.FullPathInputFile)[-1]) + "</file></item>")
      return self.MarkedList
                  