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
  int lineno = 1;
  assert (path != NULL);
  assert (start != NULL);
  assert (last != NULL);
  assert (last > start);
  assert (loadmagic_HDGJIRPS == LOADMAGIC_HDGJIRPS);
  const char *startcomm =
    strnstr ((const char *) start, start_comment_HDGJIRPS,
	     (const char *) last - (const char *) start);
  if (!startcomm)
    HDGJIRPS_FATAL ("load state file %s is lacking a start comment %s",
		    path, start_comment_HDGJIRPS);
  for (const char *p = start; p < startcomm; p++)
    if (*p == '\n')
      lineno++;
  if (startcomm > (const char *) start
      && startcomm[-1] != '\n' && startcomm[-1] != '\r')
    HDGJIRPS_FATAL ("load state file %s "
		    "with start comment %s not at start of line#%d",
		    path, start_comment_HDGJIRPS, lineno);
  const char *endcomm = startcomm + strlen (start_comment_HDGJIRPS);
  assert (endcomm < (const char *) last);
  struct load_data_HDGJIRPS_st ldata = { };
  ldata.lda_magic = LOADMAGIC_HDGJIRPS;
  ldata.lda_path = path;
  ldata.lda_start = (void *) endcomm;
  ldata.lda_cur = (void*) (endcomm + 1);
  ldata.lda_end = (void *) last;
  load_data_HDGJIRPS (&ldata);
  if (verbose_HDGJIRPS)
    printf ("%s: loaded state %s\n", progname_HDGJIRPS, path);
}				/* end load_state_HDGJIRPS */

void
load_skip_spaces_HDGJIRPS (struct load_data_HDGJIRPS_st *ld)
{
  if (!ld || ld->lda_magic != LOADMAGIC_HDGJIRPS)
    HDGJIRPS_FATAL ("load_skip_spaces_HDGJIRPS bad ld@%p", ld);
  while (ld->lda_cur < ld->lda_end && isspace (*(char *) (ld->lda_cur)))
    {
      if (*((char *) (ld->lda_cur)) == '\n')
	ld->lda_lineno++;
    };
}				/* end load_skip_spaces_HDGJIRPS */

static void *
loaded_value_HDGJIRPS (struct load_data_HDGJIRPS_st *ld)
{
  if (!ld || ld->lda_magic != LOADMAGIC_HDGJIRPS)
    return NULL;
  void *res = NULL;
  int pos = -1;
  intptr_t i = 0;
  long long il = 0;
  double d = 0;
  load_skip_spaces_HDGJIRPS (ld);
  if (ld->lda_cur >= ld->lda_end)
    return NULL;
  if (isdigit (*(char *) ld->lda_cur)
      || ((*(char*)ld->lda_cur=='+' || *(char*)ld->lda_cur=='-')
	  && isdigit (((char *) ld->lda_cur)[1])))
    {
      char*endint= NULL;
      char*endflo= NULL;
      long long lli= 0;
      double f=NAN;
      lli= strtoll((const char*)ld->lda_cur, &endint, 0);
      f= strtod((const char*)ld->lda_cur, &endflo);
      if (endint != NULL && endflo != NULL && endflo>endint) {
	ld->lda_cur = endflo;
	return make_box_double_HDGJIRPS(f);
      }
      else if (endint > (const char*)ld->lda_cur) {
	ld->lda_cur = endint;
	return make_box_int_HDGJIRPS((intptr_t)lli);
      }
      return NULL;
    }
  ///https://stackoverflow.com/a/5796039/841108
  if (sscanf (ld->lda_cur, " INT%lli%n", &il, &pos) >= 2 && pos > 0)
    {
      i = (intptr_t) il;
      res = make_box_int_HDGJIRPS (i);
      ld->lda_cur += pos;
      return res;
    }
  else if (sscanf (ld->lda_cur, " FLO%lg%n", &d, &pos) >= 2 && pos > 0)
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
  assert (ld && ld->lda_magic == LOADMAGIC_HDGJIRPS);
  fprintf (stderr,
	   "load_data_HDGJIRPS unimplemented for path %s [%s:%d] git %s\n",
	   ld->lda_path, __FILE__, __LINE__, persist_HDGJIRPS_git);
#warning incomplete load_data_HDGJIRPS should use ld
}				/* end load_data_HDGJIRPS */

