/****************************************************************
 * file gccjit-refpersys/persist.c
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *      the variant aiming to use libgccjit from gcc.gnu.org
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


const char start_comment_HDGJIRPS[] = "#*START-GCCJIT-REFPERSYS";

/// This loading state is called after a successful mmap of path
/// (start -> end)
void
load_state_HDGJIRPS (const char *path, const char *start, const char *last)
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
  ldata.lda_cur = (void *) (endcomm + 1);
  ldata.lda_end = (void *) last;
#warning should initialize the stack of ldata
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

/* Code borowed from nwcc.sourceforge.net file token.c */

/*
 * Function parses the escape sequence (following a backslash \ ...)
 * pointed to by *ps and returns the result. The instring argument
 * indicates whether the function is called on a string or character
 * constant. -1 is returned on failure (bad format.) On success the
 * *ps is updated.
 */
int
get_escape_sequence_HDGHIRPS (const char **ps, bool instring, bool *badseq)
{
  int rc = -1;
  int pos = -1;
  *badseq = false;
  assert (ps != NULL);
  const char *s = *ps;
  if (isdigit ((unsigned char) *s))
    {
      /* Octal constant */
      if (s[0] == '0' && isdigit ((unsigned char) s[1]) == 0)
	{
	  *ps = s + 1;
	  /* Null constant '\0' */
	  return 0;
	}
      if (s[0] == '0')
	++s;
      if (sscanf (s, instring ? "%3o%n" : "%o%n",
		  (unsigned *) &rc, &pos) < 1 || pos < 0)
	{
	  if (badseq)
	    *badseq = true;
	  HDGJIRPS_WARNING ("bad escape sequence %s", s);
	  return -1;
	}
      if (pos > 0)
	*ps = s + pos;
      return rc;
    }
  else if (tolower ((unsigned char) *s) == 'x')
    {
      /* Is hexadecimal constant */
      if (sscanf (s + 1, instring ? "%2x%n" : "%x%n",
		  (unsigned *) &rc, &pos) < 1)
	{
	  HDGJIRPS_WARNING ("Invalid hexadecimal constant %s", s);
	  if (badseq)
	    *badseq = true;
	  return -1;
	}
    }
  else if (*s == 'u')
    {
      if (sscanf (s + 1, "%u4x%n", (unsigned *) &rc, &pos) < 1 || pos <= 0)
	{
	  HDGJIRPS_WARNING ("Invalid hexadecimal escape %s", s);
	  if (badseq)
	    *badseq = true;
	  return -1;
	}
      if (badseq)
	*badseq = false;

      *ps = s + pos + 1;
      return rc;
    }
  else if (*s == 'U')
    {
      if (sscanf (s + 1, "%u8x%n", (unsigned *) &rc, &pos) < 1 || pos <= 0)
	{
	  HDGJIRPS_WARNING ("Invalid hexadecimal escape %s", s);
	  if (badseq)
	    *badseq = true;
	  return -1;
	}
      if (badseq)
	*badseq = false;

      *ps = s + pos + 1;
      return rc;
    }
  else
    {
      bool foundesc = false;
      /* Must be other escape sequence */
      static struct
      {
	char ch;
	char seq;
      } esc[] = {
	{'n', '\n'},
	{'r', '\r'},
	{'t', '\t'},
	{'v', '\v'},
	{'b', '\b'},
	{'a', '\a'},
	{'f', '\f'},
	{'Q', '\''},
	{'\\', '\\'},
	{'\'', '\''},
	{'\"', '\"'},
	{'\?', '\?'},
	{0, 0}
      };
      int i;
      for (i = 0; esc[i].ch != 0; ++i)
	{
	  if (s[0] == esc[i].ch)
	    {
	      foundesc = true;
	      rc = esc[i].seq;
	      if (s[1] != 0 && !instring)
		{
		  if (s[1] != '\'')
		    {
		      HDGJIRPS_WARNING ("Multiple characters "
					"in character constant %s"
					" - ignoring excess characters.", s);
		    }
		}
	      break;
	    }
	}
      if (esc[i].ch == 0)
	{
	  if (s[0] == '%')
	    {
	      HDGJIRPS_WARNING ("`\\%%' is not a valid escape in %s", s);
	      if (badseq)
		*badseq = true;
	      return -1;
	    }
	  else
	    {
	      HDGJIRPS_WARNING ("Invalid escape sequence. %s", s);
	      if (badseq)
		*badseq = true;
	      return -1;
	    }
	};
      if (!foundesc)
	{
	  int minlen = 0;
	  ucs4_t uc = 0;
	  for (i = 0; i < 8 && s[i] != (char) 0; i++)
	    minlen++;
	  int clen = u8_mblen ((const uint8_t *) s, minlen);
	  if (clen < 0)
	    {
	      HDGJIRPS_WARNING ("Invalid UTF8 sequence. %.8s", s);
	      if (badseq)
		*badseq = true;
	      return -1;
	    };
	  int ulen = u8_mbtouc (&uc, (const uint8_t *) s, clen);
	  if (ulen > 0 && uc_is_print (uc))
	    {
	      if (badseq)
		*badseq = false;
	      *ps = s + ulen + 1;
	      return uc;
	    }
#warning missing code here to handle UTF8 in strings
	}
    }

  if (!instring && (rc > UCHAR_MAX || rc < CHAR_MIN))
    {
      HDGJIRPS_WARNING ("Literal out of character range. %s", s);
      if (badseq)
	*badseq = true;
      return -1;
    }
  rc = (char) rc;
  return rc;
}				/* end get_escape_sequence_HDGHIRPS */



