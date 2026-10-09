/****************************************************************
 * file gccjit-refpersys/scalar.c
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *      (Some gccjit variant to please indian programmers)
 *      It implements boxed scalar values.
 *
 * Author(s):
 *      Basile Starynkevitch, France   <basile@starynkevitch.net>
 *
 *      © Copyright 2019 - 2026 The Reflective Persistent System Team
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
#include "hdgjirps.h"


#ifndef SHORTGITID
#error SHORTGITID should be defined in the command line
#endif

const char scalar_shortgit_HDGJIRPS[] = SHORTGITID;



///// boxed integers (intptr_t so 64 bits on AMD64)
static_assert (alignof (struct boxint_hdgjirps_st) == (1 << 4));
static_assert (sizeof (struct boxint_hdgjirps_st) < 8 * sizeof (intptr_t));

unsigned
hashstr_HDGJIRPS (const char *s)
{
  unsigned h = 0;
  const uint8_t *us = (const uint8_t *) s;
  if (!s)
    return 0;
  unsigned bytlen = strlen (s);
  int cnt = 0;
  while (us)
    {
      ucs4_t uc = 0;
      int l = u8_mbtoucr (&uc, us, s + bytlen - (char *) us);
      if (l < 0)
	HDGJIRPS_FATAL ("corrupted UTF8 string %s", s);
      cnt++;
      us += l;
      if (cnt % 2 == 0)
	h = ((h * 443) ^ (uc * 347)) + (cnt & 0xff);
      else
	h = ((h * 311) ^ (uc * 359 + cnt % 17));
    };
  if (h == 0)
    h = bytlen + 1;
  return h;
}				/* end  hashstr_HDGJIRPS */

struct boxint_hdgjirps_st *
make_box_int_HDGJIRPS (intptr_t v)
{
  static_assert (alignof (struct boxint_hdgjirps_st) == 16);
  struct boxint_hdgjirps_st *p = MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    HDGJIRPS_FATAL ("out of memory when boxing int %ld", (long) v);
  p->typenum = sca_boxed_int;
  p->gcmark = 0;
  p->flag = 0;
  p->xtranum = 0;
  p->intval = v;
  return p;
}				/* end make_box_int_HDGJIRPS */


bool
get_int_HDGJIRPS (const void *ptr, intptr_t *p)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct boxint_hdgjirps_st *d = (struct boxint_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_int)
    return false;
  if (p)
    *p = d->intval;
  return true;
}				/* end get_int_HDGJIRPS */



///// boxed pair of integers
static_assert (alignof (struct boxtwoints_hdgjirps_st) == (1 << 4));
static_assert (sizeof (struct boxtwoints_hdgjirps_st) <
	       8 * sizeof (intptr_t));

struct boxtwoints_hdgjirps_st *
make_boxtwoints_HDGJIRPS (intptr_t v0, intptr_t v1)
{
  static_assert (alignof (struct boxtwoints_hdgjirps_st) == 16);
  struct boxtwoints_hdgjirps_st *p = MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    HDGJIRPS_FATAL ("out of memory when boxing two ints %ld & %ld",
		    (long) v0, (long) v1);
  p->typenum = sca_boxed_twoints;
  p->gcmark = 0;
  p->flag = 0;
  p->xtranum = 0;
  p->intpair[0] = v0;
  p->intpair[1] = v1;
  return p;
}				/* end make_boxtwoints_HDGJIRPS */


bool
get_twoints_HDGJIRPS (const void *ptr, intptr_t *p0, intptr_t *p1)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct boxtwoints_hdgjirps_st *d =
    (struct boxtwoints_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_twoints)
    return false;
  if (p0)
    *p0 = d->intpair[0];
  if (p1)
    *p1 = d->intpair[1];
  return true;
}				/* end get_twoints_HDGJIRPS */






///// boxed floating point doubles (double so IEEE74 - 64 bits on AMD64)
static_assert (alignof (struct boxdouble_hdgjirps_st) == (1 << 4));
static_assert (sizeof (struct boxdouble_hdgjirps_st) < 8 * sizeof (intptr_t));

struct boxdouble_hdgjirps_st *
make_box_double_HDGJIRPS (double v)
{
  static_assert (alignof (struct boxdouble_hdgjirps_st) == 16);
  struct boxdouble_hdgjirps_st *p = MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    HDGJIRPS_FATAL ("out of memory when boxing double %g", v);
  p->typenum = sca_boxed_double;
  p->gcmark = 0;
  p->flag = 0;
  p->xtranum = 0;
  p->dblval = v;
  return p;
}				/* end make_box_double_HDGJIRPS */


