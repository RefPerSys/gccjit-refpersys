/****************************************************************
 * file gccjit-refpersys/main.c
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *      It is almost its main program.
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
 *    GNU General Public License for more details or the Lesser
 *    General Public License.
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

gcc_jit_context *jitctx_RPS;
const char *progname_RPS;
const char *loadpath_RPS;
const char *zlibv_RPS;
char **argv_RPS;
int argc_RPS;
char hostname_RPS[64];
struct backtrace_state *backtrace_state_RPS;
thread_local struct random_data random_data_RPS;
thread_local volatile atomic_flag random_inited_RPS;

char full_source_main_RPS[384];

#ifndef SHORTGITID
#error compilation command without SHORTGITID
#endif

const char shortgitid_RPS[] = SHORTGITID;


#ifndef SOURCEDIR
#error compilation command without SOURCEDIR
#endif

const char sourcedir_RPS[] = SOURCEDIR;

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

double
wallclock_real_time_RPS (void)
{
  struct timespec ts = { 0, 0 };
  if (clock_gettime (CLOCK_REALTIME, &ts))
    return NAN;
  return 1.0 * ts.tv_sec + 1.0e-9 * ts.tv_nsec;
}				/* end wallclock_real_time_RPS */

double
monotonic_real_time_RPS (void)
{
  struct timespec ts = { 0, 0 };
  if (clock_gettime (CLOCK_MONOTONIC, &ts))
    return NAN;
  return 1.0 * ts.tv_sec + 1.0e-9 * ts.tv_nsec;
}				// end monotonic_real_time_RPS


double
process_cpu_time_RPS (void)
{
  struct timespec ts = { 0, 0 };
  if (clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &ts))
    return NAN;
  return 1.0 * ts.tv_sec + 1.0e-9 * ts.tv_nsec;
}				// end process_cpu_time_RPS


double
thread_cpu_time_RPS (void)
{
  struct timespec ts = { 0, 0 };
  if (clock_gettime (CLOCK_THREAD_CPUTIME_ID, &ts))
    return NAN;
  return 1.0 * ts.tv_sec + 1.0e-9 * ts.tv_nsec;
}				// end thread_cpu_time_RPS


void
backtrace_error_RPS (void *data, const char *msg, int errnum)
{
  if (data)
    {
    };
  fprintf (stderr, "backtrace_error_RPS: %s (%d:%s)\n", msg, errnum,
	   strerror (errnum));
  fflush (NULL);
}				/* end backtrace_error_RPS */


void
emit_gplv3_notice_AT_RPS (FILE *fout, const char *fil, int lin,
			  const char *fromfun, const char *path,
			  const char *linprefix, const char *linsuffix,
			  char *explain)
{
  assert (fout);
  assert (path);
#define EMIT_PRINTF(Fmt,...) do {               \
    if (linprefix)                              \
      fputs(linprefix,fout);                    \
  fprintf(fout,Fmt,##__VA_ARGS__);              \
  if (linsuffix)                                \
    fputs(linsuffix,fout);                      \
  fputc('\n', fout);                            \
} while(0)
  time_t nowtime = time (NULL);
  struct tm nowtm = { };
  localtime_r (&nowtime, &nowtm);
  EMIT_PRINTF ("SPDX-License-Identifier: GPL-3.0-or-later");
  EMIT_PRINTF ("GENERATED [GPLv3+] file %s / DO NOT EDIT", path);
  if (explain)
    EMIT_PRINTF ("! %s !", explain);
  EMIT_PRINTF ("! generating git %s !", shortgitid_RPS);
  EMIT_PRINTF ("This %s file is generated by %s from %s:%d:%s",
	       path, progname_RPS, fil, lin, fromfun);
  EMIT_PRINTF ("© Copyright 2023 - %d - RefPerSys team",
	       (nowtm.tm_year + 1900));
  EMIT_PRINTF ("License: GPLv3+ (file COPYING-GPLv3)");
  EMIT_PRINTF ("see refpersys.org & github.com/RefPerSys/gccjit-refpersys");
  EMIT_PRINTF ("Contact team@refpersys.org and/or ...");
  EMIT_PRINTF ("Basile STARYNKEVITCH <basile@starynkevitch.net> (France)");
#undef EMIT_PRINTF
}				/* end emit_gplv3_copyright_notice_AT_RPS */

