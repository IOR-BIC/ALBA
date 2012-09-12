REM -----------------------------------------------------------------------------
REM AQTime coverage profiler driver script
REM -----------------------------------------------------------------------------
echo  ** coverage profile started **
echo.

REM BEWARE! These directory names are  referenced in MemoryProfile.vbs script so the not change them
set ProfilingResultsDirectory=.\build\bin\Debug\CoverageAllocation\
set XMLResultsDirectory=%ProfilingResultsDirectory%\XML\
set HTMLResultsDirectory=%ProfilingResultsDirectory%\HTML\

REM delete the directory storing previous results
REM rmdir /s /q %ProfilingResultsDirectory%

REM create the directory again the directories for storing XML and HTML coverage profiling results
REM mkdir %XMLResultsDirectory%
REM mkdir %HTMLResultsDirectory%
echo  creating profiling results... 

REM run the coverage profile Visual Basic script
.\source\JenkinsScripts\CoverageProfile.vbs

ren .\build\bin\Debug\Coverage\XML\CoverageAllocationLog.txt %1%
echo.
echo   profiling results created successfully!
echo.
echo **  coverage profile ended successfully **