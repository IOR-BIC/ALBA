from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule
import os

class LinesNumberRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.DictionaryList = []

    def execute(self):
      file = open(self.FullPathInputFile)
      nTLOC = 0 # number of Total Line Of Code
      
      for line in file:
        line = line.strip();
        if (line == ""):
            pass
        else:
            nTLOC = nTLOC + 1 # increment Total Lines Of Code    
      file.close()
      
      self.MarkedList.append("<item><file>"+(os.path.split(self.FullPathInputFile)[-1]) + "</file><linesNumber>" + str(nTLOC) + "</linesNumber>" + "</item>")
      return self.MarkedList
                  