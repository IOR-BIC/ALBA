import os
import shutil
from datetime import datetime
import sys

try:
    from qa import medPath
except ImportError:
    import medPath

scriptsDir = os.getcwd() #original directory
os.chdir(medPath.mafQADir)
baseDir = os.getcwd()

currentResultDir = os.path.join(baseDir,"QAResults")

baseArchiveDir = os.path.join(baseDir,"QAArchive")
archiveDir = baseArchiveDir + "/%s_results" % (str( datetime.now().date() )) 

try:
    if(os.path.exists(baseArchiveDir) == False):
        os.mkdir(baseArchiveDir)

    if(os.path.exists(archiveDir) == False):
        shutil.copytree(currentResultDir, archiveDir)
    else:
        print "%s archive Directory already present" % (str( datetime.now().date() )) 

    print "ARCHIVE SUCCESSFUL"

except Exception, e:
    print "ARCHIVE FAILED , %s" % e

finally:
    os.chdir(scriptsDir) #original directory
