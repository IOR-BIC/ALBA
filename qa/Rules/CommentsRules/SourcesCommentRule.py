from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule
import os

class SourcesCommentRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)
        self.DictionaryList = []

    def execute(self):
      braketsCounter = 0
      isInComment = False
      IsInlineComment = False;
      file = open(self.FullPathInputFile)
      nTLOC = 0 # number of Total Line Of Code
      nCLOC = 0 # number of Commented Line Of Code
      pCLOC = 0 # percentage of Commented Line Of Code on Total Line Of Code
      for line in file:
        if (line.find("{") != -1):
          braketsCounter = braketsCounter + 1
        if (line.find("}") != -1):
          braketsCounter = braketsCounter - 1
        if (braketsCounter > 0): # if is reading a line that is inside a function
          line = line.strip() # remove empty spaces on string bounds
          if (line.find("*/") != -1):
            isInComment = False
          if ((line.find("/*") != -1) and (line.find("*/") == -1) and line.find("%s/*") == -1): # start a comment with "/*" and without "*/"
            isInComment = True
            line = line[line.find("/*") + 2:len(line)] # get only the commented portion without "/*"
            line = line.strip() # remove empty spaces on string bounds
          isInlineComment = False
          if ((line.find("//") != -1) or ((line.find("/*") != -1 and line.find("*/") != -1))): # is a comment starting with "//" or with both "/*" and "*/"
            isInlineComment = True
            line = line[line.find("//") + 2:len(line)] # get only the commented portion without "//"
            line = line.strip() # remove empty spaces on string bounds
          if (((isInComment == True) or (isInlineComment == True)) and (line.find(";") == -1) and (len(line) > 0) and (line != "{") and (line != "}")): # non empty commented line of non executable code (?)
            #resultFile.write(line + " " + str(isInComment) + " " + str(isInlineComment) + "\n")
            nCLOC = nCLOC + 1 # increment Commented Lines Of Code
          if (len(line)>0): # non empty lines of code
            nTLOC = nTLOC + 1 # increment Total Lines Of Code
      file.close()
      if (nTLOC != 0):
        pCLOC = int(float(float(nCLOC)/float(nTLOC)) * 100)
      else:
	    pCLOC = 0
      if ((pCLOC < 20) or (pCLOC > 40)):
        self.MarkedList.append("<item><class>"+(os.path.split(self.FullPathInputFile)[-1]) + "</class><commentPercentage>" + str(pCLOC) + "%</commentPercentage>" + "</item>")
      return self.MarkedList
                  