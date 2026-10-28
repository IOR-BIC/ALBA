# ALBA - agent instructions

## Environment
- Windows only. C++ with VTK 9 and wxWidgets, built with CMake and Visual Studio.
- Unit tests use CppUnit.
- Use English for all identifiers and comments.
- Model and provider configuration live in the machine's global OpenCode
  config (`~/.config/opencode/opencode.json`), not in this repo.

## Build and test
Always use exactly these commands. Never invent, replace, or bypass them.

- Configure (only if CMakeLists.txt changed, or the build directory is empty):
  `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/configure.ps1`
- Build: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/build.ps1`
- Test: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/test.ps1`

### Execution rules
- These scripts are long-running (a build can take well over 10 minutes). When
  calling the bash tool for any of them, ALWAYS set the `timeout` parameter to
  `7200000` (2 hours). The default bash timeout is only 2 minutes and would kill
  the build.
- Run one script at a time, in the foreground. Do not use Start-Job,
  Start-Process, or any background execution.
- Wait until the script returns its exit code. Do not run other commands, edit
  files, or draw conclusions while it is running.
- Never kill or restart a running script. Only the user may interrupt it.
- Never delete, rename, or recreate the build directory without explicit
  permission from the user. If configure or build fails, investigate the error
  first and ask before considering a clean build.
- Each script prints the exit code and the first matching errors. The full log
  is saved under `build-logs/`.
- A task is complete only when build and test both exit with code 0.

## Git
- Branching policy is in CONTRIBUTING.md — read it before any git operation.
- Never push to `master` or `develop` without explicit confirmation.