class Executer:
    """Executer executes and handlse the result of a rule on a file.

    It returns 1 if the rules finds an error, 0 otherwise.
    Append the report to the output file. 
    """
    def __init__(self):
        """initialize the object.   
        """
        self.dom = None
        self.FileInput = None
        self.DirectoryInput = None
        self.ClassFileOutput = None
        self.OutputDirectory = "./"
        self.ResultFile = ""
        self.Rule = None

    def setOutputDirectory(self, directory):
        self.OutputDirectory = directory

    def setFileNameOutput(self, filename):
        self.ClassFileOutput = filename

    def setFileNameInput(self, filename):
        self.FileInput = filename

    def setDirectoryNameInput(self, directory):
        self.DirectoryInput = directory

    def setRule(self, rule):
        self.Rule = rule

    def parse(self):
        output = open(self.OutputDirectory + self.ClassFileOutput,"a")
        self.Rule.setFullPathInputFile(self.DirectoryInput + "/" + self.FileInput)
        writeList = self.Rule.execute()
        
        if (writeList):
            output.write("\n".join(writeList) + "\n")
            output.close()
            return 1
        else:
            output.close()
            return 0