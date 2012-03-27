REM -----------------------------------------------------------------------------
REM driver batch file for deploy the application
REM -----------------------------------------------------------------------------


IF "%1" == "VAPP_DEBUG" GOTO VAPP_DEBUG
IF "%1" == "VAPP_RELEASE" GOTO VAPP_RELEASE
IF "%1" == "VAPP_RELEASE_SLIM" GOTO VAPP_RELEASE_SLIM
IF "%1" == "VAPP_RELEASE_FAT" GOTO VAPP_RELEASE_FAT
IF NOT "%1" == "MAF_DEBUG" GOTO UNKNOWN_CONDITION

:VAPP_DEBUG

REM remove the deploy directory and create it again
rmdir /s /q  %PARABUILD_CHECKOUT_DIR%\openMAF_ITK_Medical_Parabuild\Medical_Parabuild\bin\Deploy
mkdir %PARABUILD_CHECKOUT_DIR%\openMAF_ITK_Medical_Parabuild\Medical_Parabuild\bin\Deploy


cd Medical_Parabuild
cd bin
cd Debug

"%ProgramFiles%\NSIS\makensis.exe" /V4 iposeInstallerScript.nsi

cd ..
cd ..
cd ..

GOTO END


:VAPP_RELEASE

REM remove the deploy directory and create it again
rmdir /s /q  %PARABUILD_CHECKOUT_DIR%\openMAF_ITK_Medical_Parabuild\Medical_Parabuild\bin\Deploy
mkdir %PARABUILD_CHECKOUT_DIR%\openMAF_ITK_Medical_Parabuild\Medical_Parabuild\bin\Deploy

cd Medical_Parabuild
cd bin
cd Release

"%ProgramFiles%\NSIS\makensis.exe" /V4 iposeInstallerScript.nsi

cd ..
cd ..
cd ..

GOTO END

:UNKNOWN_CONDITION

:END