dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(tpad, 7.3.0.6,https://bugs.launchpad.net/tpad-project/+filebug)

dnl Autoconf normally requires --enable-gtk3.  Keep that conventional spelling
dnl while also accepting the shorter --gtk3 requested by the packaging recipe.
dnl This diversion runs before Autoconf parses its command-line options.
m4_divert_text([DEFAULTS], [
tpad_arg_count=$#
while test "$tpad_arg_count" -gt 0; do
  tpad_arg=$1
  shift
  case $tpad_arg in
    --gtk3) set -- "$@" --enable-gtk3 ;;
    *) set -- "$@" "$tpad_arg" ;;
  esac
  tpad_arg_count=$((tpad_arg_count - 1))
done
])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])
AC_COPYRIGHT([Copyright (C) 2012,2013,2014,2015,2016,2017,2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025, 2026 Andrew Smith (GNA SERVICES INC) <andresmi at icloud dot com>.])
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
AC_PROG_GREP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

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
   if $MSGFMT --version 2>&1 | $GREP GNU > /dev/null; then
      GNU_GETTEXT=yes
      MSGFMTFLAGS='-c'
   fi
   AC_MSG_RESULT($GNU_GETTEXT)
fi
AC_SUBST(MSGFMTFLAGS)

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug],
    [enable printing more debugging info])
)
AS_IF([test "x$enable_debug" = "xyes"], [
            AC_DEFINE([DEBUG_TOGGLE], [""],
            [Enable debug toggle])
])

dnl Check for typedefs, structures, and compiler characteristics ----------

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

AC_ARG_ENABLE([gtk3],
  AS_HELP_STRING([--gtk3],
    [build the legacy GTK 3 frontend (also accepts --enable-gtk3)]),
  [AS_CASE([$enableval],
    [yes|no], [enable_gtk3=$enableval],
    [AC_MSG_ERROR([--enable-gtk3 accepts only yes or no])])],
  [enable_gtk3=auto])

AS_CASE([$enable_gtk3],
  [yes], [tpad_use_gtk3=yes],
  [no], [tpad_use_gtk3=no],
  [auto], [
    PKG_CHECK_EXISTS([gtk4 >= 4.10 gtksourceview-5 libspelling-1],
      [tpad_use_gtk3=no], [tpad_use_gtk3=yes])
  ])

AS_IF([test "x$tpad_use_gtk3" = "xyes"], [
  PKG_CHECK_MODULES([TPAD],
    [gtk+-3.0 gtksourceview-3.0 gtkspell3-3.0])
  TPAD_FRONTEND_CPPFLAGS="-DTPAD_USE_GTK3=1"
  AC_DEFINE([TPAD_USE_GTK3], [1],
    [Define when building the legacy GTK 3 frontend])
  AC_MSG_NOTICE([Tpad frontend: GTK 3])
], [
  PKG_CHECK_MODULES([TPAD],
    [gtk4 >= 4.10 gtksourceview-5 libspelling-1])
  TPAD_FRONTEND_CPPFLAGS=
  AC_MSG_NOTICE([Tpad frontend: GTK 4])
])
AC_SUBST([TPAD_FRONTEND_CPPFLAGS])
AM_CONDITIONAL([USE_GTK3], [test "x$tpad_use_gtk3" = "xyes"])

dnl Ubuntu Trusty provides the sd-id128 API as a split pkg-config module,
dnl while newer Ubuntu releases provide it through the unified libsystemd.
PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 233], [
  AC_DEFINE([HAVE_SD_ID128_GET_MACHINE_APP_SPECIFIC], [1],
            [Define when libsystemd provides app-specific machine IDs])
], [
  PKG_CHECK_MODULES([SYSTEMD], [libsystemd], [], [
    PKG_CHECK_MODULES([SYSTEMD], [libsystemd-id128])
  ])
])


AC_CONFIG_FILES([Makefile
src/Makefile
tests/Makefile
po/Makefile.in
icon/Makefile
man/Makefile
script/Makefile
debian/Makefile
m4/Makefile])

AC_OUTPUT
