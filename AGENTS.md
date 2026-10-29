# ALBA - agent instructions

## Environment
- Windows only. C++ with VTK 9 and wxWidgets, built with CMake and Visual Studio 2022.
- Unit tests use CppUnit.
- Use English for all identifiers and comments.
- Folder layout: sources in the repository root (e.g. `C:\Alba\ALBA`), build tree
  in the sibling folder with the `_B` suffix (e.g. `C:\Alba\ALBA_B`). The scripts
  below derive both paths, nothing is hardcoded.
- MSBuild is used from PATH if `VsDevCmd.bat` was called before starting the
  agent, otherwise the scripts locate it with `vswhere`.

## Build and test
Always use exactly these commands. Never invent, replace, or bypass them.

- Configure (only if CMakeLists.txt changed, or the build directory is empty):
  `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/configure.ps1`
- Build: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/build.ps1`
  After the first successful build it sets `COMPILE_LIBRARIES=OFF`, so the
  external libraries (VTK, GDCM, XercesC, CURL, CRYPTO) are not rebuilt again.
- Rebuild the external libraries (only if asked, or if something under `Libs/`
  changed): `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/configure.ps1 -CompileLibraries`,
  then the build.
- Test: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/test.ps1`

### Execution rules
- These scripts are long-running (configure and build can take well over 10
  minutes). If the tool's foreground timeout cannot cover that, run the script
  in the background and wait for its completion notification instead of
  polling.
- Run one script at a time. Do not chain them, and do not use Start-Job or
  Start-Process yourself.
- Wait until the script returns its exit code. Do not run other scripts, edit
  files, or draw conclusions while it is running.
- Never kill or restart a running script. Only the user may interrupt it.
- Never delete, rename, or recreate the build directory without explicit
  permission from the user. If configure or build fails, investigate the error
  first and ask before considering a clean build.
- Each script prints the exit code and the first matching errors. The full log
  is saved under `build-logs/`.
- A task is complete only when build and test both exit with code 0.

## Tests
- Tests read their input files from `ALBA_DATA_ROOT` (`Testing/unittestData`),
  which is part of the repository: never write there.
- Output files go under `GET_TEST_DATA_DIR() + "\\"`, which is inside the build
  tree. Never use bare relative file names: the current directory may have
  been changed by a previous test.
- Use `CPPUNIT_ASSERT`, not `assert`: tests run in Release, where `assert` is
  compiled out.
- After running the tests, `git status` must show no new or modified files in
  the source tree.

## Git
- Branching policy is in CONTRIBUTING.md — read it before any git operation.
- Never push to `master` or `develop` without explicit confirmation.
