REM -----------------------------------------------------------------------------
REM Doxygen documentation generation
REM -----------------------------------------------------------------------------

IF "%1" == "MAF" GOTO MAF
IF NOT "%1" == "MAF" GOTO UNKNOWN_CONDITION

:MAF

  REM home directory is openMAF_ITK_openMAF_ITK_Parabuild
 
  cd .\openMAF_ITK_Parabuild\Docs\Doxygen
  
  REM remove old stuff
  rmdir /s /q html
  del /q *.tar.gz
  
  REM Launch Doxygen  
  call makedoc.bat
   
  REM tar the directory
  REM Launch Doxygen
  ..\..\Libraries\Extras\bin\tar.exe cf MAF_DOXYGEN_TS_%PARABUILD_BUILD_TIMESTAMP%_BUILD_%PARABUILD_BUILD_NUMBER%.tar html
  
  REM gzip the directory
  ECHO running gzip
  ..\..\Libraries\Extras\bin\gzip.exe MAF_DOXYGEN_TS_%PARABUILD_BUILD_TIMESTAMP%_BUILD_%PARABUILD_BUILD_NUMBER%.tar
  
  REM back to starting directory
  cd ..\..\..
 
GOTO END

:UNKNOWN_CONDITION

ECHO Doxygen documentation created!

:END