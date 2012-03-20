REM ----------------------------------------------------------------------------
REM File name convention checking
REM ----------------------------------------------------------------------------
echo  ** File name checker started **
echo.

cd .\Medical_Parabuild\ParabuildScripts\MedicalFileNameCheck

set CheckNameResultsDirectory=..\..\bin\Debug\FileNameCheckResult\
set XMLResultsDirectory=%CheckNameResultsDirectory%\XML\
set HTMLResultsDirectory=%CheckNameResultsDirectory%\HTML\

REM delete the directory storing previous results
rmdir /s /q %CheckNameResultsDirectory%

REM create the directory again the directories for storing XML and HTML file name checking results
mkdir %CheckNameResultsDirectory%
mkdir %XMLResultsDirectory%
mkdir %HTMLResultsDirectory%

echo  creating file name check results... 

FileNameConvention.exe %XMLResultsDirectory%

cd ..
cd ..

echo.
echo   file name check results created successfully!
echo.
echo ** file name check ended successfully **
