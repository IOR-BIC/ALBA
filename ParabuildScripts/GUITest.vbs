  '-----------------------------------------------------------------------------
  ' TestComplete driver script
  '-----------------------------------------------------------------------------

  Dim TestCompleteApp
  Dim IntegrationObject
  Dim LastResult

  ' create a text log    
  Set fso = CreateObject("Scripting.FileSystemObject")
  Set logFile = fso.CreateTextFile(".\Medical_Parabuild\bin\debug\GUITesting\GUITestLog.txt", True)
  logFile.WriteLine("** Log file for TestComplete GUI testing: **")
  logFile.WriteLine("")
  logFile.Write("GUI testing started on ")
  logFile.WriteLine(Now)
  logFile.WriteLine("")
  
  ' Creates the application object
  Set TestCompleteApp = CreateObject("TestExecute.TestExecuteApplication")
  
  ' Obtains the integration object
  Set IntegrationObject = TestCompleteApp.Integration
  
  ' Opens the project; this is downloaded from the TestComplete CVS module and placed inside ApplicationModule/TestComplete/
  IntegrationObject.OpenProjectSuite "C:\Build\TestComplete\TestSuite\TestSuite.pjs"
  
  ' Checks whether the project was opened
  If Not IntegrationObject.IsProjectSuiteOpened Then
  ' MsgBox "The project suite was not opened."
  End If
    
  ' Starts the project run
  IntegrationObject.RunProject "TestProject"
  
  ' Waits until the test is over
  While IntegrationObject.IsRunning
    ' the Sleep instruction is needed since otherwise TestComplete will hang during execution
    WScript.Sleep 1000
  Wend
  
  Set LastResult = IntegrationObject.GetLastResultDescription
  logfile.WriteLine("Now testing " + CStr(LastResult.TestType) + " ...")
  logfile.WriteLine("error count: " + CStr(LastResult.ErrorCount))
  logfile.WriteLine("warning count: " + CStr(LastResult.WarningCount))
  logfile.WriteLine("exit status: " + CStr(LastResult.Status))
  logfile.WriteLine("is test completed?: " + CStr(LastResult.IsTestCompleted))
  
  ' close the Log file
  logFile.WriteLine("")
  logFile.WriteLine("")
  logFile.Write("GUI testing ended on ")
  logFile.WriteLine(Now)
  logFile.WriteLine("")
  logFile.WriteLine("** End of log file **")
  logFile.Close


  ' Closes TestComplete
  TestCompleteApp.Quit
  
