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

#ifndef NO_STATIC_ASSERTS
static_assert (sizeof (FILE) < 32 * sizeof (void *),
               "check FILE has less than 32 words");
#endif


const char start_comment_HDGJIRPS[] = "#*START-GCCJIT-REFPERSYS";

static void run_loading_routine_at_HDGJIRPS (struct load_data_HDGJIRPS_st *ldp,
                                             const char *routname, intptr_t num,
                                             int lin);

#define RUN_LOADING_ROUTINE_HDGJIRPS(Ldp,Nam,Num)			\
  do {run_loading_routine_at_HDGJIRPS((Ldp),(Nam),(Num),__LINE__);	\
  } while(0)

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
  const char *startcomm = strstr ((const char *) start, start_comment_HDGJIRPS);
  if (!startcomm)
    HDGJIRPS_FATAL
      ("load state file %s is lacking a start comment %s",
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
  ldata.lda_lineno = lineno;
  const unsigned ldstacksiz = 512;
  ldata.lda_stack = mallocx (ldstacksiz * sizeof (void *),
                             MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
  ldata.lda_sizstack = ldstacksiz;
  ldata.lda_topstack = 0;
#warning should use the stack of ldata
  load_data_HDGJIRPS (&ldata);
  if (verbose_HDGJIRPS)
    printf ("%s: loaded state %s (%d lines)\n",
            progname_HDGJIRPS, path, ldata.lda_lineno);
  fflush (NULL);
}                               /* end load_state_HDGJIRPS */

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
}                               /* end load_skip_spaces_HDGJIRPS */

/// called thru RUN_LOADING_ROUTINE_HDGJIRPS macro
void
run_loading_routine_at_HDGJIRPS (struct load_data_HDGJIRPS_st *ldp,
                                 const char *name, intptr_t num, int lin)
{
  char routname[160];
  memset (routname, 0, sizeof (routname));
  if (!ldp || ldp->lda_magic != LOADMAGIC_HDGJIRPS)
    HDGJIRPS_FATAL ("run_loading_routine_at_HDGJIRPS bad ld@%p (%s:%d)",
                    ldp, __FILE__, lin);
  if (!name)
    HDGJIRPS_FATAL ("run_loading_routine_at_HDGJIRPS no name (%s:%d)",
                    __FILE__, lin);
  if (!isalpha (name[0]) || strlen (name) > 60)
    HDGJIRPS_FATAL ("run_loading_routine_at_HDGJIRPS bad name '%s' (%s:%d)",
                    name, __FILE__, lin);
  for (const char *pn = name; *pn; pn++)
    {
      if (!isalnum (*pn) && *pn != '_')
        HDGJIRPS_FATAL ("run_loading_routine_at_HDGJIRPS bad name '%s' (%s:%d)",
                        __FILE__, name, lin);
    };
  int ln = snprintf (routname, sizeof (routname), "loader_%s_HDGJIRPS",
                     name);
  if (ln > (int) sizeof (routname) - 2)
    HDGJIRPS_FATAL ("run_loading_routine_at_HDGJIRPS too big '%s' (%s:%d)",
                    name, __FILE__, lin);
  void *routad = dlsym (full_program_dlhandle_HDGJIRPS (), routname);
  if (!routad)
    HDGJIRPS_FATAL ("in %s:%d bad routine name %s : %s (%s:%d)",
                    ldp->lda_path, ldp->lda_lineno, routname, dlerror (),
                    __FILE__, lin);
  load_routine_sig_HDGJIRPS *routptr = (load_routine_sig_HDGJIRPS *) routad;
  (*routptr) (ldp, num);
}                               /* end run_loading_routine_at_HDGJIRPS */

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
      if (sscanf (s + 1, instring ? "%2x%n" : "%x%n", (unsigned *) &rc, &pos) < 1)
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
}                               /* end get_escape_sequence_HDGHIRPS */