#define EMIT_GPLV3_COPYRIGHT_RPS(Fout,Path,Lpref,Lsuff,Explain) \
  emit_gplv3_notice_AT_RPS(Fout,                                \
                           __FILE__,__LINE__,__FUNCTION__,      \
                           (LPref),(Lsuff),(Explain))

int32_t
randomi32_RPS (void)
{
  if (__builtin_expect (!atomic_flag_test_and_set (&random_inited_RPS), true))
    {
      static thread_local char randbuf[128];
      memset (&random_data_RPS, 0, sizeof (random_data_RPS));
      if (getrandom (&randbuf, sizeof (randbuf), 0) <
	  (long) sizeof (randbuf) / 2)
	FATAL ("failed to initialize random (%s)", strerror (errno));
      unsigned seed = (unsigned) (time (NULL) + getpid ());
      initstate_r (seed, randbuf, sizeof (randbuf), &random_data_RPS);
    };
  int32_t res;
  int er = random_r (&random_data_RPS, &res);
  if (er)
    FATAL ("failed to call random_r (%s)", strerror (er));
  return res;
}				/* end randomi32_RPS */

int64_t
randomi64_RPS (void)
{
  int64_t x =
    ((unsigned long) randomi32_RPS () << 32) | ((unsigned long)
						randomi32_RPS ());
  return x;
}

int verbose_RPS;

const struct option progopt_RPS[] = {

  {.name = "verbose",.has_arg = no_argument,.flag = NULL,.val = 'V'},
  {.name = "version",.has_arg = no_argument,.flag = NULL,.val = 'v'},
  {.name = "help",.has_arg = no_argument,.flag = NULL,.val = 'H'},
  {.name = "load",.has_arg = required_argument,.flag = NULL,.val = 'l'},
  {.name = NULL,.has_arg = no_argument,.flag = NULL,.val = 0}
};				/* end progopt_RPS */



void
program_usage_RPS (void)
{
  printf ("%s usage:\n", progname_RPS);
  printf ("\t -V | --verbose         # verbose flag\n");
  printf ("\t -v | --version         # version info\n");
  printf ("\t -h | --help            # this help\n");
  printf ("\t -l | --load <topfile>  # load heap from TOPFILE\n");
  printf
    ("%s is GPLv3+ licensed, so WITHOUT WARRANTY; see www.gnu.org/licenses/gpl-3.0.html\n",
     progname_RPS);
  fflush (NULL);
}				/* end prog_usage_RPS */

void
parse_program_option_RPS (int argc, char **argv)
{
  //     extern char *optarg;
  //     extern int optind, opterr, optopt;
  int opt = -1;
  int ix = -1;
  do
    {
      opt = getopt_long (argc, argv, "Vvhl:", progopt_RPS, &ix);
      switch (opt)
	{
	case 0:
	  continue;
	case -1:
	  break;
	case 'V':		/* --verbose */
	  printf ("%s is verbose (pid %d on %s), GPLv3+ licensed\n",
		  progname_RPS, (int) getpid (), hostname_RPS);
	  verbose_RPS = 1;
	  break;
	case 'v':		/* --version */
	  printf ("%s version gitid %s built on %s\n", progname_RPS,
		  shortgitid_RPS, __DATE__ "@" __TIME__);
	  printf ("\t libunistring version: %d.%d.%d\n",
		  _libunistring_version >> 16,
		  0xff & (_libunistring_version >> 8),
		  _libunistring_version & 0xff);
	  printf ("\t GNU libc version: %s [%s]\n", gnu_get_libc_version (),
		  gnu_get_libc_release ());
	  printf ("\t libgccjit version: %d.%d.%d\n",
		  gcc_jit_version_major (), gcc_jit_version_minor (),
		  gcc_jit_version_patchlevel ());
	  printf
	    ("%s is GPLv3+ licensed, so WITHOUT WARRANTY; see www.gnu.org/licenses/gpl-3.0.html\n",
	     progname_RPS);
	  printf
	    ("\t Its source code could be on github.com/RefPerSys/gccjit-refpersys/\n");
	  break;
	case 'h':		/* --help */
	  program_usage_RPS ();
	  break;
	case 'l':
	  if (loadpath_RPS)
	    FATAL ("cannot load more than once (%s & %s)",
		   loadpath_RPS, optarg);
	  loadpath_RPS = optarg;
	  break;
	default:
	  break;
	};
    }
  while (opt >= 0);
}				/* end parse_program_option_RPS */


