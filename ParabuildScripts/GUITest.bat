REM -----------------------------------------------------------------------------
REM TestComplete driver script
REM -----------------------------------------------------------------------------

echo ** starting TestComplete GUI testing **
echo.
echo performing tests...

REM BEWARE! These directory names are  referenced in GUITest.vbs script so the not change them
set GUITestResultsDirectory=.\bin\debug\GUITesting\
rmdir /s /q %GUITestResultsDirectory%
mkdir %GUITestResultsDirectory%
.\Medical_Parabuild\ParabuildScripts\GUITest.vbs
echo.
echo writing results...
echo.
echo ** GUI testing ended **


