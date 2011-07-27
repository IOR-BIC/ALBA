class AbstractTestsValidator:
    def __init__(self):
        self.FullPathInputFile = ""
        self.TestList = []
        self.PassedTestList = []
        self.FailedTestList = []

    def setFullPathInputFile(self, fullPathInputFile):
        self.FullPathInputFile = fullPathInputFile

    def validate(self):
        pass