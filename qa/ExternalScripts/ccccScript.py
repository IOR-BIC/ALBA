import os
import sys
import getopt

currentPathScript = os.path.split(os.path.realpath(__file__))[0]

try:
    sys.path.append(os.path.realpath(os.path.join(currentPathScript,"..","..")))
    from qa import mafPath
except ImportError:
    import mafPath


modulesDir = mafPath.mafSourcesDir
qaDir = mafPath.mafQADir

def usage():
    print "python ccccScript.py [-h] [-e ccccPath]"
    print "-h, --help                 show help (this)"
    #print "-e, --cccc-path=       cccc executable path"
    print 

def run(param):
    print "cccc working..."
    try:
        baseDir = modulesDir
        qaResultsDir = os.path.join(qaDir, "QAResults")
        CCCCDir = os.path.join(qaResultsDir, "externalcccc") #need to parametrize this?
        
        if(os.path.exists(qaResultsDir) == False):
          print "QA Results Dir doesn't exist %s" % qaResultsDir
          exit()

        if(os.path.exists(CCCCDir) == False):
          os.mkdir(CCCCDir);
        
        moduleDir = os.path.join(baseDir, param['module'])
        reportXML = os.path.join(CCCCDir, param['module'] + "cccc.xml")     
        cccc = param['ccccPath'] #find directly cppcheck absolute path
        command = cccc + " --xml_outfile=" + reportXML + " " + moduleDir + "/*.cpp"
        #print command
        os.system(command)

    except Exception, e:
        print "Error on launching executable, ", e
            
def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hm:e", ["help", "module", "cccc-path"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    param = {}
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            return
        #elif o in ("-e", "--cppcheck-path"):
            #param = {'cppcheckPath': a}
        elif o in ("-m", "--module="):
            param['module'] = a
        else:
            assert False, "unhandled option"

    import subprocess as sp
    result = sp.Popen(["which", "cccc"], stdout=sp.PIPE).communicate()[0]
    found = result.find("cccc")
    if(found == -1):
        print "cccc does not exist -", sys.argv[0]
        print os.environ
        print os.getuid()
        exit()
        
    param['ccccPath'] = result.replace("\n","") #remove final \n
    run(param)


if __name__ == "__main__":
  main()