//// this function is adapted from nwcc
void *
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
      || ((*(char *) ld->lda_cur == '+' || *(char *) ld->lda_cur == '-')
	  && isdigit (((char *) ld->lda_cur)[1])))
    {
      char *endint = NULL;
      char *endflo = NULL;
      long long lli = 0;
      double f = NAN;
      lli = strtoll ((const char *) ld->lda_cur, &endint, 0);
      f = strtod ((const char *) ld->lda_cur, &endflo);
      if (endint != NULL && endflo != NULL && endflo > endint)
	{
	  ld->lda_cur = endflo;
	  return make_box_double_HDGJIRPS (f);
	}
      else if (endint > (const char *) ld->lda_cur)
	{
	  ld->lda_cur = endint;
	  return make_box_int_HDGJIRPS ((intptr_t) lli);
	}
      return NULL;
    }
  else if (*(char *) ld->lda_cur == '\"')
    {
      char smbuf[64];
      memset (smbuf, 0, sizeof (smbuf));
      if (sscanf (ld->lda_cur, "\"%60[^\\\n\r\f\"]\"%n", smbuf, &pos) >= 1
	  && pos > 2)
	{
	  ld->lda_cur += pos;
	  return make_string_HDGJIRPS (smbuf);
	}
      else
	{
	  const char *eol = strchr ((const char *) ld->lda_cur, '\n');
	  char *buf = NULL;
	  unsigned bufsiz = 0;
	  char smallbuf[80];
	  memset (smallbuf, 0, sizeof (smallbuf));
	  if (!eol)
	    eol = ld->lda_end;
	  assert (eol >= (const char *) ld->lda_cur
		  && (const char *) eol <= (const char *) ld->lda_end);
	  if (eol - (const char *) ld->lda_cur < (long) sizeof (smallbuf))
	    buf = smallbuf;
	  else
	    {
	      bool stop = true;
	      bufsiz = ((2 * sizeof (smallbuf) + 10) | 0xf) + 1;
	      buf = mallocx (bufsiz, MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
	      memset (buf, 0, bufsiz);
	      do
		{
		  int u =
		    get_escape_sequence_HDGHIRPS ((const char **)
						  &ld->lda_cur,
						  true, &stop);
		  if (u > 0)
		    {
#warning should UTF8 encode u and append it in buf...
		    }
		}
	      while (!stop && ld->lda_cur < ld->lda_end);
	    }
	}
    }
  ///https://stackoverflow.com/a/5796039/841108
  else if (sscanf (ld->lda_cur, "INT%lli%n", &il, &pos) >= 2 && pos > 0)
    {
      i = (intptr_t) il;
      res = make_box_int_HDGJIRPS (i);
      ld->lda_cur += pos;
      return res;
    }
  else if (sscanf (ld->lda_cur, "FLO%lg%n", &d, &pos) >= 2 && pos > 0)
    {
      res = make_box_double_HDGJIRPS (d);
      ld->lda_cur += pos;
      return res;
    }
  else if (sscanf (ld->lda_cur, "OBREF_%n", &pos) >= 1 && pos > 0)
    {
      uint32_t hi = 0;
      uint64_t lo = 0;
      char *endhi = NULL;
      char *endlo = NULL;
      long lh = strtol (ld->lda_cur + pos, &endhi, 36);
      if (lh < UINT32_MAX)
	hi = lh;
      if (hi > 0 && endhi && endhi[0] == '_')
	lo = strtol (endhi + 1, &endlo, 36);
      if (hi > 0 && lo > 0 && endlo)
	{
	  Obj_HDGJIRPS *pob = find_object_HDGJIRPS (hi, lo);
	  if (!pob)
	    HDGJIRPS_FATAL
	      ("invalid object reference %s in loaded file %s:%d",
	       (const char *) (ld->lda_cur), ld->lda_path, ld->lda_lineno);
	  res = pob;
	  ld->lda_cur = endlo;
	  return res;
	}
    }
#warning loaded_value_HDGJIRPS very incomplete
  return NULL;
}				/* end loaded_value_HDGJIRPS */

