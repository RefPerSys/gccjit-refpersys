/****************************************************************
 * file gccjit-refpersys/persist.c
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *      It is implementing persistence (in a single textual file)
 *
 * Author(s):
 *      Basile Starynkevitch, France   <basile@starynkevitch.net>
 *      Abhishek Chakravarti, India    <abhishek@taranjali.org>
 *      Nimesh Neema, India            <nimeshneema@gmail.com>
 *
 *      © Copyright 2019 - 2024 The Reflective Persistent System Team
 *      team@refpersys.org & http://refpersys.org/
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

//// TODO: avoid any non-generated header file!

#define _GNU_SOURCE
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
#include <libgccjit.h>

extern gcc_jit_context *jitctx_RPS;
extern const char *progname_RPS;
extern const char *loadpath_RPS;
extern char **argv_RPS;
extern int argc_RPS;
extern char hostname_RPS[64];
extern struct backtrace_state *backtrace_state_RPS;
extern const char shortgitid_RPS[];
extern const char sourcedir_RPS[];
extern char full_source_main_RPS[];
extern int verbose_RPS;

extern double wallclock_real_time_RPS (void);
extern double monotonic_real_time_RPS (void);
extern double process_cpu_time_RPS (void);
extern double thread_cpu_time_RPS (void);

extern void
emit_gplv3_notice_AT_RPS (FILE *fout, const char *fil, int lin,
			  const char *fromfun, const char *path,
			  const char *linprefix, const char *linsuffix,
			  char *explain);

extern int32_t randomi32_RPS (void);
extern int64_t randomi64_RPS (void);

#define FATAL_AT_BIS(Fil,Lin,Func,Fmt,...) do {         \
    char thrname##Lin[32];                              \
    memset(thrname##Lin, 0, sizeof(thrname##Lin));      \
    pthread_getname_np(pthread_self(), thrname##Lin,    \
                       sizeof(thrname##Lin));           \
    fprintf (stderr, "%s:%d:%s [%s]", (Fil), (Lin),     \
             (Func), thrname##Lin);                     \
    fprintf (stderr, Fmt "\n", ##__VA_ARGS__);          \
    fprintf (stderr, "%s: shortgit %s pid %d\n",        \
             progname_RPS, shortgitid_RPS,              \
             (int)getpid());                            \
    fflush (stderr);                                    \
    if (backtrace_state_RPS)                            \
      backtrace_print (backtrace_state_RPS, 1, stderr); \
    fflush(NULL);                                       \
    abort(); } while(0)

#define FATAL_AT(Fil,Lin,Func,Fmt,...) \
  FATAL_AT_BIS(Fil,Lin,Func,Fmt,##__VA_ARGS__)

#define FATAL(Fmt,...) FATAL_AT(__FILE__,__LINE__,__FUNCTION__,Fmt,##__VA_ARGS__)


#warning TODO: define a simple and human readable syntax of the persistent file

void
load_state_RPS(const char*path, const void*start, const void*last)
{
  assert(path != NULL);
  assert(start != NULL);
  assert(last != NULL);
} /* end load_state_RPS */


void
write_state_RPS(const char*path)
{
#warning write_state_RPS is missing and needs a better signature
  FATAL("unimplemented write_state_RPS path=%s", path);
} /* end write_state_RPS */
