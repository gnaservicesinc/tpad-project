# Repository Guidelines

## Project Structure & Module Organization
- src/: C sources and headers for the tpad binary. Feature modules follow the tpad_*.c/.h pattern; vendored crypto headers live under src/mbedtls/.
- po/: Localization files. icon/: App icons. man/: Manpages.
- script/: Helper scripts. debian/: Packaging metadata. snap/snapcraft.yaml: Snap build config.
- Build system: GNU Autotools (configure.ac, Makefile.am, m4/). The built binary is src/tpad.

## Build, Test, and Development Commands
- ./autogen.sh: Bootstrap Autotools (run after cloning or when build files change).
- ./configure --prefix="$PWD/.local": Configure for a local install path.
- make -j$(nproc): Compile the project. Output binary at src/tpad.
- make install: Install to the configured prefix. make clean: Remove build artifacts.
- make check: Run the automated unit and integration tests.
- make dist: Produce a release tarball. For packaging, use debian/ or snap/snapcraft.yaml as needed.
- Run locally: ./src/tpad

## Coding Style & Naming Conventions
- Indentation: Tabs with 4-space width; c-basic-offset: 4 (see Emacs modeline in src/main.c).
- Language: ISO C; release-audit builds use strict warning flags. Keep code warning-free.
- Modules: Use tpad_* names with matching headers (tpad_foo.c and tpad_foo.h). Prefer snake_case for functions and variables.
- Headers: Keep includes minimal; place shared declarations in tpad_*.h and include via "tpad_headers.h" when appropriate.

## Testing Guidelines
- Automated tests live under tests/ and are wired into make check via Automake; name new test files test_*.c.
- Manual smoke tests: build, run ./src/tpad, verify open/save, find/replace, preferences, and multi-file launch (e.g., ./src/tpad README.md AUTHORS).

## Commit & Pull Request Guidelines
- Messages: Use imperative mood and concise scope (e.g., fix: avoid TOCTOU in file open). History shows mixed styles; prefer a clear type: short summary format.
- PRs: Describe What/Why/How, link related issues, include build output or steps to reproduce, and screenshots if UI is affected.
- Scope: Keep changes focused; update man/ and translations (po/) when user-visible strings or behavior change.

## Security & Configuration Tips
- File I/O: Avoid time-of-check/time-of-use races; prefer atomic operations and validate after open. Handle untrusted paths carefully.