void
load_file_RPS (const char *ldpath)
{
  extern void load_state_RPS(const char*path, const void*start, const void*last);
  struct stat ldstat = { };
  int ldfd = open (ldpath, R_OK);
  if (ldfd < 0)
    FATAL ("%s failed to open loaded file %s (%s)",	//
	   progname_RPS, ldpath, strerror (errno));
  if (fstat (ldfd, &ldstat))
    FATAL ("%s failed to fstat loaded file %s (%s) fd#%d",	//
	   progname_RPS, ldpath, strerror (errno), ldfd);
  size_t ldsize = ldstat.st_size;
  size_t pgsize = getpagesize ();
  size_t memsize = (ldsize % pgsize) ? (1 + (ldsize | (pgsize - 1))) : ldsize;
  const void *ldad = mmap (NULL, memsize, PROT_READ,
#ifdef MAP_HUGETLB
			   MAP_HUGETLB |
#endif //MAP_HUGETLB
			   MAP_SHARED,
			   ldfd, 0);
  if (ldad == MAP_FAILED)
    FATAL ("%s failed to mmap fd#%d (%zd Kbytes) for loaded file %s (%s)",
	   progname_RPS, ldfd, memsize >> 10, ldpath, strerror (errno));
  const void*ldend = (const char*)ldad + memsize;
  if (verbose_RPS)
    {
      printf ("%s mmaped loaded file %s (fd#%d) for %zd Kbytes @%p-%p\n",
	      progname_RPS, ldpath, ldfd, memsize >> 10, ldad, ldend);
      fflush (NULL);
    };
  load_state_RPS(ldpath, ldad, ldend);
}				/* end load_file_RPS */


int
main (int argc, char **argv)
{
  assert (argc > 0);
  progname_RPS = argv[0];
  pthread_setname_np (pthread_self (), "main-rpsjit");
  gethostname (hostname_RPS, sizeof (hostname_RPS));
  parse_program_option_RPS (argc, argv);
  rl_initialize ();		/// initialize readline
  zlibv_RPS = zlibVersion ();
  jitctx_RPS = gcc_jit_context_acquire ();
  if (!jitctx_RPS)
    FATAL ("%s failed to create jitctx_RPS (%s)", progname_RPS,
	   strerror (errno));
  snprintf (full_source_main_RPS, sizeof (full_source_main_RPS), "%s/%s",
	    sourcedir_RPS, __FILE__);
  if (access (full_source_main_RPS, R_OK))
    FATAL ("%s failed to access full_source_main_RPS %s (%s)",
	   progname_RPS, full_source_main_RPS, strerror (errno));
  backtrace_state_RPS =
    backtrace_create_state ("/proc/self/exe", /*THREADED: */ 1,
			    backtrace_error_RPS, NULL);
  gcc_jit_context_release (jitctx_RPS);
  printf ("%s ending successfully (git %s) on %s source in %s\n",
	  progname_RPS, shortgitid_RPS, hostname_RPS, full_source_main_RPS);
  return 0;
}				/* end main */
