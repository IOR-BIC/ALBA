import os
try:
    from qa import mafPath
except ImportError:
    import mafPath

currentPathScript = os.path.split(os.path.realpath(__file__))[0]

#read all files and directories , for every rule directory, generate a python file
allRules = []

def CheckRulesConfiguration(arg, directory, files):
    for file in files:
      name, ext = os.path.splitext(file)
      if(ext == ".ini"):
          found = False
          for item in allRules:
            if(item[0] == arg):
              item[1].append(name)
              found = True
          if(found == False):
            allRules.append([arg,[name]])


def CheckRulesDirectories(arg, directory, files):
    if(arg == directory): return
    if os.path.isdir(directory) and not os.path.islink(directory):
      rule = os.path.split(directory)[-1]
      os.path.walk(directory, CheckRulesConfiguration, rule)

def GeneratePythonQAScripts():
    testList = open("TestListQA.txt", 'w')
    for ruleGroup, rules in allRules:
      f = open(ruleGroup + "QA.py", 'w')
      f.write("import os\n")
      f.write("import sys\n")
      f.write("import re\n")
      f.write("import math\n")
      f.write("import Executer\n")
      
      for rule in rules:
        f.write("from Rules." + ruleGroup + " import " + rule + "\n")

	  #costant
      f.write("\n")
      f.write("class " + ruleGroup + "QA():" + "\n")

      testList.write(ruleGroup + "QA.py" + "\n")

      f.write("    " + "def __init__(self):" + "\n")
      f.write("    " + "    self.FileOutput = None" + "\n")
      f.write("    " + "    self.OutputDirectory = None" + "\n")
      f.write("    " + "    self.Classes = []" + "\n")
      f.write("    " + "    self.NumberOfErroneusClasses = 0" + "\n")
      f.write("    " + "    self.CurrentRule = \"\"" + "\n")
      f.write("    " + "    self.FilePattern = \"\"" + "\n")

      f.write("\n")
      f.write("    " + "def SetFilePattern(self, filePattern):" + "\n")
      f.write("    " + "    self.FilePattern = filePattern" + "\n")

      f.write("\n")
      f.write("    " + "def CheckClassFile(self, dirPath, file, rule):" + "\n")
      f.write("    " + "    x = re.compile(self.FilePattern) " + "\n")
      f.write("    " + "    if(re.match(x, str(file))):" + "\n")
      f.write("    " + "      executer = Executer.Executer()" + "\n")
      f.write("    " + "      executer.setRule(rule)" + "\n")
      f.write("    " + "      executer.setFileNameInput(str(file))" + "\n")
      f.write("    " + "      executer.setFileNameOutput(self.CurrentRule + \".xml\")" + "\n")
      f.write("    " + "      executer.setDirectoryNameInput(str(dirPath))" + "\n")
      f.write("    " + "      executer.setOutputDirectory(self.OutputDirectory)" + "\n")
      f.write("    " + "      self.NumberOfErroneusClasses = self.NumberOfErroneusClasses + executer.parse()" + "\n")
      f.write("    " + "      return file" + "\n")
      f.write("    " + "    else:" + "\n")
      f.write("    " + "      return None" + "\n")

	  #for
      for rule in rules:
        f.write("\n")
        f.write("    " + "def " + rule + "(self, dirPath, file):" + "\n")
        f.write("    " + "    rule = " + rule + "." + rule + "()"  + "\n")
        
        inif = open("./Rules/" + ruleGroup + "/" + rule + ".ini", 'r')
        lines = inif.readlines()
        inif.close()
        parameters = []
        for line in lines:
          if(line[-1] == "\n"):
            parameters.append(line[10:-1]) # remove "parameter="
          else:
            parameters.append(line[10:]) # remove "parameter="
        parameterString = ",".join(parameters)
        f.write("    " + "    rule.setRuleParameters(" + parameterString + ")" + "\n")

        f.write("    " + "    xmlFile = self.CheckClassFile(dirPath, file, rule)" + "\n")
        f.write("    " + "    if(xmlFile != None):" + "\n")
        f.write("    " + "      self.Classes.append(xmlFile)" + "\n")
      
      ruleIni = os.path.join(mafPath.mafQADir, "Rules", ruleGroup + "FilePattern.ini")
      iniR = open(ruleIni , 'r')
      lines = iniR.readlines()
      iniR.close()
      
      parameters = []
      for line in lines:
        if(line[-1] == "\n"):
          parameters.append(line[10:-1]) # remove "parameter="
        else:
          parameters.append(line[10:]) # remove "parameter="
      #parameterString = ",".join(parameters)
      
	  #constant
      f.write("\n")
      f.write("    " + "def CheckSources(self,baseDirectory, outputDirectoryCheck, ruleMethod):" + "\n")
      f.write("    " + "    self.Classes = []" + "\n")
      f.write("    " + "    self.CurrentRule = ruleMethod" + "\n")
      f.write("    " + "    self.NumberOfErroneusClasses = 0" + "\n")
      f.write("    " + "    self.OutputDirectory = outputDirectoryCheck" + "\n")
      f.write("    " + "    try:" + "\n")
      f.write("    " + "      os.remove(outputDirectoryCheck + self.CurrentRule +\".xml\")" + "\n")
      f.write("    " + "    except:" + "\n")
      f.write("    " + "      print \"No file to remove\"" + "\n")
      f.write("    " + "    self.FileOutput = open(outputDirectoryCheck + self.CurrentRule +\".xml\",'a')" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<?xml version=\\\"1.0\\\"?>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<root>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<ruleName>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(self.CurrentRule + \"\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"</ruleName>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<ruleDescription>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(" + str(parameters[2]).replace("\r", "").replace("\n", "") + " + \"" + "\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"</ruleDescription>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<results>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.close()" + "\n")

      f.write("    " + "    for dirpath,dirnames,filenames in os.walk(baseDirectory):" + "\n")
      f.write("    " + "      for file in filenames:" + "\n")
      f.write("    " + "        getattr(self, self.CurrentRule)(dirpath, file)" + "\n")
      
      f.write("\n")
      f.write("    " + "    self.FileOutput = open(outputDirectoryCheck + self.CurrentRule +\".xml\",'a')" + "\n")
      f.write("    " + "    percentage = int(float(len(self.Classes) - self.NumberOfErroneusClasses)/float(len(self.Classes)) * 100)" + "\n")
      f.write("    " + "    percentageCoverage = int(float((len(self.Classes)- self.NumberOfErroneusClasses)/2)/(float((len(self.Classes)+ self.NumberOfErroneusClasses)/2)) * 100)" + "\n")
      f.write("    " + "    # result XML tag open" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<percentage>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"\\n\" + str(len(self.Classes)- self.NumberOfErroneusClasses) + \" on \" + str(len(self.Classes)) + \" percentage: \" + str(percentage) + \"%\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"\\n</percentage>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"<percentageCoverage>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"\\n\" + str((len(self.Classes)- self.NumberOfErroneusClasses)/2) + \" on \" + str((len(self.Classes)+ self.NumberOfErroneusClasses)/2) + \" percentage: \" + str(percentageCoverage) + \"%\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"\\n</percentageCoverage>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"</results>\\n\")" + "\n")
      f.write("    " + "    self.FileOutput.write(\"</root>\\n\")" + "\n")
      f.write("    " + "    # result XML tag close" + "\n")
      f.write("    " + "    self.FileOutput.close()" + "\n")
      
      f.write("\n")      
      f.write("if __name__ == '__main__':" + "\n")
      f.write("    if len(sys.argv) != 3:" + "\n")
      f.write("        print \"need argv[1](directory to check), argv[2](directory in which generate scripts) \""  + "\n")
      f.write("    else:"  + "\n")
      f.write("        nc = " + ruleGroup + "QA()"  + "\n")

      f.write("        nc.SetFilePattern(" + str(parameters[0]).replace("\r", "").replace("\n", "") + ")"  + "\n")
      for rule in rules:
        f.write("        nc.CheckSources(" + "sys.argv[1],  sys.argv[2]," + "\"" + rule +"\")"  + "\n")

      f.close()

    testList.close()

if __name__ == '__main__':
  scriptsDir = os.getcwd()
  os.path.walk(scriptsDir + "/Rules", CheckRulesDirectories, scriptsDir + "/Rules")
  GeneratePythonQAScripts()
  print "GENERATION SUCCESSFUL"
#in each directory check .ini, for every ini generate rule
