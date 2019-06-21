REM -----------------------------------------------------------------------------
REM Doxygen documentation generation
REM -----------------------------------------------------------------------------

IF "%1" == "ALBA" GOTO ALBA
IF NOT "%1" == "ALBA" GOTO UNKNOWN_CONDITION

:ALBA

  REM home directory is ALBA_ITK_ALBA_ITK_Parabuild
 
  cd .\ALBA_ITK_Parabuild\Docs\Doxygen
  
  REM remove old stuff
  rmdir /s /q html
  del /q *.tar.gz
  
  REM Launch Doxygen  
  call makedoc.bat
   
  REM tar the directory
  REM Launch Doxygen
  ..\..\Libraries\Extras\bin\tar.exe cf ALBA_DOXYGEN_TS_%PARABUILD_BUILD_TIMESTAMP%_BUILD_%PARABUILD_BUILD_NUMBER%.tar html
  
  REM gzip the directory
  ECHO running gzip
  ..\..\Libraries\Extras\bin\gzip.exe ALBA_DOXYGEN_TS_%PARABUILD_BUILD_TIMESTAMP%_BUILD_%PARABUILD_BUILD_NUMBER%.tar
  
  REM back to starting directory
  cd ..\..\..
 
GOTO END

:UNKNOWN_CONDITION

ECHO Doxygen documentation created!

:END