void
load_data_HDGJIRPS (struct load_data_HDGJIRPS_st *ld)
{
  assert (ld && ld->lda_magic == LOADMAGIC_HDGJIRPS);
  assert (ld->lda_path != NULL);
  while (ld->lda_cur < ld->lda_end)
    {
      int pos = -1;
      char bufhi[32] = { };
      char buflo[32] = { };
      memset (bufhi, 0, sizeof (bufhi));
      memset (buflo, 0, sizeof (buflo));
      load_skip_spaces_HDGJIRPS (ld);
      const char *eol = strchr (ld->lda_cur, '\n');
      if (!eol)
	eol = ld->lda_end;
      if (sscanf (ld->lda_cur, "MAKOBJ_%30[0-9A-Z]_%30[0-9A-Z]%n",
		  bufhi, buflo, &pos) > 2 && pos > 0
	  && (char *) ld->lda_cur + pos <= eol)
	{
	  bufhi[sizeof (bufhi) - 1] = (char) 0;
	  buflo[sizeof (buflo) - 1] = (char) 0;
	  uint32_t hi = 0;
	  uint64_t lo = 0;
	  hi = (uint32_t) strtol (bufhi, NULL, 36);
	  lo = (uint64_t) strtol (buflo, NULL, 36);
	  if (hi > 0 && lo > 0)
	    {
	      if (find_object_HDGJIRPS (hi, lo))
		{
		  fprintf (stderr,
			   "load_data_HDGJIRPS at %s:%d cannot create existing object\n",
			   ld->lda_path, ld->lda_lineno);
		  fprintf (stderr,
			   " (hi=%d=%s, lo=%ld=%s) [%s:%d]\n",
			   (int) hi, bufhi, (long) lo, buflo, __FILE__,
			   __LINE__);
		  fflush (stderr);
		  continue;
		}
	      else
		{
		  Obj_HDGJIRPS *newob =
		    make_loaded_object_HDGJIRPS (ld, hi, lo);
		  if (!newob)
		    continue;
		  register_composite_value_HDGJIRPS (newob,
						     comp_boxed_object);
		};
	    }
	}
      else if (sscanf (ld->lda_cur, "FILLOBJ_%30[0-9A-Z]_%30[0-9A-Z]%n",
		       bufhi, buflo, &pos) > 2 && pos > 0
	       && (char *) ld->lda_cur + pos <= eol)
	{
	  bufhi[sizeof (bufhi) - 1] = (char) 0;
	  buflo[sizeof (buflo) - 1] = (char) 0;
	  uint32_t hi = 0;
	  uint64_t lo = 0;
	  hi = (uint32_t) strtol (bufhi, NULL, 36);
	  lo = (uint64_t) strtol (buflo, NULL, 36);
	  if (hi > 0 && lo > 0)
	    {
	      Obj_HDGJIRPS *pob = find_object_HDGJIRPS (hi, lo);
	      if (!pob)
		HDGJIRPS_FATAL ("cannot fill object (_%s_%s) at %s:%d",
				bufhi, buflo, ld->lda_path, ld->lda_lineno);
	      ld->lda_cur += pos;
	      load_fill_object_HDGJIRPS (ld, pob);
	    };
	}
      /* todo: define a load stack and some operations to handle it,
         with ideas inspired by PostScript, PDF, FORTH, or NeWS */
    };
  fprintf (stderr,
	   "load_data_HDGJIRPS unimplemented for path %s [%s:%d] git %s\n",
	   ld->lda_path, __FILE__, __LINE__, persist_HDGJIRPS_git);
#warning incomplete load_data_HDGJIRPS should use ld
}				/* end load_data_HDGJIRPS */



void
load_fill_object_HDGJIRPS (struct load_data_HDGJIRPS_st *ld,
			   Obj_HDGJIRPS *pob)
{
  assert (ld && ld->lda_magic == LOADMAGIC_HDGJIRPS);
#warning unimplemented load_fill_object_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented load_fill_object_HDGJIRPS"
		  " at %s:%d (pob@%p)", ld->lda_path, ld->lda_lineno, pob);
}				/* end load_fill_object_HDGJIRPS */

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
#warning unimplemented dump_reader_HDGJIRPS
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
#warning unimplemented dump_writer_HDGJIRPS
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
