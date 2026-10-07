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
 *      © Copyright (C) 2019 - 2026 The Reflective Persistent System Team
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

//// TODO: avoid most non-generated header files (except hdgjirps.h)!

#include "hdgjirps.h"

#warning TODO: define a simple and human readable syntax of the persistent file

const char start_comment_HDGJIRPS[] = "#*START-GCCJIT-REFPERSYS";
void
load_state_HDGJIRPS(const char*path, const void*start, const void*last)
{
  assert(path != NULL);
  assert(start != NULL);
  assert(last != NULL);
  assert (last > start);
  const char*startcomm = strnstr((const char*)start, start_comment_HDGJIRPS,
				 (const char*)last-(const char*)start);
  if (!startcomm)
    HDGJIRPS_FATAL("load state file %s is lacking a start comment %s",
	  path, start_comment_HDGJIRPS);
  if (startcomm > (const char*)start
      && startcomm[-1]!='\n' && startcomm[-1]!='\r')
    HDGJIRPS_FATAL("load state file %s with start comment %s not at start of line",
	  path, start_comment_HDGJIRPS);
  const char*endcomm = startcomm + strlen(start_comment_HDGJIRPS);
  assert (endcomm < (const char*)last);
#warning incomplete load_state_HDGJIRPS
  if (verbose_HDGJIRPS)
    printf("%s: loaded state %s\n", progname_HDGJIRPS, path);
} /* end load_state_HDGJIRPS */


void
write_state_HDGJIRPS(const char*path)
{
  FILE*filsta = NULL;
  if (!access(path, F_OK)) {
    char backupath[384];
    memset (backupath, 0, sizeof(backupath));
    snprintf(backupath, sizeof(backupath)-4, "%s~", path);
    if (strlen(backupath) > strlen(path))
      rename(path, backupath);
  };
  filsta = fopen(path, "w");
  if (!filsta)
    HDGJIRPS_FATAL("failed to open state file %s (%s)", path, strerror(errno));
#warning write_state_HDGJIRPS is missing and needs a better signature
  HDGJIRPS_FATAL("unimplemented write_state_HDGJIRPS path=%s", path);
} /* end write_state_HDGJIRPS */
