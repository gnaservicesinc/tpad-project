# tpad - Text Editor

tpad is a simple text editor built with GTK and GtkSourceView.

## GTK-4 Migration

This codebase has been updated to use GTK-4 and GtkSourceView-5 instead of the original GTK-3 and GtkSourceView-3. The migration involved several key changes:

1. Updated build system to use GTK-4 and GtkSourceView-5
2. Updated UI creation to use GTK-4 APIs
3. Updated event handling for GTK-4
4. Updated container APIs to use GTK-4 methods
5. Updated menu system to use GMenu and GtkPopoverMenu
6. Updated spell checking to use libspelling instead of gtkspell3

## Building tpad

To build tpad, you need the following dependencies:

- GTK-4
- GtkSourceView-5
- libspelling-1 (for spell checking)
- Other standard libraries (see configure.ac)

### Build Steps

```bash
# Generate build files
./autogen.sh

# Configure the build
./configure

# Build the application
make

# Install (optional)
sudo make install
```

## Known Issues

The GTK-4 migration is a work in progress. Some features may not work correctly:

1. Some UI elements may not appear correctly due to GTK-4's different layout system
2. The spell checking implementation using libspelling is new and may have some issues
3. Package naming conventions for GTK-4 and related libraries may vary across different Ubuntu versions

## Package Naming Notes

The configure.ac file includes fallback options for different package naming conventions:

- GTK-4 may be named `gtk4` or `gtk-4.0` depending on the distribution
- GTKSourceView-5 may be named `gtksourceview-5` or `gtksourceview-5.0`
- libspelling may be named `libspelling-1` or `spelling-1`

If you encounter build issues related to package names, you may need to adjust the package names in configure.ac to match your system.

## License

tpad is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
