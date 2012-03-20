REM -----------------------------------------------------------------------------
REM Doxygen documentation generation
REM -----------------------------------------------------------------------------

IF "%1" == "MAFMED" GOTO MAFMED
IF NOT "%1" == "MAFMED" GOTO UNKNOWN_CONDITION

:MAFMED

  REM home directory is openMAF_ITK_Medical_Parabuild
    
  cd .\Medical_Parabuild\Docs\Doxygen
  
  REM remove old stuff
  rmdir /s /q html
  del /q *.tar.gz
  
  REM Launch Doxygen
  call makedoc.bat
   
  REM tar the directory
  ECHO running tar
  ..\..\..\openMAF_ITK_Parabuild\Libraries\Extras\bin\tar.exe cf MAFMEDICAL_DOXYGEN_TS_%PARABUILD_BUILD_TIMESTAMP%_BUILD_%PARABUILD_BUILD_NUMBER%.tar html
  
  REM gzip the directory
  ECHO running gzip
  ..\..\..\openMAF_ITK_Parabuild\Libraries\Extras\bin\gzip.exe MAFMEDICAL_DOXYGEN_TS_%PARABUILD_BUILD_TIMESTAMP%_BUILD_%PARABUILD_BUILD_NUMBER%.tar
  
  REM back to starting directory
  cd ..\..\..
 
GOTO END

:UNKNOWN_CONDITION

ECHO Doxygen documentation created!

:END

 