//// this function is adapted from nwcc
void *
loaded_value_HDGJIRPS (struct load_data_HDGJIRPS_st *ld)
{
  if (!ld || ld->lda_magic != LOADMAGIC_HDGJIRPS)
    return NULL;
  void *res = NULL;
  char *buf = NULL;
  unsigned bufsiz = 0;
  unsigned buflen = 0;
  char smallbuf[80];
  int pos = -1;
  intptr_t i = 0;
  long long il = 0;
  double d = 0;
  memset (smallbuf, 0, sizeof (smallbuf));
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
          buf = NULL;
          if (!eol)
            eol = ld->lda_end;
          assert (eol >= (const char *) ld->lda_cur
                  && (const char *) eol <= (const char *) ld->lda_end);
          if (eol - (const char *) ld->lda_cur < (long) sizeof (smallbuf) - 2)
            buf = smallbuf;
          else
            {
              bool stop = true;
              bufsiz = ((2 * sizeof (smallbuf) + 10) | 0xf) + 1;
              buf = mallocx (bufsiz, MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
              //useless since using MALLOCX_ZERO: memset (buf, 0, bufsiz);
              do
                {
                  int u = get_escape_sequence_HDGHIRPS ((const char **) &ld->lda_cur,
                                                        true, &stop);
                  /// the escaped NULL \0 is forbidden
                  if (u == 0 && !stop)
                    {
                      HDGJIRPS_FATAL ("in %s:%d the null byte is forbidden",
                                      ld->lda_path, ld->lda_lineno);
                    };
                  if (u > 0 && !stop)
                    {
                      char minbuf[8];
                      memset (minbuf, 0, sizeof (minbuf));
                      int siz = u8_uctomb ((uint8_t *) minbuf, (ucs4_t) u,
                                           (ptrdiff_t) sizeof (minbuf));
                      if (siz > 0 && siz + strlen (buf) < bufsiz)
                        {
                          strncpy (buf + buflen, minbuf, siz);
                          buflen += siz;
                        }
                      else
                        {
                          unsigned newbufsiz = ((5 * bufsiz / 4 + 50) | 0x1f) + 1;
                          assert (newbufsiz + 2 >= sizeof (minbuf));
                          const char *oldbuf = buf;
                          char *newbuf = mallocx (newbufsiz,
                                                  MALLOCX_ZERO |
                                                  MALLOCX_ALIGN (1 << 8));
                          memcpy (newbuf, oldbuf, buflen);
                          if (oldbuf != smallbuf)
                            {
                              /// perhaps sdallocx should be used?
                              /** note: in nov 2026 on Debian/Testing
                                 the system libjemalloc 5.3 don't have
                                 free_sized */
#if JEMALLOC_VERSION_MAJOR<=5 && JEMALLOC_VERSION_MINOR<4
                              sdallocx ((void *) oldbuf, bufsiz,
                                        MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
#else
                              free_sized ((void *) oldbuf, bufsiz);
#endif /*libjemalloc v <= 3.4 */
                              oldbuf = NULL;
                            };
                          buf = newbuf;
                        }
                    }
                }
              while (!stop && ld->lda_cur < ld->lda_end);
            }
        }
      if (buf && buf[0] && buflen > 0)
        res = make_sized_string_HDGJIRPS (buf, buflen);
      return res;
    }                           /* end literal string */
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
}                               /* end loaded_value_HDGJIRPS */

