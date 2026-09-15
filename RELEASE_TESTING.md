# Tpad release testing

This file records the repeatable release-audit procedure. Run commands from
the repository root. Audit output is written below `BUILDLOGS/`, which is
ignored by Git.

## Tool setup

Use the versioned LLVM 23 programs and runtime. Do not install the unversioned
`clang`, `llvm`, or `clang-tools` packages: Ubuntu may resolve those names to a
different LLVM release.

The audit expects these tools:

```sh
apt-get install --no-install-recommends \
  clang-23 clang-tools-23 llvm-23 llvm-23-tools libclang-rt-23-dev \
  valgrind xvfb xdotool cppcheck flawfinder shellcheck
```

Install the project build dependencies listed in `debian/control` before the
first build. The GTK frontend requires GTK 4.10 or newer, GtkSourceView 5,
and libspelling. Compatibility testing also requires GTK 3, GtkSourceView 3,
and GtkSpell 3. About 4 GB of free disk space is sufficient because each
audit uses a separate, small, out-of-tree build directory.

The installed LLVM 23 manual is under `/usr/share/doc/clang-23/html/`. The
online equivalents are the official Clang documentation for
[AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html),
[LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html),
[UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html),
[ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html),
[TypeSanitizer](https://clang.llvm.org/docs/TypeSanitizer.html),
[SanitizerCoverage](https://clang.llvm.org/docs/SanitizerCoverage.html), and the
[Clang Static Analyzer](https://clang.llvm.org/docs/ClangStaticAnalyzer.html).

## Audit matrix

The selected builds reflect sanitizer compatibility and the kinds of code in
tpad.

| Build | Instrumentation | Reason |
| --- | --- | --- |
| `build` | Strict clang-23 warnings | Release build, unit tests, and GUI/print smoke test |
| `analyzer` | Clang 23 Static Analyzer | Path-sensitive ownership, bounds, and API analysis |
| `asan` | Address + UndefinedBehavior + integrated LeakSanitizer | ASan/UBSan cover unit and GUI paths; LSan covers instrumented unit and fuzz targets |
| `tsan` | ThreadSanitizer build and unit test | Confirms instrumentation; active tpad code has no application-managed threads |
| `tysan` | TypeSanitizer | Separate build because TySan cannot be combined with other sanitizers |
| `valgrind` | Memcheck | Strict core leak check plus GUI invalid-access and advisory leak records |
| `fuzz` | libFuzzer + ASan + UBSan + SanitizerCoverage | Coverage-guided exercise of replacement, encoding, hashing, and BOM code |
| `extras` | Cppcheck, Flawfinder, ShellCheck | Independent source and developer-script review |

The following Clang facilities are deliberately not full-application builds:

- MemorySanitizer requires dependencies, including libc and the GTK stack, to
  be rebuilt with MSan. Using the system libraries would create unreliable
  uninitialized-value reports.
- DataFlowSanitizer is a framework that needs application-specific data labels
  and policies; it is not a general bug detector.
- RealtimeSanitizer only checks functions marked as nonblocking real-time
  entry points. Tpad has no real-time threads or deadlines.
- SanitizerStats currently records CFI check frequency. It does not find a bug
  class and tpad's C/GTK callback build does not use whole-program CFI.
- Standalone LeakSanitizer duplicates the leak detector enabled in the ASan
  build. Valgrind supplies the independent second leak implementation.
- SanitizerCoverage is enabled in the libFuzzer build, where its feedback is
  actionable, rather than in a normal GUI run.

## Running the checks

Regenerate the build system after changing `configure.ac` or a `Makefile.am`:

```sh
NOCONFIGURE=1 ./autogen.sh
```

Run one audit at a time while investigating findings:

```sh
./tools/release-audit.sh build
./tools/release-audit.sh analyzer
./tools/release-audit.sh asan
./tools/release-audit.sh tsan
./tools/release-audit.sh tysan
./tools/release-audit.sh valgrind
TPAD_FUZZ_SECONDS=300 ./tools/release-audit.sh fuzz
./tools/release-audit.sh extras
```

Once every individual check passes, run the complete sequence:

```sh
./tools/release-audit.sh all
```

The GUI helpers start tpad under Xvfb. They open File → Print through Ctrl+P,
exercise open/save and Find / Replace, open Preferences, verify multi-file
launching (including a non-ASCII UTF-8 filename), and close each editor
normally. A separate integration-test binary
renders a source buffer through the print compositor and verifies that GTK
wrote a PDF. A physical printer should also be checked manually when one is
available.

Manual frontend UI checks should also confirm:

- On macOS, the Qt toolbar is text-only, has no generic icons or drag handle,
  and groups New/Open/Save/Save As, Reload, Undo/Redo, and Quit. The Edit menu
  must still contain its complete action set. Right-click the editor with and
  without a selection, choose Writing Tools, close it, and invoke it again;
  each invocation must open the system UI without crashing.
- GTK Frequency Analysis handles both selections and whole documents,
  including Unicode and many unique characters, in a bounded, resizable,
  scrollable table sorted by count. Whitespace-only input shows an information
  dialog.
- Both frontends default recent-file recording to enabled with a limit of 100.
  Verify MRU ordering and deduplication, lowering the cap, 0 as unlimited, the
  1,000,000 upper setting, typing a valid Qt limit before pressing OK, and
  disabling/re-enabling without changing the retained history.
- With a document already open, enabling the concurrent-editing guard must
  acquire its guard immediately and disabling it must release the owned guard.
  A pre-existing guard must be preserved, and newly opened Qt windows must see
  the changed preference without restarting Tpad.

GUI test scripts set `TPAD_CONFIG_FILE` and `TPAD_RECENT_FILE` to absolute
paths in their temporary directory. Tpad accepts these overrides specifically
so an automated test cannot read or overwrite the invoking account's normal
`~/.tpad.cfg` preferences or `~/.tpad.recent` history; relative override paths
are ignored.

Memcheck treats definite and indirect leaks in the instrumented core test as
failures. GUI, workflow, and print runs still use full leak reporting, but make
only invalid accesses fatal: GTK, Fontconfig, D-Bus, and Mesa retain large
process-global caches whose ownership is outside tpad. Those logs use the
distribution's official GLib/GTK suppressions and disable the accessibility
bridge; release reviewers must still check that no loss record is rooted in
tpad-owned allocation. ASan enables stack-use-after-return,
strict string checking, UBSan stack traces, and integrated leak detection for
the unit and fuzz targets. GUI and print ASan runs disable LSan because GTK,
Fontconfig, and D-Bus retain allocations in uninstrumented process-global
caches; Memcheck supplies the full GUI leak check instead. A narrow ASan
suppression covers a verified Cairo interceptor overread in `libcairo.so.2`;
it does not suppress errors originating in tpad. TSan GUI runs are omitted
because uninstrumented Pango's font thread produces an allocator race inside
GLib while active tpad sources contain no threads. TySan enables stack traces
and stops at its first strict-aliasing violation.

The scripts clear `DEBUGINFOD_URLS` while invoking sanitizers. This keeps the
required `llvm-symbolizer-23` local and prevents it from blocking on Ubuntu's
remote debuginfod service in an offline or constrained VM.

The fuzz target defaults to 60 seconds and a 4096-byte maximum input. Set
`TPAD_FUZZ_SECONDS` for a longer release run. Any reproducer is saved in the
audit's `fuzz-artifacts` directory.

## Packaging and release

Validate the source archive and Debian packaging after all source changes:

```sh
./autogen.sh
make -j"$(nproc)"
make check
make distcheck
dpkg-buildpackage -b -us -uc
```

Also run an out-of-tree compatibility build before requesting the multi-series
Launchpad recipe:

```sh
mkdir build-gtk3
cd build-gtk3
../configure --gtk3
make -j"$(getconf _NPROCESSORS_ONLN)"
make check
cd ..
```

Validate `snap/snapcraft.yaml` with a Snapcraft version that supports the
declared base. The current Core24 build uses Noble's
`libgtksourceview-5-0` and `libspelling-1-1` runtime package names. Core26
migration guidance is maintained in the official
[Snapcraft base migration documentation](https://ubuntu.com/docs/snapcraft/9/how-to/change-bases/change-from-core24-to-core26/).

Prepare the final release only after the audit is clean. Each argument after
the four-component upstream version becomes a Debian changelog bullet:

```sh
./prepare-release.sh 7.2.0.0 \
  "Add the cross-platform Qt 6 frontend." \
  "Add CMake platform and deployment options." \
  "Fix release-audit findings."
```

The script writes version `7.2.0.0` to Debian, Snap, `CMakeLists.txt`, and
`configure.ac`, creates a fresh changelog signature timestamp, runs
`autogen.sh`, and finishes with `make distclean`.

The Launchpad recipe has daily builds enabled, so a push normally creates its
all-series batch automatically. Check the recipe's build list for the pushed
revision before using **Request builds**. Never request the same revision
twice: the recipe's `{revtime}` version is identical, while a second native
source archive can have different bytes and will be rejected as a reused
version. If an API client reports an error after requesting builds, first
check whether new build records were created before retrying. Launchpad's
[upload-error documentation](https://documentation.ubuntu.com/launchpad/user/explanation/packaging/package-upload-errors/)
explains why archive versions cannot be reused.