bool
get_double_HDGJIRPS (const void *ptr, double *p)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct boxdouble_hdgjirps_st *d =
    (struct boxdouble_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_double)
    return false;
  if (p)
    *p = d->dblval;
  return true;
}				/* end get_double_HDGJIRPS */

bool
get_double_xtra_HDGJIRPS (const void *ptr, double *p, int32_t *x)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct boxdouble_hdgjirps_st *d =
    (struct boxdouble_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_double)
    return false;
  if (p)
    *p = d->dblval;
  if (x)
    *x = d->xtranum;
  return true;
}				/* end get_double_xtra_HDGJIRPS */


struct string_hdgjirps_st *
make_string_HDGJIRPS (const char *str)
{
  if (!str)
    return NULL;
  size_t slen = strlen (str);
  const uint8_t *uc = u8_check ((const uint8_t *) str, slen);
  if (uc)
    return NULL;
  struct string_hdgjirps_st *p
    = MALLOC_HDGJIRPS (sizeof (*p) + ((slen + 1) | 7) + 1);
  if (!p)
    return NULL;
  p->typenum = sca_boxed_string;
  p->gcmark = 0;
  p->flag = 0;
  p->length = slen;
  memcpy (p->cstr, str, slen);
  return p;
}				/* end make_string_HDGJIRPS */

struct string_hdgjirps_st *
make_sized_string_HDGJIRPS (const char *str, int bytesize)
{
  if (!str || !is_valid_ptr_HDGJIRPS (str))
    return NULL;
  size_t slen = (bytesize < 0) ? strlen (str) : (size_t) bytesize;
  struct string_hdgjirps_st *p
    = MALLOC_HDGJIRPS (sizeof (*p) + ((slen + 1) | 7) + 1);
  if (!p)
    return NULL;
  p->typenum = sca_boxed_string;
  p->gcmark = 0;
  p->flag = 0;
  p->length = slen;
  memcpy (p->cstr, str, slen);
  return p;
}				/* end make_sized_string_HDGJIRPS */

bool
get_string_length_HDGJIRPS (void *ptr, const char **pstr, unsigned *pslen)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct string_hdgjirps_st *d = (struct string_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_string)
    return false;
  if (pstr)
    *pstr = d->cstr;
  if (pslen)
    *pslen = d->length;
  return true;
}				/* end get_string_length_HDGJIRPS */

bool
get_string_HDGJIRPS (void *ptr, const char **pstr)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct string_hdgjirps_st *d = (struct string_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_string)
    return false;
  if (pstr)
    *pstr = d->cstr;
  return true;
}				/* end get_string_HDGJIRPS */

static_assert (sizeof (void *) == sizeof (&fopen));
struct namedrout_hdgjirps_st *
make_namedrout_HDGJIRPS (const char *nam)
{
  if (!nam || !nam[0])
    return NULL;
  size_t namlen = strlen (nam);
  if (namlen >= NAMEDROUT_LENGTH_HDGJIRPS)
    return NULL;
  void *ad = dlsym (full_program_dlhandle_HOGJIRPS (), nam);
  if (!ad)
    {
      fprintf (stderr, "%s: missing symbol %s (%s) [%s:%d]\n",
	       progname_HDGJIRPS, nam, dlerror (), __FILE__, __LINE__);
      fflush (NULL);
      return NULL;
    };
  struct namedrout_hdgjirps_st *p = MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    return NULL;
  p->typenum = sca_boxed_namedrout;
  p->gcmark = 0;
  p->flag = 0;
  strcpy ((char *) p->routnam, nam);
  p->routad = ad;
  return p;
}				/* end make_namedrout_HDGJIRPS */


bool
get_namedrout_HDGJIRPS (const void *ptr, void **pad, const char **pnam)
{
  void *ad = NULL;
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  struct namedrout_hdgjirps_st *d = (struct namedrout_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_namedrout)
    return false;
  assert (isalnum (d->routnam[0]) || d->routnam[0] == '_');
  assert (strlen (d->routnam) < NAMEDROUT_LENGTH_HDGJIRPS);
  if (d->routad == NULL)
    {
      ad = dlsym (full_program_dlhandle_HOGJIRPS (), d->routnam);
      if (!ad)
	{
	  fprintf (stderr, "%s: missing symbol %s (%s) [%s:%d]\n",
		   progname_HDGJIRPS, d->routnam, dlerror (),
		   __FILE__, __LINE__ - 2);
	  fflush (NULL);
	  return false;
	};
      d->routad = ad;
    }
  else
    ad = d->routad;
  if (pad)
    *pad = ad;
  if (pnam)
    *pnam = d->routnam;
  return true;
}				/* end get_namedrout_HDGJIRPS */


