# Tpad

Tpad is a small, fast text editor for plain text and source files. The current
application uses GTK 3, GtkSourceView 3, and gtkspell3. Each command-line file
opens in its own editor window.

The File menu includes a Print action (`Ctrl+P`) that opens GTK's native system
print dialog. Printer selection, print-to-file, page setup, ranges, and other
options come from the desktop print service.

## Building

On Debian or Ubuntu, install the normal Autotools toolchain plus these project
development packages:

- `libgtk-3-dev`
- `libgtksourceview-3.0-dev`
- `libgtkspell3-3-dev`
- `libsystemd-dev`
- `gettext`, `intltool`, `libtool`, and `pkg-config`

Then build and test from the repository root:

```sh
./autogen.sh
make -j"$(nproc)"
make check
```

`./autogen.sh` regenerates the Autotools files and runs `configure`. Use
`NOCONFIGURE=1 ./autogen.sh` when only the generated build files are needed.

Tpad keeps its small, embedded mbedTLS hashing subset as separate source and
header files under `src/`; its provenance and license are documented in
`src/mbedtls/README.tpad` and `src/mbedtls/LICENSE`.

## Release testing and packaging

The repeatable clang-23 sanitizer, static-analyzer, Valgrind, fuzzing, GUI, and
packaging procedure is in [RELEASE_TESTING.md](RELEASE_TESTING.md). Debian
metadata is under `debian/`, and the strict Core 26 Snap recipe is
`snap/snapcraft.yaml`.

Report bugs at <https://bugs.launchpad.net/tpad-project/+filebug>.

## License

Tpad is distributed under the GNU General Public License, version 3 or later.
See `COPYING` and `debian/copyright` for the complete licensing and bundled-code
notices.
