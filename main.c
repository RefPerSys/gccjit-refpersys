/****************************************************************
 * file refpersys.hh
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *      It is almost its only public C++ header file.
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
#include <libgccjit.h>
gcc_jit_context *jitctx;
const char *progname;
#define FATAL_AT_BIS(Fil,Lin,Func,Fmt,...) do {  \
    fprintf(stderr, "%s:%d:%s ", (Fil), (Lin), (Func));	\
    fprintf(stderr, Fmt "\n", ##__VA_ARGS__);		\
    fflush(NULL);					\
    abort(); } while(0)

#define FATAL_AT(Fil,Lin,Func,Fmt,...) \
  FATAL_AT_BIS(Fil,Lin,Func,Fmt,##__VA_ARGS__)

#define FATAL(Fmt,...) FATAL_AT(__FILE__,__LINE__,__FUNCTION__,Fmt,##__VA_ARGS__)

int
main (int argc, char **argv)
{
  assert (argc > 0);
  rl_initialize ();		/// initialize readline
  progname = argv[0];
  jitctx = gcc_jit_context_acquire ();
  if (!jitctx)
    FATAL ("%s failed to create jitctx (%s)", progname, strerror (errno));
  gcc_jit_context_release (jitctx);
}				/* end main */
