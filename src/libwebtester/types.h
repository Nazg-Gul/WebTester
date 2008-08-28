/*
 *
 * ================================================================================
 *  types.h - part of the WebTester Server
 * ================================================================================
 *
 *  Written (by Nazgul) under General Public License.
 *
*/

#ifndef _wt_types_h_
#define _wt_types_h_

#include <sys/types.h>

#ifndef BOOL

#  define BOOL int

#  ifndef FALSE
#    define FALSE 0
#  endif

#  ifndef TRUE
#    define TRUE 1
#  endif

#endif

#ifndef DWORD
#  define DWORD unsigned long long
#endif

typedef unsigned int       __u16_t;
typedef unsigned long      __u32_t;
typedef unsigned long long __u64_t;

typedef struct timeval timeval_t;

#endif
