REM ----------------------------------------------------------------------------
REM TestComplete Result Copy
REM ----------------------------------------------------------------------------

echo.

cd ..

set CheckNameResultsDirectory=GuiTestResult


REM delete the directory storing previous results
rmdir /s /q %CheckNameResultsDirectory%

REM create the directory again the directories for storing XML and HTML file name checking results
mkdir %CheckNameResultsDirectory%

echo  creating file name check results... 
cd %CheckNameResultsDirectory%
xcopy ..\..\..\..\..\..\..\..\..\..\Results\Resultlog 




echo.
