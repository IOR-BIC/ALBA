  '-----------------------------------------------------------------------------
  ' AQTime memory profiler execution script
  '-----------------------------------------------------------------------------
  
  ' load current summary file
  ' search for node LiveObjects
  ' get the node value
  Function GetLiveObjects(strSourceFileName)
   Set XMLObj = CreateObject("Msxml2.DOMDocument")
   If XMLObj.load(CStr(strSourceFileName)) Then
       
       Set XMLRoot = XMLObj.documentElement
       
       ' MsgBox XMLRoot.ChildNodes.item(1).text
       GetLiveObjects = XMLRoot.ChildNodes.item(1).text
   Else
       MsgBox "The file cannot be loaded"
   End IF
  End Function

  relativeExePath = ".\bin\debug\"
  
  ' create a text log
  Set fso = CreateObject("Scripting.FileSystemObject")
  Set logFile = fso.CreateTextFile(".\bin\Debug\MemoryAllocation\XML\MemoryAllocationLog.txt", True)
  logFile.WriteLine("** Log file for AQTime memory profiling: **")
  logFile.WriteLine("")
  logFile.Write("Memory profile started on ")
  logFile.WriteLine(Now)
  logFile.WriteLine("")
   
  Dim path, i, intTestedFiles, intLeakedFiles, intTotalLeak
  Dim fso, dir, ExeFile
  
  intTestedFiles = 0
  intLeakedFiles = 0
  intTotalLeak = 0

  ' Path in which create projects; executables are located here (parabuil dependent)  
  ' Parabuild active directory is the downloaded CVS module name for example OpenMAF
  
  i = 1
  
  Set fso = CreateObject("Scripting.FileSystemObject")
  
  Set absoluteExePath = fso.GetFolder(relativeExePath)

  For Each ExeFile In absoluteExePath.Files
  
    Dim txt,pos,exeFileName
         
    txt=CStr(ExeFile)
    pos=InStr(txt,".")
    pos = pos - 1
    relativePathFileName = Left(txt, pos)
    
    'MsgBox "relativePathFileName: " + CStr(relativePathFileName)
    
    txt = CStr(relativePathFileName)
    pos=InStrRev(txt,"\")
    pos = pos
    exeFileName = Right(txt, Len(relativePathFileName) - pos)       
    
    'MsgBox "exeFileName: " + CStr(exeFileName)
    
    txt=CStr(relativePathFileName)
    pos =  Len(relativeExePath)
    relativePathFileName = Right(txt, Len(relativePathFileName) - pos)
      
    'MsgBox "relativePathFileName: " + CStr(relativePathFileName)
    
    '-----------------------------------------------------------------------------
    ' executed tests name pattern 
    '-----------------------------------------------------------------------------
    ' Works with files beginning with "maf" ending with "Test.exe" files only
    ' only tests matching this pattern are executed
    ' is you need to modify this behavior modify the next line
    
    ' Names pattern examples:
    ' pattern to test all maf*Test.exe files
    ' If  Left(exeFileName, 3) = "maf" And Right(ExeFile, 8) = "Test.exe" Then 
    
    ' CURRENT ACTIVE TEST NAME PATTERN
    ' pattern to test all *.exe files
    If Right(ExeFile, 4) = ".exe" Then 
    
      ' Connects to AQtime
      Set AQtimeObject = CreateObject("AQtime.AQtime")
   
      ' Obtains the IntegrationManager
      Set IntegrationManager = AQtimeObject.IntegrationManager
   
      Dim canProfile 
      canProfile = True
          
      ' Creates the new project from an exe module
      If Not IntegrationManager.NewProjectFromModule(ExeFile) Then
        'MsgBox "Cannot create the new project."
         Dim newProjectMessage
         newProjectMessage = "Cannot create new AQTime project from " + CStr(exeFileName)
         logFile.WriteLine(CStr(newProjectMessage))
         canProfile = False       
         ' Closes AQtime
         AQtimeObject.Quit
      End If
    
      If canProfile = True Then
        logFile.WriteLine("Starting AQTime on: " + exeFileName)
        ' Selects the desired profiler
        If Not IntegrationManager.SelectProfiler("Allocation Profiler") Then 
          'MsgBox "The specified profiler was not found."
           Dim profilerMessage
           profilerMessage = "Cannot find the Allocation Profiler"
           logFile.WriteLine(CStr(profilerMessage))
           canProfile = False
           ' Closes AQtime
           AQtimeObject.Quit
        End If
      End If        
      
      If canProfile = True Then      
        ' Starts profiling and saves results to xml files: MemoryAllocation dir must exist inside relativeExePath dir
        Dim XMLFileName
        XMLSummaryFile = CStr(absoluteExePath) + "\MemoryAllocation\XML\AllocationSummary-" + CStr(exeFileName) + ".xml"
        XMLResultsFile = CStr(absoluteExePath) + "\MemoryAllocation\XML\AllocationResults-" + CStr(exeFileName)+".xml"
        'MsgBox "XMLSummaryFile: " + CStr(XMLSummaryFile)
        'MsgBox "XMLResultsFile: " + CStr(XMLResultsFile)
      
        call IntegrationManager.Start(CStr(XMLSummaryFile),CStr(XMLResultsFile))
        
        i = i + 1
      
        ' Waits until profiling is over
        While IntegrationManager.ProfilingStarted
          ' the Sleep command is needed otherwise the profiler application will hang
          WScript.Sleep 10000
        Wend
        
        WScript.Sleep 10000
        
       
        ' increase the tested files number
        intTestedFiles = intTestedFiles + 1
        
        ' parse the summary file for LiveObjects node: is this node is founded then memory leaks are present
        Dim strLiveObjectsNumber
        strLiveObjectsNumber = GetLiveObjects(XMLSummaryFile)
        If strLiveObjectsNumber = "0" Then 
          ' MsgBox "no leaks detected"
        Else
          ' MsgBox CStr(exeFileName) + " has leaks! " + CStr(strLiveObjectsNumber) + " live objects detected"
          Dim message
          message = CStr(exeFileName) + " has leaks! " + CStr(strLiveObjectsNumber) + " live objects detected"
          logFile.WriteLine(CStr(message))
          ' increase leaked files number
          intLeakedFiles = intLeakedFiles + 1
          intTotalLeak = intTotalLeak + strLiveObjectsNumber
          
        End If
        
     ' Closes AQtime
     AQtimeObject.Quit
     WScript.Sleep 30000
  
     Else 'canProfile
       'skipping the file 
     End If 
     
  Else 
   'MsgBox "skipping the file: " + CStr(exeFileName)  
  End If 
  
  Next 
  
  ' closed the Log file
  Dim strEndMessage, strCurrentNumber
  logFile.WriteLine("")
  logFile.WriteLine("profiled " + CStr(intTestedFiles) + " files") 
  logFile.WriteLine("found " + CStr(intLeakedFiles) + " leaked files")  
  logFile.WriteLine(CStr(Int(intLeakedFiles / intTestedFiles * 100)) + "% leaked")
  logFile.WriteLine("")
  logFile.WriteLine(CStr(intTotalLeak) + " live objects detected") 
  logFile.WriteLine("")
  logFile.Write("Memory profile ended on ")
  logFile.WriteLine(Now)
  logFile.WriteLine("")
  logFile.WriteLine("** End of log file **")
  logFile.Close
  