# ALBA - agent instructions

## Environment
- Windows only. C++ with VTK 9 and wxWidgets, built with CMake and Visual Studio.
- Unit tests use CppUnit.
- Use English for all identifiers and comments.
- Model and provider configuration live in the machine's global OpenCode
  config (`~/.config/opencode/opencode.json`), not in this repo.

## Build and test
Always use these commands. Never invent, replace, or bypass the build/test commands.

- Configure (only if CMakeLists.txt changed, or the build directory is empty):
  `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/configure.ps1`
- Build: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/build.ps1`
- Test: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/test.ps1`

### Execution rules

- **Never impose a timeout** on configure and build. These operations are allowed to take as long as necessary. Do not terminate them because they appear to be taking too long. The user will interrupt them manually if necessary.
- **Always wait for each script to finish and return its exit code before taking any other action.** Do not start another command, inspect or modify the project, launch another build/test/configure process, or make changes based on an incomplete result while a script is still running.
- Configure, build, and test are **synchronous operations**. After starting one of the scripts, wait until it has completely terminated before proceeding.
- **Never run configure, build, or test concurrently or in the background.**
- **Never delete, rename, recreate, or otherwise discard the build directory without explicitly asking the user for permission first.** Recreating the build directory can take hours and may leave the project unavailable for a long time.
- If configure or build fails, **do not automatically delete or recreate the build directory**. Investigate the error first and ask the user before considering any operation that would remove the existing build directory.
- If a clean build is considered necessary, explain why and **ask the user for confirmation before deleting the build directory**.
- The build directory is considered valuable state and must be preserved unless the user explicitly authorizes its removal.
- Do not use Start-Job, Start-Process, background execution, shell operators, or any other mechanism that allows the script to continue running after the command invocation returns.
- Each script prints the exit code and the first matching errors. The full log is saved under `build-logs/`.
- A task is not complete until build and test both exit with code 0.

## Git
- Branching policy is in CONTRIBUTING.md — read it before any git operation.
- Never push to `master` or `develop` without explicit confirmation.