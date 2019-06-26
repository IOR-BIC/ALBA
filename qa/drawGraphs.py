import datetime
import os
import sys
import getopt

from xml.dom import minidom as xd

from matplotlib.pyplot import figure, show
from matplotlib.dates import DayLocator, HourLocator, DateFormatter, drange
from numpy import arange

"""
try:
    from qa import albaPath
except ImportError:
    import albaPath
"""
currentPathScript = os.path.split(os.path.realpath(__file__))[0]
#modulesDir = albaPath.albaSourcesDir
#outputDir = albaPath.albaQADir

ruleArray = []

""" ruleArray is an array of ruleDictionary
"""

""" ruleDictionary
    {
        "name": FileNameRule
        "days":   [list of days]
        "values": [list of values]
    }
"""


def parseData():
    root = os.path.join("/home/guestadmin/workspace/QAResults");
    resultFiles = []
    # walk on the filesystem listing xml files in an array of full paths
    for oDirPaths, oDirNames, oFiles in os.walk( root, True, None ):
        for file in oFiles:
            extension = file.split(".")[-1]
            path = oDirPaths.split("/")[-1]
            if(extension == "xml" and path == "xml"):
                resultFiles.append( os.path.join(oDirPaths,file) )
    
    #print "\n".join(resultFiles)
    # parse each file, retrieving information
    for item in resultFiles:
        day = None
        rule = None
        value = -1
        #get the day
        day = item.split("/")[-3][:-8]
        #get the rule
        rule = item.split("/")[-1][:-4]
        #get the value
        dom = xd.parse(item)
        if("Coverage" in rule):
            value = dom.getElementsByTagName('root')[0].getElementsByTagName('results')[0].getElementsByTagName('percentage')[0].firstChild.nodeValue
        else:
            value = dom.getElementsByTagName('root')[0].getElementsByTagName('results')[0].getElementsByTagName('percentageCoverage')[0].firstChild.nodeValue
        
        #print day, rule, value[-3:-1]
        #search if the rule is already inserted
        ruleDictionary = None
        for r in ruleArray:
            if(r['name'] == rule):
                ruleDictionary = r
            
        if(ruleDictionary == None):
            ruleDictionary = {}
            ruleDictionary['name'] = rule
            ruleDictionary['days'] = []
            ruleDictionary['values'] = []
            ruleArray.append(ruleDictionary)
            
        ruleDictionary['days'].append(day)
        ruleDictionary['values'].append(int(value[-4:-2]))
        #print ruleDictionary    
    
    # graph data
    

def drawGraphs():
    print "Start Drawing..."
    for rule in ruleArray:
        days   = rule['days'] 
        values = rule['values']
            
        dates = []
        for day in days:
            y,m,d = day.split("-")
            v = datetime.datetime( int(y), int(m), int(d))
            dates.append(v)
        
        #print rule['name']    
        #print dates, len(dates)
        #print values, len(values)
        
        fig = figure()
        ax = fig.add_subplot(111)
        ax.plot_date(dates, values, '-')
    
        # The hour locator takes the hour or sequence of hours you want to
        # tick, not the base multiple

    
        fig.savefig(rule['name'] +'.png', dpi=300)

def run(param):
    parseData()
    drawGraphs()

def usage():
    print "Usage: python drawGraphs.py [-h]"
    print "-h, --help                    show help (this)"
    print 

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help",])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            return
        else:
            assert False, "unhandled option"

    param = {}
    run(param)
    
if __name__ == "__main__":
  main()
