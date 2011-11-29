import os
import sys
import getopt

try:
    from qa import mafPath
except ImportError:
    import mafPath

currentPathScript = os.path.split(os.path.realpath(__file__))[0]

extensionToAvoid = [".dll",".prl", ".a",".lib", ".dylib",".so", ".0", ".1"] # necessary to insert .N because of symbolic link of linux
currentPathScript = os.path.split(os.path.realpath(__file__))[0]
modulesDir = mafPath.mafSourcesDir #need to be changed
param = {}

def find_executable(executable, path=None):
    fullPathName = os.path.join(path, executable)
    if(os.access(fullPathName, os.X_OK)):
        ext = os.path.splitext(fullPathName)[-1]
        if(ext in  extensionToAvoid):
            return None
        else:
            return fullPathName
            
def qtestValidate(fileName):
    from TestsValidator.QTestsValidator import QTestsValidator
    val = QTestsValidator.QTestsValidator()
    val.setFullPathInputFile(fileName)
    val.validate()
    
def cppunitValidate(fileName):
    print "cppunit already not implemented"

def errhandler():
   print "Unrecognized compiler"
  
def validate(fileName):
    takeValidator = {
    "QTest": qtestValidate,
    "cppunit": cppunitValidate,
    }
    
    try:
        takeValidator.get(param['test-suite'],errhandler)(fileName)
    except Exception, e:
        print "Test suite not present" , e 

def execute():
    scriptsDir = currentPathScript
    dirList = [ name for name in os.listdir(modulesDir) if (name[0:3] == "maf" and os.path.isdir(os.path.join(modulesDir, name))) ] 
    print modulesDir, dirList
    #execDir = modulesDir + "/../Install/bin/Debug"
    execDir = param['directory']
    os.chdir(execDir)
    #some system requirements

    suffix = "Test"
    if(str(os.sys.platform).lower() == 'linux2'):
        #os.system('export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH')
        try:
            os.environ['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH'] + ":" + execDir
        except KeyError:
            os.environ['LD_LIBRARY_PATH'] = execDir
            
        suffix = "Test"
        #need to start X
        os.environ['DISPLAY'] = "localhost:0.0"
        os.system("Xvfb :0.0 &")
    elif(str(os.sys.platform).lower() == 'darwin'):
        #os.system('export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH')
        try:
            os.environ['DYLD_LIBRARY_PATH'] = os.environ['DYLD_LIBRARY_PATH'] + ":" + execDir
        except KeyError:
            os.environ['DYLD_LIBRARY_PATH'] = execDir
        
        suffix = "Test"
    elif(str(os.sys.platform).lower() == 'win32'):
        suffix = "Test"
        
    if(param['release-enable'] == True):
        suffix = "Test" 
        
    for dir in dirList:
        executable = dir  + suffix
        if(os.path.exists(executable) or os.path.exists(executable + ".exe")):
            #print os.getcwd() + "/" + executable
            absPath = os.path.join(execDir, executable)
            logResult = absPath + "LOG.txt";
            if(os.path.exists(logResult)):
                os.remove(logResult)
            #create file, and append results in it
            if(str(os.sys.platform).lower() == 'win32'):
                os.system(absPath + " > " + logResult + " 2>&1 ")
            else:
                os.system(absPath + " &> " + logResult)
            name_key = "test-suite"
            if name_key in param:
                validate(logResult)
        else:
            print "Executable %s Not Present in %s" % (executable , execDir)
    os.chdir(scriptsDir)

def usage():
    print "python BinaryLauncher.py -d directory -t test-suite"
    print "-h, --help                 show help (this)"
    print "-d, --directory=           select path in which find executables"
    print "-t, --test-suite=          choose the test suite from supported ones: QTest , cppunit"
    print "-R, --release-enable       this flag activate the release execution"

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hd:t:R", ["help", "directory=", "test-suite=", "release-enable"] )
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
        
    param['release-enable'] = False
    
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-d", "--directory"):
            param['directory'] = os.path.abspath(os.path.normpath(a))
        elif o in ("-t", "--test-suite"):
            param['test-suite'] = a
        elif o in ("-R", "--release-enable"):
            param['release-enable'] = True
        else:
            assert False, "unhandled option"
    
    if(len(param) == 0):
        usage()
        #print currentPathScript
        return
    
    execute()
    
    
if __name__ == '__main__':
    main()
