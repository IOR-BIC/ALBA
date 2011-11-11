import string, sys, os, fnmatch

instIncDir=sys.argv[1]
sourceIncDirs=sys.argv[2]

sourceDirList=sourceIncDirs.split(";")

#creating array of files from source folders
sourceIncFiles=[]
for dir in sourceDirList:
	for file in os.listdir(dir):
		if fnmatch.fnmatch(file, '*.h') or fnmatch.fnmatch(file, '*.txx') or fnmatch.fnmatch(file, '*.xpm'):
			sourceIncFiles.append(file)


#creating array of files in destination folder
instIncFiles=[]
for file in os.listdir(instIncDir):
	if fnmatch.fnmatch(file, '*.h') or fnmatch.fnmatch(file, '*.txx') or fnmatch.fnmatch(file, '*.xpm'):
		instIncFiles.append(file)

#removing file that are already removed in source folders
for file in instIncFiles:
	if not file in sourceIncFiles:
		os.remove(instIncDir+"/"+file)
		print ("DELETED: "+instIncDir+"/"+file)
		