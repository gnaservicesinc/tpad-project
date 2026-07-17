/* Lightweight malloc wrapper header compatible with modern libc */
#ifndef TPAD_MALLOC_H
#define TPAD_MALLOC_H

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stddef.h>

/*
 * If configure decides to replace malloc, it will define
 *   #define malloc rpl_malloc
 * in config.h. We only need to provide the prototype for rpl_malloc.
 * Do NOT redeclare the system malloc here to avoid conflicting types.
 */
void *rpl_malloc (size_t n);

#endif /* TPAD_MALLOC_H */
