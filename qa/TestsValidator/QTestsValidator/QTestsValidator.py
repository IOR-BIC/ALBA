from TestsValidator.AbstractTestsValidator import AbstractTestsValidator

class QTestsValidator(AbstractTestsValidator):
    def __init__(self):
        AbstractTestsValidator.__init__(self)
        
    def validate(self):
        fLog = open(self.FullPathInputFile, 'r');
        #print fLog.read(-1) #print all the file
        
        testName = ""
        position = fLog.seek(0, 0); # position at the beginning
        
        isStarted = False
        isFinished = False
        
        for line in fLog.readlines():
            line = line.replace("\n", "").replace("\r", "")
            items = line.split(" ")
            startTest = "Start"
            endTest = "Finished"
            init = "*********"
            term = init
            if(items[0] == init and items[-1] == term):
                if(items[1] == startTest):
                    if(isStarted == True and isFinished == False):
                        print "################### error: %s has not finished. ###################" % testName
                    isStarted = True
                    isFinished = False
                    testName = items[-2]    
                elif(items[1] == endTest):
                    isFinished = True
                    isStarted = False
                else:
                    continue
            print line
        
        fLog.close()