void
load_push_value_HDGJIRPS (struct load_data_HDGJIRPS_st *ldp, const void *val)
{
  assert (ldp && ldp->lda_magic == LOADMAGIC_HDGJIRPS);
  assert (ldp->lda_path != NULL);
  assert (ldp->lda_stack != NULL);
  if (ldp->lda_topstack + 2 > ldp->lda_sizstack)
    {
      void *oldstack = ldp->lda_stack;
      uint32_t oldsiz = ldp->lda_sizstack;
      assert (ldp->lda_sizstack < UINT32_MAX / 2);
      uint32_t newsiz = 1 + ((ldp->lda_topstack + ldp->lda_topstack / 3) | 0x1f);
      assert (newsiz < 3 * UINT32_MAX / 4);
      ldp->lda_stack =
        mallocx (newsiz * sizeof (void *), MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
      if (!ldp->lda_stack)
        HDGJIRPS_FATAL
          ("failed to grow load stack to %d pointers (previous %d) at %s:%d",
           (int) newsiz, oldsiz, ldp->lda_path, ldp->lda_lineno);
      memcpy (ldp->lda_stack, oldstack, ldp->lda_topstack * sizeof (void *));
      /// perhaps sdallocx should be used?
      /** note: in nov 2026 on Debian/Testing the system libjemalloc 5.3 don't have
         free_sized */
#if JEMALLOC_VERSION_MAJOR<=5 && JEMALLOC_VERSION_MINOR<4
      sdallocx ((void *) oldstack, oldsiz * sizeof (void *),
                MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
#else
      free_sized (oldstack, oldsiz * sizeof (void *));
#endif
      ldp->lda_sizstack = newsiz;
    };
  ldp->lda_stack[ldp->lda_topstack++] = val;
}                               /* end load_push_value_HDGJIRPS */

void
load_data_HDGJIRPS (struct load_data_HDGJIRPS_st *ldp)
{
  const char *eol = NULL;
  int nbscan = -1;
  assert (ldp && ldp->lda_magic == LOADMAGIC_HDGJIRPS);
  assert (ldp->lda_path != NULL);
  while (ldp->lda_cur < ldp->lda_end)
    {
      void *val = NULL;
      int pos = -1;
      long num = 0;
      char nam[72] = { };
      char routnam[128] = { };
      char bufhi[32] = { };
      char buflo[32] = { };
      memset (nam, 0, sizeof (nam));
      memset (routnam, 0, sizeof (routnam));
      memset (bufhi, 0, sizeof (bufhi));
      memset (buflo, 0, sizeof (buflo));
      load_skip_spaces_HDGJIRPS (ldp);
      eol = strchr (ldp->lda_cur, '\n');
      if (!eol)
        eol = ldp->lda_end;
      nbscan = -1;
      if (*(const char *) ldp->lda_cur == '#')
        {
          ldp->lda_cur = (void *) (eol + 1);
          ldp->lda_lineno++;
          continue;
        }
      else if ((nbscan = sscanf (ldp->lda_cur, "@MAKOBJ_%30[0-9A-Z]_%30[0-9A-Z]%n",
                                 bufhi, buflo, &pos)) >= 2 && pos > 0
               && (char *) ldp->lda_cur + pos <= eol)
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
                  HDGJIRPS_FATAL
                    ("load_data_HDGJIRPS at %s:%d cannot @MAKOBJ existing object (hi=%u='%s' lo=%lu='%s')",
                     ldp->lda_path, ldp->lda_lineno, (unsigned) hi, bufhi,
                     (unsigned long) lo, buflo);
                }
              else
                {
                  Obj_HDGJIRPS *newob = make_loaded_object_HDGJIRPS (ldp, hi, lo);
                  if (!newob)
                    continue;
                  register_composite_value_HDGJIRPS (newob, comp_boxed_object);
                };
            }
        }
      else if ((val = loaded_value_HDGJIRPS (ldp)) != NULL)
        {
          load_push_value_HDGJIRPS (ldp, val);
        }
      else if ((nbscan = sscanf (ldp->lda_cur, "@%70[a-zA-Z0-9_] %ld %n",
                                 nam, &num, &pos)) >= 2
               && isalpha (nam[0]) && pos > 0 && (char *) ldp->lda_cur + pos <= eol)
        {
          RUN_LOADING_ROUTINE_HDGJIRPS (ldp, nam, num);
        }
      else if ((nbscan = sscanf (ldp->lda_cur, "@%70[a-zA-Z0-9_] '%n",
                                 nam, &pos)) >= 2
               && isalpha (nam[0]) && pos > 0 && (char *) ldp->lda_cur + pos <= eol)
        {
          RUN_LOADING_ROUTINE_HDGJIRPS (ldp, nam, num);
        }
      else
        {
          eol = strchr ((const char *) ldp->lda_cur, '\n');
          if (!eol)
            eol = ldp->lda_end;
          HDGJIRPS_WARNING ("incomplete load_data_HDGJIRPS @%s:%d line:%.*s",
                            ldp->lda_path, ldp->lda_lineno,
                            (int) (eol - (const char *) ldp->lda_cur),
                            (const char *) ldp->lda_cur);
          /* todo: define a load stack and some operations to handle it,
             with ideas inspired by PostScript, PDF, FORTH, or NeWS */
          goto end;
        }
    };
