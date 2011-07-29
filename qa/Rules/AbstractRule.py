import os

class AbstractRule:
    def __init__(self):
        self.FullPathInputFile = ""
        self.ParameterList = []
        self.MarkedList = []
        self.GlobalList = []
        self.TempDir = os.path.join(os.path.split(os.path.realpath(__file__))[0], "..", "Temp")
        self.TempFile = ""

    def setFullPathInputFile(self, fullPathInputFile):
        self.FullPathInputFile = fullPathInputFile
        self.ValueList = []

    def setRuleParameters(self, *args):
        self.ParameterList = args

    def execute(self):
        pass