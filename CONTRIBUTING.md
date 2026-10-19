# Contributing Guidelines

## Coding Standards

### Compact C++ Formatting

- For `if` and `else` blocks containing a single statement, omit braces and place the statement on the next indented line.
- Avoid `const` and `static_cast` for local variables and member-related code unless there is a strong technical justification.
- Keep `if` conditions, assignments, and function arguments on a single line where practical. Prefer longer lines over unnecessary line wrapping.
- Favor compact, clear, and readable code without unnecessary syntactic or formatting noise.

### General Rules

- Follow the existing project formatting and naming conventions.
- Write code comments and variable names in English.
- Preserve CRLF line endings and use spaces for indentation as defined by `.editorconfig`.

## Branching Policy

- `master` and `develop` are the two long-lived branches.
- For every feature or fix: create a branch from `master`, named
  `feature/<short-name>`. All commits for that work happen on this branch.
- When the work is ready, merge the branch into `develop` first.
- Only after tests pass on `develop`, merge the SAME branch into `master`.
  Never merge `develop` into `master` directly.
- Direct commits to `master` are allowed only for changes that cannot affect
  code or tests (icons, splash screen, documentation). If more than one such
  commit is needed, create a branch instead, even for non-code changes.
- Never force-push, never rewrite history on a shared branch.
