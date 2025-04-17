#ifndef _MALLOC_H
#define _MALLOC_H
#if HAVE_CONFIG_H
# include <config.h>
#endif
#undef malloc
#include <sys/types.h>
void *malloc ();
#endif