from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule
import os

class GuiCommentsRules(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.DictionaryList = []
        
    def CheckFile(self,filename,match,num,minNumElem):
      r = open(filename, 'r')

      lines = r.readlines()
      lineNumber = 0
      for line in lines:
        lineNumber = lineNumber + 1
        x = re.compile(match)
        if(re.search(x, line)):

          founded =  re.findall('".+"\b*,|_\(".+"\b*\),|".+"\b*\);',re.split(x, line)[1])#sUBDIVIDE ELEMENTS OF THE GUI METHOD
          for f in founded:
            line = line.replace(re.findall('".*"',f)[0],re.findall('".*"',f)[0].replace(',',' '))#REMOVE ','

          if (len(re.split(',',re.split(x, line)[1])) != num and len(re.split(',',re.split(x, line)[1])) >= minNumElem):#CHECK IF THE GUI METHOD HAS A NUMBER OF ELEMENTS SUFFICIENT FOR TOOLTIP
            # print filename + ' : ' + line
            self.MarkedList.append("<item><class>" + (os.path.split(self.FullPathInputFile)[-1]) + "</class><line>" + str(lineNumber) + "</line>" + "</item>")          

      r.close()

    def execute(self):
      f = open("./Rules/GuiCommentsRules/" + self.ParameterList[0], 'r')
      lines = f.readlines()
      for line in lines:
        self.CheckFile( self.FullPathInputFile, line.split()[0], int(line.split()[1]), int(line.split()[2]) )
            
      return self.MarkedList
                  