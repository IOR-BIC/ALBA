# CommentsQA:
# Static implementation

import os
import sys
import time

if(len(sys.argv) != 2):
  print "Usage: findCandidatesClasses.py ALBA | MED"
  sys.exit(-1)

framework = sys.argv[1]

# print "Analizing framework " + framework + ":"

classDir = sys.path[0] + "\.."
resultsDir = sys.path[0] 

# if(framework == "ALBA"):
  # classDir = classDir + "\ALBA"
# elif(framework == "MED"):
  # classDir = classDir + "\Medical"
# else:
  # print "Usage: findCandidatesClasses.py ALBA | MED"
  # sys.exit(-1)

nTLOC = 0 # Source Line Of Code
nCLOC = 0 # Comment Line Of Code
good = 0
bad = 0
cGood = 0
cBad = 0
isInFunction = False
isInComment = False
isInLineComment = False

asc_time = time.strftime("%Y.%m.%d",time.localtime(time.time()))
resultFile = open(resultsDir + "/comments_" + framework + ".txt","w+")

for dirPath,dirName,fileName in os.walk(classDir):

  dirName = os.path.basename(dirPath)
  if((dirName != "Base" and dirName != "Core" and dirName != "Gui" and dirName != "Interaction" and dirName != "Operations" and dirName != "VME" and dirName != "DataPipe" and dirName != "vtkALBA" and dirName != "vtkMED" and (dirName != "Common" or framework == "ALBA") and dirName != "Views" and dirName != "DataPipes") or dirPath.find("Testing") != -1):
    continue
  filesNames = os.listdir(dirPath)
  resultFile.write("\n" + os.path.basename(dirPath) + "\n")
  print os.path.basename(dirPath)
  cGood = 0
  cBad = 0
  for fileName in filesNames:
    if ((os.path.splitext(fileName)[-1] == ".cpp") or (os.path.splitext(fileName)[-1] == ".txx") or (os.path.splitext(fileName)[-1] == ".cxx")): # file is a cpp file
      cppFile = open(dirPath + "/" + fileName)
      nTLOC = 0
      nCLOC = 0
      counter = 0
      braketsCounter = 0
      isInFunction = False
      isInComment = False
      isInlineComment = False
      for line in cppFile:
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
            #resultFile.write(line + " " + "\n")
            nCLOC = nCLOC + 1 # increment Commented Lines Of Code
          #else:
          #  if (len(line)>0): # non empty lines of code
          #    resultFile.write(line + " " + "\n")
          if (len(line)>0): # non empty lines of code
            nTLOC = nTLOC + 1 # increment Total Lines Of Code
      cppFile.close()
      if (nTLOC != 0):
        perc = float(float(nCLOC)/float(nTLOC)) * 100
      else:
	    perc = 0
      if ((perc >= 10) and (perc <= 40)):
        result = "GOOD"
        good = good + 1
        cGood = cGood + 1
      elif ((perc < 10) and (perc > 7)):
        result = "CANDIDATE"
        bad = bad + 1
        cBad = cBad + 1
      else:
        result = "BAD"
        bad = bad + 1
        cBad = cBad + 1
      resultFile.write(fileName + ("-" *  (50 - len(fileName))) + result + ("-" *  (15 - len(result))) + "%.2f" % perc + "%" +"\n")
  if(cBad + cGood != 0):
    percc = float(float(cGood)/float(cGood+cBad)) * 100
  else:
    percc = 0
  resultFile.write("good:%.d" % cGood + " (%.2f " % percc + "%)" + " - bad: %.d" % cBad +"\n")
if (bad + good != 0):
  percct = float(float(good)/float(good+bad)) * 100
else:
  percct = 0
resultFile.write("\ngood:%.d" % good + " (%.2f " % percct + "%)" +  " - bad: %.d" % bad +"\n")
resultFile.close()