end:
  HDGJIRPS_WARNING ("load_data_HDGJIRPS unimplemented for path %s"
                    " line %d, git %s\n"
                    "\n parsed line: %.*s\n",
                    ldp->lda_path, ldp->lda_lineno, persist_HDGJIRPS_git,
                    (int) (eol - (const char *) ldp->lda_cur),
                    (const char *) ldp->lda_cur);
#warning incomplete load_data_HDGJIRPS should use ldp
}                               /* end load_data_HDGJIRPS */



void
load_fill_object_HDGJIRPS (struct load_data_HDGJIRPS_st *ldp, Obj_HDGJIRPS *pob)
{
  assert (ldp && ldp->lda_magic == LOADMAGIC_HDGJIRPS);
#warning unimplemented and perhaps useless load_fill_object_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented load_fill_object_HDGJIRPS ldp@%p"
                  " at %s:%d (pob@%p)", ldp, ldp->lda_path, ldp->lda_lineno, pob);
}                               /* end load_fill_object_HDGJIRPS */

const uint32_t dump_magic_HDGJIRPS = DUMPMAGIC_HDGJIRPS;

static ssize_t dump_reader_HDGJIRPS (void *cookie, char *buffer, size_t size);
static ssize_t dump_writer_HDGJIRPS (void *cookie, const char *buffer, size_t size);
static int dump_seeker_HDGJIRPS (void *cookie, off64_t * position, int whence);
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
    HDGJIRPS_FATAL ("failed to open state file %s (%s)", path, strerror (errno));
  cookie_io_functions_t iofun;
  memset (&iofun, 0, sizeof (iofun));
  iofun.read = dump_reader_HDGJIRPS;
  iofun.write = dump_writer_HDGJIRPS;
  iofun.seek = dump_seeker_HDGJIRPS;
  iofun.close = dump_cleaner_HDGJIRPS;
  FILE *cookf = fopencookie (&dd, "w", iofun);
  if (!cookf)
    HDGJIRPS_FATAL ("failed to open cookie file dd@%p (%s)", &dd, strerror (errno));
  dd.dump_magic = DUMPMAGIC_HDGJIRPS;
  dd.dump_indent = 0;
  dd.dump_path = path;
  dd.dump_file = filsta;
  dd.dump_bol = 0;
  dd.dump_data = NULL;
#warning write_state_HDGJIRPS is incomplete and needs a better signature
  HDGJIRPS_FATAL ("unimplemented write_state_HDGJIRPS path=%s", path);
}                               /* end write_state_HDGJIRPS */

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
}                               // end dump_reader_HDGJIRPS

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
}                               // end dump_writer_HDGJIRPS

static int
dump_seeker_HDGJIRPS (void *cookie, off64_t *position, int whence)
{
  int res = 0;
  struct dump_data_HDGJIRPS_st *dd = (struct dump_data_HDGJIRPS_st *) cookie;
  assert (dd && dd->dump_magic == DUMPMAGIC_HDGJIRPS);
  assert (position);
  assert (whence >= 0);
#warning unimplemented dump_seeker_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented dump_seeker_HDGJIRPS dd@%p", cookie);
  return res;
}                               /* end dump_seeker_HDGJIRPS */

static int
dump_cleaner_HDGJIRPS (void *cookie)
{
  int res = 0;
  struct dump_data_HDGJIRPS_st *dd = (struct dump_data_HDGJIRPS_st *) cookie;
  assert (dd && dd->dump_magic == DUMPMAGIC_HDGJIRPS);
#warning unimplemented dump_cleaner_HDGJIRPS
  HDGJIRPS_FATAL ("unimplemented dump_cleaner_HDGJIRPS dd@%p", cookie);
  return res;
}                               /* end dump_cleaner_HDGJIRPS */

/* end of file gccjit-refpersys/persist.c */
