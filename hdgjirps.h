/****************************************************************
 * file gccjit-refpersys/hdgjirps.h
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System
 *      (the gccjit-refpersys variant which might be accepted in India)
 *      It is its header file
 *
 * Author(s):
 *      Basile Starynkevitch, France   <basile@starynkevitch.net>
 *
 *      © Copyright (C) 2026 Basile Starynkevitch
 *
 * You can consider RefPerSys as either GPLv3+ or LGPLv3+ licensed (at
 * your choice)
 *
 * License: GPLv3+ (file COPYING-GPLv3)
 *    This software is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#define _GNU_SOURCE

#ifndef HDGJIRPS_INCLUDED
#define HDGJIRPS_INCLUDED

#ifndef SHORTGITID
#error compilation command without SHORTGITID
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <zlib.h>
#include <unistr.h>
#include <pthread.h>
#include <threads.h>
#include <backtrace.h>
#include <readline/readline.h>
#include <unistring/version.h>
#include <gnu/libc-version.h>
#include "jemalloc/jemalloc.h"	/* see jemalloc.net */
#include <bsd/string.h>		/* for strnstr(3) */
#include <string.h>

#include <libgccjit.h> /* see gcc.gnu.org/onlinedocs/jit/ */

extern gcc_jit_context *jitctx_HDGJIRPS;
extern const char *progname_HDGJIRPS;
extern const char *loadpath_HDGJIRPS;
extern char **argv_HDGJIRPS;
extern int argc_HDGJIRPS;
extern char hostname_HDGJIRPS[64];
extern const char* zlibv_HDGJIRPS;
extern struct backtrace_state *backtrace_state_HDGJIRPS;
extern const char shortgitid_HDGJIRPS[];
extern const char sourcedir_HDGJIRPS[];
extern char full_source_main_HDGJIRPS[];
extern char executable_HDGJIRPS[128];
extern int verbose_HDGJIRPS;
extern pthread_mutex_t globmtx_HDGJIRPS;

extern double wallclock_real_time_HDGJIRPS (void);
extern double monotonic_real_time_HDGJIRPS (void);
extern double process_cpu_time_HDGJIRPS (void);
extern double thread_cpu_time_HDGJIRPS (void);

extern void
emit_gplv3_notice_AT_HDGJIRPS (FILE *fout, const char *fil, int lin,
			  const char *fromfun, const char *path,
			  const char *linprefix, const char *linsuffix,
			  char *explain);

extern int32_t randomi32_HDGJIRPS (void);
extern int64_t randomi64_HDGJIRPS (void);

#define HDGJIRPS_FATAL_AT_BIS(Fil,Lin,Func,Fmt,...) do {	\
    char thrname##Lin[32];					\
    memset(thrname##Lin, 0, sizeof(thrname##Lin));		\
    pthread_getname_np(pthread_self(), thrname##Lin,		\
                       sizeof(thrname##Lin));			\
    fprintf (stderr, "%s:%d:%s [%s]", (Fil), (Lin),		\
             (Func), thrname##Lin);				\
    fprintf (stderr, "FATAL ERROR\n");				\
    fprintf (stderr, Fmt "\n", ##__VA_ARGS__);			\
    fprintf (stderr, "%s: shortgit %s pid %d\n",		\
             progname_HDGJIRPS, shortgitid_HDGJIRPS,		\
             (int)getpid());					\
    fflush (stderr);						\
    if (backtrace_state_HDGJIRPS)				\
      backtrace_print (backtrace_state_HDGJIRPS, 1,		\
		       stderr);					\
    fflush(NULL);						\
    abort(); } while(0)

#define HDGJIRPS_FATAL_AT(Fil,Lin,Func,Fmt,...) \
   HDGJIRPS_FATAL_AT_BIS(Fil,Lin,Func,Fmt,##__VA_ARGS__)

#define  HDGJIRPS_FATAL(Fmt,...)  HDGJIRPS_FATAL_AT(__FILE__,__LINE__,__FUNCTION__,Fmt,##__VA_ARGS__)

#define HDGJIRPS_HEADER_FIELDS \
  uint16_t typenum;	       \
  uint8_t  gcmark;	       \
  uint8_t  flag;	       \
  uint32_t numval

#endif /*HDGJIRPS_INCLUDED*/
/*end of file */
