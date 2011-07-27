from xml.dom import minidom as xd
import re
from Rules.AbstractRule import AbstractRule
import os

class GuideCreatorRule(AbstractRule):
    def __init__(self):
        AbstractRule.__init__(self)

    def execute(self):
      commentTag = "//! "
      startTitleTag = "//! <title>"
      startDescriptionTag = "//! <description>"
      startSnippetTag = "//! <snippet>"
      endTitleTag = "//! </title>"
      endDescriptionTag = "//! </description>"
      endSnippetTag = "//! </snippet>"
      snippetsCounter = 0

      className = os.path.basename(self.FullPathInputFile)
      className = os.path.splitext(className)[0] #without extension

      x = re.compile(self.ParameterList[0])
      if(re.match(x, str(className))): #get only test classes
        file = open(self.FullPathInputFile)
        guideLines = "<item>\n"
        while True: #iterate on file lines
          line = file.readline()
          if len(line) == 0: break

          #Search for Title tag
          startTitle = line.find(startTitleTag)
          if (startTitle != -1):
            while True:
              if(line.find(endTitleTag) != -1): #if startTag and endTag are on the same line
                commentLine = line[len(startTitleTag):line.find(endTitleTag)]
                guideLines += line[startTitle:len(startTitleTag)]
                guideLines += self.replaceLine(commentLine)
                guideLines += line[line.find(endTitleTag):(line.find(endTitleTag)+len(endTitleTag))]
                guideLines += "\n"
                break
              guideLines += line.lstrip()
              line = file.readline()
              endTitle = line.find(endTitleTag)
              if (endTitle != -1):
                commentLine	= line[0:line.find(endTitleTag)].lstrip()
                guideLines += self.replaceLine(commentLine)
                guideLines += line[line.find(endTitleTag):(line.find(endTitleTag)+len(endTitleTag))]
                guideLines += "\n"
                break
              line = self.replaceLine(line)    
              if len(line) == 0: break #to avoid infinite loop

          #Search for Description tag
          startDescription = line.find(startDescriptionTag)
          if (startDescription != -1):
            while True:
              if(line.find(endDescriptionTag) != -1): #if startTag and endTag are on the same line
                commentLine = line[len(startDescriptionTag):line.find(endDescriptionTag)]
                guideLines += line[startDescription:len(startDescriptionTag)]
                guideLines += self.replaceLine(commentLine)
                guideLines += line[line.find(endDescriptionTag):(line.find(endDescriptionTag) + len(endDescriptionTag))]
                guideLines += "\n"
                break
              guideLines += line.lstrip()
              line = file.readline()
              endDescription = line.find(endDescriptionTag)
              if (endDescription != -1):
                commentLine	= line[0:line.find(endDescriptionTag)].lstrip()
                guideLines += self.replaceLine(commentLine)
                guideLines += line[line.find(endDescriptionTag):(line.find(endDescriptionTag)+len(endDescriptionTag))]
                guideLines += "\n"
                break
              line = self.replaceLine(line)    
              if len(line) == 0: break #to avoid infinite loop

          #Search for Snippet tag
          startSnippet = line.find(startSnippetTag)
          if (startSnippet != -1):
            if(snippetsCounter == 0):
               guideLines += "<snippets>\n"
            snippetsCounter = snippetsCounter+1   
            while True:
              if(line.find(endSnippetTag) != -1): #if startTag and endTag are on the same line
                commentLine = line[len(startSnippetTag):line.find(endSnippetTag)]
                guideLines += line[startSnippet:len(startSnippetTag)]
                guideLines += self.replaceLine(commentLine) #
                guideLines += line[line.find(endSnippetTag):(line.find(endSnippetTag) + len(endSnippetTag))]
                guideLines += "\n"
                break
              guideLines += line.lstrip()
              line = file.readline()
              endSnippet = line.find(endSnippetTag)
              if (endSnippet != -1):
                commentLine	= line[0:line.find(endSnippetTag)].lstrip()
                guideLines += self.replaceLine(commentLine) #
                guideLines += line[line.find(endSnippetTag):(line.find(endSnippetTag)+len(endSnippetTag))]
                guideLines += "\n"
                break
              line = self.replaceLine(line) #
              if len(line) == 0: break #to avoid infinite loop

        if(snippetsCounter != 0):
          guideLines += "</snippets>\n"
        file.close()
        guideLines = guideLines.strip()
        guideLines = guideLines.replace(commentTag,"")
        guideLines = guideLines.replace("//","")
        guideLines += "</item>"
        self.MarkedList.append(guideLines)
      return self.MarkedList

    def replaceLine(self, lineOld):
      lineNew = lineOld.replace("&","&amp;")
      lineNew = lineNew.replace(">","&gt;")
      lineNew = lineNew.replace("<","&lt;")
      return lineNew
      