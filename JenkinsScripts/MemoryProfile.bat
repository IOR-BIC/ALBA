REM -----------------------------------------------------------------------------
REM AQTime memory profiler driver script
REM -----------------------------------------------------------------------------
echo  ** memory profile started **
echo.

REM BEWARE! These directory names are  referenced in MemoryProfile.vbs script so the not change them
set ProfilingResultsDirectory=.\build\bin\Debug\MemoryAllocation\
set XMLResultsDirectory=%ProfilingResultsDirectory%\XML\
set HTMLResultsDirectory=%ProfilingResultsDirectory%\HTML\

REM delete the directory storing previous results
rmdir /s /q %ProfilingResultsDirectory%

REM create the directory again the directories for storing XML and HTML memory profiling results
mkdir %XMLResultsDirectory%
mkdir %HTMLResultsDirectory%
echo  creating profiling results... 

REM run the memory profile Visual Basic script
.\ParabuildScripts\MemoryProfile.vbs
echo.
echo   profiling results created successfully!
echo.
echo **  memory profile ended successfully **