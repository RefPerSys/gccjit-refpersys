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

#include <stdlib.h>
#include <readline/readline.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <zlib.h>
#include <libgccjit.h>
gcc_jit_context *jitctx_RPS;
const char *progname_RPS;
const char *zlibv_RPS;

#ifndef SHORTGITID
#error compilation command without SHORTGITID
#endif

const char shortgitid_RPS[] = SHORTGITID;


#ifndef SOURCEDIR
#error compilation command without SOURCEDIR
#endif

const char sourcedir_RPS[] = SOURCEDIR;

#define FATAL_AT_BIS(Fil,Lin,Func,Fmt,...) do {         \
    fprintf(stderr, "%s:%d:%s ", (Fil), (Lin), (Func)); \
    fprintf(stderr, Fmt "\n", ##__VA_ARGS__);           \
    fprintf(stderr, "%s: shortgit %s\n",                \
            progname_RPS, shortgitid_RPS);              \
    fflush(NULL);                                       \
    abort(); } while(0)

#define FATAL_AT(Fil,Lin,Func,Fmt,...) \
  FATAL_AT_BIS(Fil,Lin,Func,Fmt,##__VA_ARGS__)

#define FATAL(Fmt,...) FATAL_AT(__FILE__,__LINE__,__FUNCTION__,Fmt,##__VA_ARGS__)

int
main (int argc, char **argv)
{
  assert (argc > 0);
  rl_initialize ();		/// initialize readline
  progname_RPS = argv[0];
  zlibv_RPS = zlibVersion ();
  jitctx_RPS = gcc_jit_context_acquire ();
  if (!jitctx_RPS)
    FATAL ("%s failed to create jitctx_RPS (%s)", progname_RPS,
	   strerror (errno));
  {
    char selfpath[256];
    snprintf (selfpath, sizeof (selfpath), "%s/%s", sourcedir_RPS, __FILE__);
    if (access (selfpath, R_OK))
      FATAL ("%s failed to access selfpath %s (%s)",
	     progname_RPS, selfpath, strerror (errno));
  }
  // should define textual formats, a few global variables, what they
  // contain, how is the libgccjit transforming textual data to
  // code... Some global variables are pointers functions and they
  // could need a libgccjit computation
#warning TODO a lot of things
  gcc_jit_context_release (jitctx_RPS);
  printf ("%s ending successfully (git %s)\n", progname_RPS, shortgitid_RPS);
  return 0;
}				/* end main */