static pthread_mutex_t scalareg_mtx_HDGJIRPS = PTHREAD_MUTEX_INITIALIZER;
static void **scalareg_arrptr_HDGJIRPS[(unsigned) sca__lasttypid];
static unsigned scalareg_arrcnt_HDGJIRPS[(unsigned) sca__lasttypid];
static unsigned scalareg_arrsize_HDGJIRPS[(unsigned) sca__lasttypid];
static inline void scalareg_add_HDGJIRPS (struct header_hdgjirps_st *h);

void
scalareg_add_HDGJIRPS (struct header_hdgjirps_st *h)
{
  assert (h != NULL && is_valid_ptr_HDGJIRPS ((void *) h));
  int16_t curtypnum = h->typenum;
  if (curtypnum <= (int) sca__none || curtypnum > (int) sca__lasttypid)
    HDGJIRPS_FATAL ("corrupted scalar @%p of typenum#%d",
		    (void *) h, (int) curtypnum);
  uintptr_t n = ((uintptr_t) h) ^ (((uintptr_t) h) >> 9);
  void **arr = scalareg_arrptr_HDGJIRPS[curtypnum];
  unsigned siz = scalareg_arrsize_HDGJIRPS[curtypnum];
  unsigned cnt = scalareg_arrcnt_HDGJIRPS[curtypnum];
  unsigned lim = n % siz;
  assert (arr != NULL);
  assert (siz + siz / 2 < 2 * cnt);
  for (unsigned ix = lim; ix < siz; ix++)
    {
      if (!arr[ix])
	{
	  arr[ix] = (void *) h;
	  scalareg_arrcnt_HDGJIRPS[curtypnum]++;
	  return;
	}
    };
  for (unsigned ix = 0; ix < lim; ix++)
    {
      if (!arr[ix])
	{
	  arr[ix] = (void *) h;
	  scalareg_arrcnt_HDGJIRPS[curtypnum]++;
	  return;
	}
    };
  /// not supposed to be reached:
  HDGJIRPS_FATAL ("corrupted scalar array @%p of typenum#%d",
		  arr, (int) curtypnum);
}				/* end  scalareg_add_HDGJIRPS */

void
register_scalar_value_HDGJIRPS (void *ptr,
				enum scalar_typid_HDGJIRPS_en typcod)
{
  bool needgrow = false;
  assert (typcod > sca__none && typcod < sca__lasttypid);
  pthread_mutex_lock (&scalareg_mtx_HDGJIRPS);
  if (!is_valid_ptr_HDGJIRPS (ptr))
    goto end;
  const struct header_hdgjirps_st *had = ptr;
  if (had->typenum != (int16_t) typcod)
    goto end;
  void **oldarr = scalareg_arrptr_HDGJIRPS[typcod];
  unsigned oldcnt = scalareg_arrcnt_HDGJIRPS[typcod];
  unsigned oldsize = scalareg_arrsize_HDGJIRPS[typcod];
  if (!scalareg_arrptr_HDGJIRPS[typcod])
    needgrow = true;
  else
    needgrow = 4 * (oldcnt + 1) > 3 * oldsize;
  if (needgrow)
    {
      unsigned newsiz =
	((4 * scalareg_arrcnt_HDGJIRPS[typcod] + 100) | 0x1f) + 1;
      if (newsiz > INT_MAX / sizeof (void *))
	HDGJIRPS_FATAL ("out of memory when registering scalar value @%p"
			" of type#%d", ptr, (int) typcod);
      assert (newsiz > scalareg_arrsize_HDGJIRPS[typcod]);
      scalareg_arrptr_HDGJIRPS[typcod]
	= mallocx (newsiz * sizeof (void *),
		   MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
      scalareg_arrsize_HDGJIRPS[typcod] = newsiz;
      scalareg_arrcnt_HDGJIRPS[typcod] = 0;
      for (unsigned oldix = 0; oldix < oldsize; oldix++)
	{
	  if (oldarr[oldix])
	    scalareg_add_HDGJIRPS ((struct header_hdgjirps_st *)
				   oldarr[oldix]);
	};
    };
  scalareg_add_HDGJIRPS ((struct header_hdgjirps_st *) ptr);
end:
  pthread_mutex_unlock (&scalareg_mtx_HDGJIRPS);
}				/* end of register_scalar_value_HDGJIRPS */

/// end of file gccjit-refpersys/scalar.c
