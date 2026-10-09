/****************************************************************
 * file gccjit-refpersys/composite.c
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *      (Some gccjit variant to please indian programmers)
 *      It implements boxed composite values.
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

const char composite_shortgit_HDGJIRPS[] = SHORTGITID;



static pthread_mutex_t comporeg_mtx_HDGJIRPS = PTHREAD_MUTEX_INITIALIZER;
static void **comporeg_arrptr_HDGJIRPS[(unsigned) comp__lasttypid];
static unsigned comporeg_arrcnt_HDGJIRPS[(unsigned) comp__lasttypid];
static unsigned comporeg_arrsize_HDGJIRPS[(unsigned) comp__lasttypid];
static inline void comporeg_add_HDGJIRPS (struct header_hdgjirps_st *h);

void
comporeg_add_HDGJIRPS (struct header_hdgjirps_st *h)
{
  assert (h != NULL && is_valid_ptr_HDGJIRPS ((void *) h));
  int16_t curtypnum = -(h->typenum);
  if (curtypnum <= (int) comp__none || curtypnum > (int) comp__lasttypid)
    HDGJIRPS_FATAL ("corrupted composite @%p of typenum#%d",
		    (void *) h, (int) curtypnum);
  uintptr_t n = ((uintptr_t) h) ^ (((uintptr_t) h) >> 9);
  void **arr = comporeg_arrptr_HDGJIRPS[curtypnum];
  unsigned siz = comporeg_arrsize_HDGJIRPS[curtypnum];
  unsigned cnt = comporeg_arrcnt_HDGJIRPS[curtypnum];
  unsigned lim = n % siz;
  assert (arr != NULL);
  assert (siz + siz / 2 < 2 * cnt);
  for (unsigned ix = lim; ix < siz; ix++)
    {
      if (!arr[ix])
	{
	  arr[ix] = (void *) h;
	  comporeg_arrcnt_HDGJIRPS[curtypnum]++;
	  return;
	}
    };
  for (unsigned ix = 0; ix < lim; ix++)
    {
      if (!arr[ix])
	{
	  arr[ix] = (void *) h;
	  comporeg_arrcnt_HDGJIRPS[curtypnum]++;
	  return;
	}
    };
  /// not supposed to be reached:
  HDGJIRPS_FATAL ("corrupted composite array @%p of typenum#%d",
		  arr, (int) curtypnum);
}				/* end  comporeg_add_HDGJIRPS */

void
register_composite_value_HDGJIRPS (void *ptr,
				   enum composite_typid_HDGJIRPS_en typcod)
{
  bool needgrow = false;
  assert (typcod > comp__none && typcod < comp__lasttypid);
  pthread_mutex_lock (&comporeg_mtx_HDGJIRPS);
  if (!is_valid_ptr_HDGJIRPS (ptr))
    goto end;
  const struct header_hdgjirps_st *had = ptr;
  if (had->typenum != (int16_t) typcod)
    goto end;
  void **oldarr = comporeg_arrptr_HDGJIRPS[typcod];
  unsigned oldcnt = comporeg_arrcnt_HDGJIRPS[typcod];
  unsigned oldsize = comporeg_arrsize_HDGJIRPS[typcod];
  if (!comporeg_arrptr_HDGJIRPS[typcod])
    needgrow = true;
  else
    needgrow = 4 * (oldcnt + 1) > 3 * oldsize;
  if (needgrow)
    {
      unsigned newsiz =
	((4 * comporeg_arrcnt_HDGJIRPS[typcod] + 100) | 0x1f) + 1;
      if (newsiz > INT_MAX / sizeof (void *))
	HDGJIRPS_FATAL ("out of memory when registering composite value @%p"
			" of type#%d", ptr, (int) typcod);
      assert (newsiz > comporeg_arrsize_HDGJIRPS[typcod]);
      comporeg_arrptr_HDGJIRPS[typcod]
	= mallocx (newsiz * sizeof (void *),
		   MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
      comporeg_arrsize_HDGJIRPS[typcod] = newsiz;
      comporeg_arrcnt_HDGJIRPS[typcod] = 0;
      for (unsigned oldix = 0; oldix < oldsize; oldix++)
	{
	  if (oldarr[oldix])
	    comporeg_add_HDGJIRPS ((struct header_hdgjirps_st *)
				   oldarr[oldix]);
	};
    };
  comporeg_add_HDGJIRPS ((struct header_hdgjirps_st *) ptr);
end:
  pthread_mutex_unlock (&comporeg_mtx_HDGJIRPS);
}				/* end of register_composite_value_HDGJIRPS */
