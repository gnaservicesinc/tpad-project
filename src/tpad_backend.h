/*
 * Compile-time frontend selection.  CMake gives the GTK and Qt targets
 * disjoint source lists in addition to these guards, so toolkit headers and
 * objects can never leak into the other frontend.
 */
#ifndef TPAD_BACKEND_H
#define TPAD_BACKEND_H

#if defined(TPAD_USE_QT) && defined(TPAD_USE_GTK)
# error "Tpad cannot build the Qt and GTK frontends into the same executable"
#elif !defined(TPAD_USE_QT) && !defined(TPAD_USE_GTK)
/* The historical Autotools build is GTK-only. */
# if defined(HAVE_CONFIG_H)
#  define TPAD_USE_GTK 1
# else
#  error "Define exactly one of TPAD_USE_QT or TPAD_USE_GTK"
# endif
#endif

#endif
