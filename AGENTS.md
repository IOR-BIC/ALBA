# ALBA - agent instructions

## Environment
- Windows only. C++ with VTK 9 and wxWidgets, built with CMake and Visual Studio.
- Unit tests use CppUnit.
- Use English for all identifiers and comments.
- Model and provider configuration live in the machine's global OpenCode
  config (`~/.config/opencode/opencode.json`), not in this repo.

## Build and test
Always use these commands. Never invent other build or test commands.
- Configure (only if CMakeLists.txt changed, or the build directory is empty):
  `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/configure.ps1`
- Build: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/build.ps1`
- Test: `powershell -NoProfile -ExecutionPolicy Bypass -File tools/agent/test.ps1`
- Each script prints the exit code and the first matching errors. The full log
  is saved under `build-logs/`.
- A task is not complete until build and test both exit with code 0.

## Git
- Branching policy is in CONTRIBUTING.md — read it before any git operation.
- Never push to `master` or `develop` without explicit confirmation.