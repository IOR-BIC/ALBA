class AbstractRule:
    def __init__(self):
        self.FullPathInputFile = ""
        self.ParameterList = []
        self.MarkedList = []
        self.GlobalList = []

    def setFullPathInputFile(self, fullPathInputFile):
        self.FullPathInputFile = fullPathInputFile
        self.ValueList = []

    def setRuleParameters(self, *args):
        self.ParameterList = args

    def execute(self):
        pass