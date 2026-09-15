# Building Tpad

Tpad has three user-interface configurations:

- GTK 4 is the current Linux and Unix desktop frontend. It can be built
  with Autotools or CMake.
- GTK 3 is the compatibility frontend for Ubuntu Trusty, Xenial, Bionic,
  Focal, and Jammy. It can be built with Autotools or CMake.
- Qt 6 is the cross-platform frontend for macOS, Windows, and Linux. The
  supported reference kit is Qt 6.11.1 and it is built with CMake.

Only one frontend is compiled into a binary. Tpad's Qt target does not compile
or directly link GTK, GtkSourceView, libspelling, GLib, or systemd; a Linux Qt
package can still have its own platform-integration dependencies. The GTK
target does not compile or link Qt.

Always use a separate build directory for each frontend, Qt installation, and
build configuration. CMake caches the selected Qt installation, so remove the
build directory before changing `TPAD_QT_PATH` between shared and static Qt
kits.

## CMake options

The root `CMakeLists.txt` provides these frontend and deployment options:

| Option | Default | Purpose |
| --- | --- | --- |
| `TPAD_USE_QT` | On for macOS/Windows; off elsewhere | Build the Qt frontend when on, or a GTK frontend when off. |
| `TPAD_USE_GTK3` | Off | Build the legacy GTK 3 frontend instead of GTK 4; mutually exclusive with `TPAD_USE_QT`. |
| `TPAD_QT_PATH` | Empty | Qt installation prefix, such as `/opt/Qt/6.11.1/macos` or `C:/Qt/6.11.1/msvc2022_64`. |
| `TPAD_QT_STATIC` | Off | Require the selected Qt installation to be a static build. |
| `TPAD_QT_DEPLOY` | On for macOS/Windows; off elsewhere | During installation of a shared-Qt build, copy the required Qt runtime libraries and plugins. |

`TPAD_QT_STATIC` selects and verifies a Qt kit; it cannot turn a shared Qt
installation into a static one. To use it, first build and install a separate
static Qt 6.11.1 kit, then point `TPAD_QT_PATH` at that prefix. Tpad uses
Qt's `qt_add_executable()` so the required static platform and print-support
plugins are linked and initialized by Qt's CMake integration.

If `TPAD_QT_PATH` is empty, CMake uses its normal Qt package search. Running
the kit's `qt-cmake` or `qt-cmake.bat` is another supported way to supply the
Qt prefix.

## Linux and other Unix desktops: GTK

The GTK frontend requires a C99 compiler and these development components:

- GTK 4.10 or newer
- GtkSourceView 5
- libspelling
- GLib/GIO and GNU gettext
- libsystemd, or the older split libsystemd-id128 package
- pkg-config

For a Debian or Ubuntu developer checkout, install the build tools and
libraries with:

```sh
sudo apt-get install \
  build-essential autoconf automake cmake gettext intltool libtool ninja-build \
  pkg-config libgtk-4-dev libgtksourceview-5-dev libspelling-1-dev \
  libsystemd-dev
```

### GTK with Autotools

Autotools remains the build used by the Debian, Ubuntu, Snap, and Launchpad
packaging:

```sh
./autogen.sh --prefix="$PWD/.local"
make -j"$(getconf _NPROCESSORS_ONLN)"
make check
make install
```

Configure selects GTK 4 when its complete dependency stack is available and
otherwise falls back to GTK 3. To force the compatibility frontend, install
`libgtk-3-dev`, `libgtksourceview-3.0-dev`, and `libgtkspell3-3-dev`, then use
the literal `--gtk3` flag:

```sh
./autogen.sh --gtk3 --prefix="$PWD/.local-gtk3"
make -j"$(getconf _NPROCESSORS_ONLN)"
make check
```

The conventional Autoconf spelling `--enable-gtk3` is accepted as well.
Launchpad packaging passes `--gtk3` for Trusty through Jammy and keeps GTK 4
as the default for newer series.

From a release archive, `configure` is already generated, so begin with
`./configure` instead of `./autogen.sh`. Use
`NOCONFIGURE=1 ./autogen.sh` when only regenerating the checked-in Autotools
files.

### GTK with CMake

```sh
cmake -S . -B build-gtk -G Ninja \
  -DTPAD_USE_QT=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/stage-gtk"
cmake --build build-gtk --parallel
cmake --install build-gtk
```

For a GTK 3 compatibility build, add `-DTPAD_USE_GTK3=ON` and use a separate
build directory:

