REM -----------------------------------------------------------------------------
REM Unit tests driver script
REM -----------------------------------------------------------------------------
CALL "C:/Programmi/Microsoft Visual Studio .NET 2003/Common7/Tools/vsvars32.bat"
devenv ./Medical_Parabuild/MED.sln /project RUN_TESTS.vcproj  /build debug /out build_log
 