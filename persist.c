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

const char persist_HDGJIRPS_git[] = SHORTGITID;

#warning TODO: define a simple and human readable syntax of the persistent file

static_assert (sizeof (FILE) < 32 * sizeof (void *));

static void load_data_HDGJIRPS (struct load_data_HDGJIRPS_st *ld);

static void *loaded_value_HDGJIRPS (struct load_data_HDGJIRPS_st *ld);

const char start_comment_HDGJIRPS[] = "#*START-GCCJIT-REFPERSYS";

/// This loading state is called after a successful mmap of path
/// (start -> end)
void
load_state_HDGJIRPS (const char *path, const void *start, const void *last)
{
  assert (path != NULL);
  assert (start != NULL);
  assert (last != NULL);
  assert (last > start);
  assert (loadmagic_HDGJRPS == LOADMAGIC_HDGJRPS);
  const char *startcomm =
    strnstr ((const char *) start, start_comment_HDGJIRPS,
	     (const char *) last - (const char *) start);
  if (!startcomm)
    HDGJIRPS_FATAL ("load state file %s is lacking a start comment %s",
		    path, start_comment_HDGJIRPS);
  if (startcomm > (const char *) start
      && startcomm[-1] != '\n' && startcomm[-1] != '\r')
    HDGJIRPS_FATAL
      ("load state file %s with start comment %s not at start of line", path,
       start_comment_HDGJIRPS);
  const char *endcomm = startcomm + strlen (start_comment_HDGJIRPS);
  assert (endcomm < (const char *) last);
  struct load_data_HDGJIRPS_st ldata = { };
  ldata.lda_magic = LOADMAGIC_HDGJRPS;
  ldata.lda_path = path;
  ldata.lda_start = (void *) endcomm;
  ldata.lda_cur = endcomm + 1;
  ldata.lda_end = (void *) last;
  load_data_HDGJIRPS (&ldata);
  if (verbose_HDGJIRPS)
    printf ("%s: loaded state %s\n", progname_HDGJIRPS, path);
}				/* end load_state_HDGJIRPS */

static void *
loaded_value_HDGJIRPS (struct load_data_HDGJIRPS_st *ld)
{
  if (!ld || ld->lda_magic != LOADMAGIC_HDGJRPS)
    return NULL;
  void *res = NULL;
  int pos = -1;
  intptr_t i = 0;
  long long il = 0;
  double d = 0;
  ///https://stackoverflow.com/a/5796039/841108
  if (sscanf (ld->lda_cur, " INT%lli%n", &il, &pos) >= 2 && pos > 0)
    {
      i = (intptr_t) il;
      res = make_box_int_HDGJIRPS (i);
      ld->lda_cur += pos;
      return res;
    }
  else if (sscanf (ld->lda_cur, " FLO%lg%n", &d, &pos) >= 2 && pos)
    {
      res = make_box_double_HDGJIRPS (d);
      ld->lda_cur += pos;
      return res;
    }
#warning loaded_value_HDGJIRPS very incomplete
  return NULL;
}				/* end loaded_value_HDGJIRPS */

void
load_data_HDGJIRPS (struct load_data_HDGJIRPS_st *ld)
{
  assert (ld && ld->lda_magic == LOADMAGIC_HDGJRPS);
  fprintf (stderr,
	   "load_data_HDGJIRPS unimplemented for path %s [%s:%d] git %s\n",
	   ld->lda_path, __FILE__, __LINE__, persist_HDGJIRPS_git);
#warning incomplete load_data_HDGJIRPS should use ld
}				/* end load_data_HDGJIRPS */

const uint32_t dump_magic_HDGJIRPS = DUMPMAGIC_HDGJIRPS;
void
write_state_HDGJIRPS (const char *path, void **tabptr, size_t siztab)
{
  struct dump_data_HDGJIRPS_st dd = { };
  FILE *filsta = NULL;
  assert (path);
  assert (tabptr != NULL);
  assert (siztab > 0);
  memset (&dd, 0, sizeof (dd));
  if (!access (path, F_OK))
    {
      char backupath[384];
      memset (backupath, 0, sizeof (backupath));
      snprintf (backupath, sizeof (backupath) - 4, "%s~", path);
      if (strlen (backupath) > strlen (path))
	rename (path, backupath);
    };
  filsta = fopen (path, "w");
  if (!filsta)
    HDGJIRPS_FATAL ("failed to open state file %s (%s)", path,
		    strerror (errno));
  dd.dump_magic = DUMPMAGIC_HDGJIRPS;
  dd.dump_path = path;
  dd.dump_file = filsta;
  dd.dump_bol = 0;
#warning write_state_HDGJIRPS is incomplete and needs a better signature
  HDGJIRPS_FATAL ("unimplemented write_state_HDGJIRPS path=%s", path);
}				/* end write_state_HDGJIRPS */