const uint32_t dump_magic_HDGJIRPS = DUMPMAGIC_HDGJIRPS;
static ssize_t dump_reader_HDGJIRPS (void *cookie, char *buffer, size_t size);
static ssize_t dump_writer_HDGJIRPS (void *cookie, const char *buffer,
				     size_t size);
static int dump_seeker_HDGJIRPS (void *cookie, off64_t * position,
				 int whence);
static int dump_cleaner_HDGJIRPS (void *cookie);

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
  // see https://sourceware.org/glibc/manual/latest/html_node/Streams-and-Cookies.html
#warning should probably use fopencookie
  filsta = fopen (path, "w");
  if (!filsta)
    HDGJIRPS_FATAL ("failed to open state file %s (%s)", path,
		    strerror (errno));
  cookie_io_functions_t iofun;
  memset (&iofun, 0, sizeof (iofun));
  iofun.read = dump_reader_HDGJIRPS;
  iofun.write = dump_writer_HDGJIRPS;
  iofun.seek = dump_seeker_HDGJIRPS;
  iofun.close = dump_cleaner_HDGJIRPS;
  FILE *cookf = fopencookie (&dd, "w", iofun);
  if (!cookf)
    HDGJIRPS_FATAL ("failed to open cookie file dd@%p (%s)", &dd,
		    strerror (errno));
  dd.dump_magic = DUMPMAGIC_HDGJIRPS;
  dd.dump_indent = 0;
  dd.dump_path = path;
  dd.dump_file = filsta;
  dd.dump_bol = 0;
  dd.dump_data = NULL;
#warning write_state_HDGJIRPS is incomplete and needs a better signature
  HDGJIRPS_FATAL ("unimplemented write_state_HDGJIRPS path=%s", path);
}				/* end write_state_HDGJIRPS */

static ssize_t
dump_reader_HDGJIRPS (void *cookie, char *buffer, size_t size)
{
  ssize_t res = 0;
  struct dump_data_HDGJIRPS_st *dd = (struct dump_data_HDGJIRPS_st *) cookie;
  assert (dd && dd->dump_magic == DUMPMAGIC_HDGJIRPS);
  assert (buffer);
  assert (size > 0);
#warning unimplimented dump_reader_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented dump_reader_HDGJIRPS dd@%p", cookie);
  return res;
}				// end dump_reader_HDGJIRPS

static ssize_t
dump_writer_HDGJIRPS (void *cookie, const char *buffer, size_t size)
{
  ssize_t res = 0;
  struct dump_data_HDGJIRPS_st *dd = (struct dump_data_HDGJIRPS_st *) cookie;
  assert (dd && dd->dump_magic == DUMPMAGIC_HDGJIRPS);
  assert (buffer);
  assert (size > 0);
#warning unimplimented dump_writer_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented dump_writer_HDGJIRPS dd@%p", cookie);
  return res;
}				// end dump_writer_HDGJIRPS

static int
dump_seeker_HDGJIRPS (void *cookie, off64_t *position, int whence)
{
  int res = 0;
  struct dump_data_HDGJIRPS_st *dd = (struct dump_data_HDGJIRPS_st *) cookie;
  assert (dd && dd->dump_magic == DUMPMAGIC_HDGJIRPS);
  assert (position);
#warning unimplemented dump_seeker_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented dump_seeker_HDGJIRPS dd@%p", cookie);
  return res;
}				/* end dump_seeker_HDGJIRPS */

static int
dump_cleaner_HDGJIRPS (void *cookie)
{
  int res = 0;
  struct dump_data_HDGJIRPS_st *dd = (struct dump_data_HDGJIRPS_st *) cookie;
  assert (dd && dd->dump_magic == DUMPMAGIC_HDGJIRPS);
#warning unimplemented dump_cleaner_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented dump_cleaner_HDGJIRPS dd@%p", cookie);
  return res;
}				/* end dump_cleaner_HDGJIRPS */

/* end of file gccjit-refpersys/persist.c */
