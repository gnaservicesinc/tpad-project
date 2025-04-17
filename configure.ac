dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(tpad, 7.0.0.3-1,https://bugs.launchpad.net/tpad-project/+filebug)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AC_COPYRIGHT([Copyright (C) 2012,2013,2014,2015,2016,2017,2018, 2019, 2020, 2021, 2024 Andrew Smith (GNA SERVICES INC) <andresmi at icloud dot com>.])
AC_MSG_NOTICE([Configuring tpad ]AC_PACKAGE_VERSION)
AC_CONFIG_MACRO_DIR([m4])


AM_SILENT_RULES([yes])
AC_SUBST([abs_srcdir], ["`cd $srcdir && pwd`"])
AC_SUBST([abs_builddir], ["`pwd`"])
if test "$abs_srcdir" = "$abs_builddir"; then
  canonicalized_srcdir=""
else
  canonicalized_srcdir="$srcdir/"
fi
AC_SUBST([canonicalized_srcdir])

SWIG_LDFLAGS="$LDFLAGS"
AC_SUBST([SWIG_LDFLAGS])


AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_C_INLINE
AC_TYPE_SIZE_T

AC_CHECK_HEADERS([libintl.h stdlib.h string.h syslog.h unistd.h wchar.h sys/time.h])
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strrchr pathconf realpath strerror])
AC_PATH_PROG(PKG_CONFIG, pkg-config)

# Look for a good sed
# AC_PROG_SED was introduced in Autoconf 2.59b
m4_ifdef([AC_PROG_SED], [AC_PROG_SED], [SED="${SED:-sed}"])
dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=tpad
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])

AM_GLIB_GNU_GETTEXT

AC_HEADER_ASSERT
AC_ARG_ENABLE([autotab],
  AS_HELP_STRING([--enable-autotab],
    [enable a menu option to toggle auto indent])
)
AS_IF([test "x$enable_autotab" = "xyes"], [
            AC_DEFINE([AUTO_TAB_TOGGLE], [""],
            [Enable autotab toggle])
])

dnl Used to simulate makefile conditionals.
GETTEXT_CODESET=\#
NO_GETTEXT_CODESET=\#
if test $USE_NLS = "yes"; then
   AC_CHECK_FUNCS(bind_textdomain_codeset,
                  [ GETTEXT_CODESET="" ],
                  [ NO_GETTEXT_CODESET="" ])
fi
AC_SUBST(GETTEXT_CODESET)
AC_SUBST(NO_GETTEXT_CODESET)

# Check if we are using GNU gettext.
GNU_GETTEXT=no
MSGFMTFLAGS=''
if test $USE_NLS = "yes"; then
   AC_MSG_CHECKING(if we are using GNU gettext)
   if $MSGFMT --version 2>&1 | $EGREP GNU > /dev/null; then
      GNU_GETTEXT=yes
      MSGFMTFLAGS='-c'
   fi
   AC_MSG_RESULT($GNU_GETTEXT)
fi
AC_SUBST(MSGFMTFLAGS)

dnl libmagic -------------------

libmagic_found=no

AC_ARG_WITH(libmagic,AS_HELP_STRING([--with-libmagic=PREFIX],
                                [libmagic filetype detection library]),
[
  if test "$withval" = "yes" ; then
    AC_CHECK_HEADER(magic.h, [
      AC_CHECK_LIB(magic, magic_open, [libmagic_found="builtin"])
    ])
    libmagic_prefix="the default locations"
  elif test "$withval" != "no"; then
    libmagic_prefix=$withval
    save_cflags="$CFLAGS"
    CFLAGS="$CFLAGS -I$libmagic_prefix/include"
    AC_CHECK_HEADERS(magic.h,[
      save_ldflags="$LDFLAGS"
      LDFLAGS="-L$libmagic_prefix/lib"
-lmagic
      AC_CHECK_LIB(magic, magic_open, [libmagic_found="yes"])
      LDFLAGS="$save_ldflags"
    ])
  fi
  if test "$withval" != "no" && test "$libmagic_found" = "no"; then
    AC_MSG_ERROR([[--with-libmagic requested, but libmagic not found at $libmagic_prefix]])
  fi
],
[
  AC_CHECK_HEADER(magic.h, [
    AC_CHECK_LIB(magic, magic_open, [libmagic_found="builtin"])
  ])
])

if test "$libmagic_found" != "no"; then
  AC_DEFINE([HAVE_LIBMAGIC], [1], [Defined if libmagic support is enabled])
  MAGIC_LIBS="-lmagic"
fi

if test "$libmagic_found" = "yes"; then
 MAGIC_INCLUDES="-I$libmagic_prefix/include"
  LDFLAGS="$LDFLAGS `REMOVE_STANDARD_LIB_DIRS(-L$libmagic_prefix/lib)`"

fi

AC_SUBST(MAGIC_INCLUDES)
AC_SUBST(MAGIC_LIBS)

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug],
    [enable printing more debugging info])
)
AS_IF([test "x$enable_debug" = "xyes"], [
            AC_DEFINE([DEBUG_TOGGLE], [""],
            [Enable debug toggle])
])

dnl Standard C headers
AC_HEADER_STDC

dnl Check for typedefs, structures, and compiler characteristics ----------

dnl if compiler doesn't understand `const', then define it empty
AC_C_CONST

dnl if non-existent, define size_t to be `unsigned'
AC_TYPE_SIZE_T


dnl Check for library functions ----------

AC_FUNC_MEMCMP

dnl svn_error's default warning handler uses vfprintf()
AC_FUNC_VPRINTF

dnl check for functions needed in special file handling
AC_CHECK_FUNCS(symlink readlink)

dnl check for uname
AC_CHECK_HEADERS(sys/utsname.h, [AC_CHECK_FUNCS(uname)], [])

dnl Since libunity is not actually implimented in code
dnl Removed from configure.ac so not shown in configure
dnl AC_ARG_ENABLE(libunity, AS_HELP_STRING([--disable-libunity],[compile without libunity support]),,enable_libunity=yes)

dnl if test "x$enable_libunity" = "xyes"; then
dnl    PKG_CHECK_MODULES([LIBUNITY], unity >= 3.4.2,enable_libunity=yes,enable_libunity=no)
dnl    AC_SUBST(LIBUNITY_CFLAGS)
 dnl   AC_SUBST(LIBUNITY_LIBS)

dnl  else
dnl   enable_libunity=no
dnl fi

dnl if test "x$enable_libunity" = "xyes"; then
dnl   AC_DEFINE(HAVE_LIBUNITY, 1, [Define if libunity support is enabled])
dnl fi

dnl AM_CONDITIONAL(WITH_LIBUNITY, test "x$enable_libunity" = "xyes")

LT_INIT



PKG_CHECK_MODULES(TPAD, [ gucharmap-2.90 pixman-1 gtk+-3.0 gdk-pixbuf-2.0 gtksourceview-3.0 uuid gtkspell3-3.0 pango gdk-3.0 pangocairo cairo-gobject glib-2.0 cairo freetype2 atk gtk+-3.0 gio-2.0 icu-i18n libsystemd ])


AC_CONFIG_FILES([Makefile
src/Makefile
po/Makefile.in
icon/Makefile
man/Makefile
script/Makefile
debian/Makefile
m4/Makefile])

AC_OUTPUT