```sh
cmake -S . -B build-gtk3 -G Ninja \
  -DTPAD_USE_QT=OFF \
  -DTPAD_USE_GTK3=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-gtk3 --parallel
```

The installed binary is `stage-gtk/bin/tpad`. The CMake GTK frontend is
supported on Linux and other Unix desktops; use Qt on macOS and Windows.

## macOS: Qt 6.11.1

The bundle and install flow follows Qt's official
[macOS deployment guidance](https://doc.qt.io/qt-6/macos-deployment.html).
The root project therefore requires CMake 3.21.1 on Darwin (and uses the same
minimum everywhere so one configuration remains easy to audit).

Prerequisites:

- macOS 13 or newer
- Xcode 15 with the macOS 14 SDK, or newer, plus its command-line tools
  (`xcode-select --install`)
- CMake 3.21.1 or newer
- Ninja, Xcode, or another CMake generator
- the Qt 6.11.1 macOS desktop kit

Install Qt 6.11.1 with the Qt Online Installer, or provide an equivalent Qt
build. The examples below use `/opt/Qt/6.11.1/macos`; replace it with the
actual kit location, commonly `$HOME/Qt/6.11.1/macos`.

### Shared Qt application bundle

The normal Qt installer provides a shared-framework kit. Configure, build,
and create a self-contained application bundle with:

```sh
cmake -S . -B build-macos -G Ninja \
  -DTPAD_USE_QT=ON \
  -DTPAD_QT_PATH=/opt/Qt/6.11.1/macos \
  -DTPAD_QT_STATIC=OFF \
  -DTPAD_QT_DEPLOY=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/stage-macos"
cmake --build build-macos --parallel
cmake --install build-macos
open stage-macos/tpad.app
```

When `TPAD_QT_DEPLOY=ON`, the build step runs Qt's `macdeployqt` tool and
verifies that the build-tree bundle contains Qt Core, Gui, Widgets,
PrintSupport, and the Cocoa platform plugin. The install step copies that
self-contained bundle and runs Qt's install-time deployment support as a
second packaging check. Both `build-macos/tpad.app` and
`stage-macos/tpad.app` are therefore launchable, while the staged bundle is
the one to distribute.

Confirm the framework that provides `QPrinter` and `QPrintDialog`, the Cocoa
platform plugin, and the executable's dynamic dependencies with:

```sh
test -f build-macos/tpad.app/Contents/Frameworks/QtPrintSupport.framework/Versions/A/QtPrintSupport
test -f build-macos/tpad.app/Contents/PlugIns/platforms/libqcocoa.dylib
otool -L stage-macos/tpad.app/Contents/MacOS/tpad
```

For local development against an installed Qt, deployment can be disabled
with `-DTPAD_QT_DEPLOY=OFF`. Distribution outside the local machine also
requires the appropriate Apple code-signing and notarization steps; those
credentials are intentionally not part of this basic build configuration.

To build a universal application, add the architectures supported by the Qt
kit, for example `-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`. Every library in
the selected Qt kit must contain the same architectures.

### Static Qt application bundle

The shared macOS kit cannot be statically linked. Building Qt 6.11.1 itself
requires CMake 3.22 or newer and Ninja on `PATH`; Qt's source configuration
uses Ninja even when Tpad itself is built with another generator. Build it
from source into a separate prefix first:

```sh
mkdir build-qt-macos-static
cd build-qt-macos-static
/path/to/qt-everywhere-src-6.11.1/configure \
  -prefix /opt/Qt/6.11.1/macos-static \
  -static -release -opensource -confirm-license \
  -nomake examples -nomake tests
cmake --build . --parallel
cmake --install .
cd ..
```

Then configure Tpad against that static kit:

```sh
cmake -S . -B build-macos-static -G Ninja \
  -DTPAD_USE_QT=ON \
  -DTPAD_QT_PATH=/opt/Qt/6.11.1/macos-static \
  -DTPAD_QT_STATIC=ON \
  -DTPAD_QT_DEPLOY=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-macos-static --parallel
open build-macos-static/tpad.app
```

Qt and every required third-party dependency must have been built for static
linking. `TPAD_QT_STATIC=ON` stops configuration with a clear error when the
path selects a shared Qt installation.

## Windows: Qt 6.11.1

The executable, shared-DLL deployment, and static-kit flow follow Qt's
[Windows deployment guidance](https://doc.qt.io/qt-6/windows-deployment.html),
including its instructions for
[static Qt applications](https://doc.qt.io/qt-6/windows-deployment.html#linking-the-application-to-the-static-version-of).

Prerequisites:

- Windows 10 or Windows 11
- Visual Studio 2022 with **Desktop development with C++**, or the compiler
  matching the chosen Qt kit
- CMake and either Ninja or the Visual Studio generator
- a Qt 6.11.1 desktop kit for the same compiler and architecture

The examples use the 64-bit MSVC 2022 kit at
`C:\Qt\6.11.1\msvc2022_64`. Run them from the matching **x64 Native Tools
Command Prompt for VS 2022**, or initialize the Qt environment with the kit's
`bin\qtenv2.bat`. Do not mix MSVC and MinGW kits or 32-bit and 64-bit tools.

### Shared Qt executable and deployment directory

In PowerShell, a Visual Studio generator build is:

```powershell
cmake -S . -B build-windows -G "Visual Studio 17 2022" -A x64 `
  -DTPAD_USE_QT=ON `
  -DTPAD_QT_PATH=C:/Qt/6.11.1/msvc2022_64 `
  -DTPAD_QT_STATIC=OFF `
  -DTPAD_QT_DEPLOY=ON `
  -DCMAKE_INSTALL_PREFIX="$PWD/stage-windows"
cmake --build build-windows --config Release --parallel
cmake --install build-windows --config Release
& "$PWD/stage-windows/bin/tpad.exe"
```

With `TPAD_QT_DEPLOY=ON`, the install step performs Qt's Windows deployment
and copies the required Qt DLLs, compiler runtime, and plugins beside the
application. The resulting `stage-windows` tree is the directory to test on a
Windows machine without Qt installed. The equivalent manual deployment tool
is:

```powershell
C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe `
  --release .\build-windows\Release\tpad.exe
```

Use either the CMake install deployment or a deliberate manual
`windeployqt` workflow; a bare `.exe` from a shared build is not independently
deployable.

For a single-configuration Ninja build, replace the generator options with
`-G Ninja -DCMAKE_BUILD_TYPE=Release` and omit `--config Release` from the
build and install commands.

### Static Qt executable

The normal Online Installer kit is shared. Building Qt 6.11.1 itself requires
CMake 3.22 or newer, Ninja, and Python 3, with `cmake.exe`, `ninja.exe`, and
`python.exe` all on `PATH`. From an x64 Visual Studio tools prompt, build and
install a separate static Qt 6.11.1 kit:

```bat
mkdir build-qt-windows-static
cd build-qt-windows-static
C:\src\qt-everywhere-src-6.11.1\configure.bat ^
  -prefix C:\Qt\6.11.1\msvc2022_64_static ^
  -static -release -opensource -confirm-license ^
  -nomake examples -nomake tests
cmake --build . --config Release --parallel
cmake --install . --config Release
cd ..
```

Build Tpad with the same compiler and runtime configuration:

```powershell
cmake -S . -B build-windows-static -G "Visual Studio 17 2022" -A x64 `
  -DTPAD_USE_QT=ON `
  -DTPAD_QT_PATH=C:/Qt/6.11.1/msvc2022_64_static `
  -DTPAD_QT_STATIC=ON `
  -DTPAD_QT_DEPLOY=OFF
cmake --build build-windows-static --config Release --parallel
```

Static Qt removes the Qt DLL deployment step, but compiler-specific runtime
and operating-system dependencies still apply. Verify the result on a clean
system. Building or distributing Qt, especially as static libraries, remains
subject to the license terms of the Qt edition and third-party components you
use.

## Troubleshooting Qt discovery

- Confirm the selected kit with
  `<qt-prefix>/bin/qtpaths --qt-version` (`qtpaths.exe` on Windows).
- Prefer forward slashes in paths passed through `-D` on Windows.
- If CMake reports a different Qt after changing `TPAD_QT_PATH`, remove the
  build directory and configure again.
- A `TPAD_QT_STATIC=ON` error means the selected kit contains shared Qt
  libraries. Point it at a Qt installation configured with `-static`.
- If macOS reports a missing `@rpath/Qt*.framework`, confirm that
  `TPAD_QT_DEPLOY=ON`, remove any reused build directory, and configure and
  build again. A deployment-enabled build now stops instead of producing a
  bundle without the required Qt frameworks and Cocoa plugin.
- With the Xcode generator, pass `--config Release` to both `cmake --build`
  and `cmake --install` so the same configuration is built and staged.
