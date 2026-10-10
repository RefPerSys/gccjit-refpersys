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


static uint32_t hash_obid_HDGJIRPS (uint32_t hi, uint64_t lo);

static pthread_mutex_t comporeg_mtx_HDGJIRPS
  = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static void **comporeg_arrptr_HDGJIRPS[(unsigned) comp__lasttypid];
static unsigned comporeg_arrcnt_HDGJIRPS[(unsigned) comp__lasttypid];
static unsigned comporeg_arrsize_HDGJIRPS[(unsigned) comp__lasttypid];
static inline void comporeg_add_HDGJIRPS (struct header_hdgjirps_st *h);

static pthread_mutex_t object_mtx_HDGJIRPS
  = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

static struct object_hdgjirps_st **object_arrptr_HDGJIRPS;
static unsigned object_arrcnt_HDGJIRPS;
static unsigned object_arrsize_HDGJIRPS;
static inline void object_add_HDGJIRPS (struct object_hdgjirps_st *pob);

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

uint32_t
hash_obid_HDGJIRPS (uint32_t hi, uint64_t lo)
{
  uint32_t h = hi ^ (lo >> 32);
  assert (hi != 0 && lo != 0);
  if (h == 0)
    h = 1 + (lo & 0x7fffffff);
  assert (h != 0);
  return h;
}

uint32_t
hash_object_HDGJIRPS (struct object_hdgjirps_st *ob)
{
  if (!ob)
    return 0;
  if (!is_valid_ptr_HDGJIRPS ((void *) ob))
    return 0;
  if (ob->typenum != -(int) comp_boxed_object)
    return 0;
  return hash_obid_HDGJIRPS (ob->ob_idhi, ob->ob_idlo);
}				/* end hash_object_HDGJIRPS */

struct object_hdgjirps_st *
find_object_HDGJIRPS (uint32_t hi, uint64_t lo)
{
  struct object_hdgjirps_st *resob = NULL;
  if (hi == 0 || lo == 0)
    return NULL;
  uint32_t h = hash_obid_HDGJIRPS (hi, lo);
  pthread_mutex_lock (&object_mtx_HDGJIRPS);
  if (object_arrcnt_HDGJIRPS == 0)
    goto end;
  assert (object_arrptr_HDGJIRPS != NULL);
  assert (object_arrsize_HDGJIRPS > 0);
  assert (object_arrcnt_HDGJIRPS < object_arrsize_HDGJIRPS);
  unsigned begix = h % object_arrsize_HDGJIRPS;
  for (unsigned ix = begix; ix < object_arrsize_HDGJIRPS; ix++)
    {
      struct object_hdgjirps_st *curob = object_arrptr_HDGJIRPS[ix];
      if (!curob)
	break;
      if (curob->ob_idhi == hi && curob->ob_idlo == lo)
	{
	  resob = curob;
	  goto end;
	};
    };
  for (unsigned ix = 0; ix < begix; ix++)
    {
      struct object_hdgjirps_st *curob = object_arrptr_HDGJIRPS[ix];
      if (!curob)
	break;
      if (curob->ob_idhi == hi && curob->ob_idlo == lo)
	{
	  resob = curob;
	  goto end;
	};
    };
end:
  pthread_mutex_unlock (&object_mtx_HDGJIRPS);
  return resob;
}				/* end find_object_HDGJIRPS */

void
object_add_HDGJIRPS (struct object_hdgjirps_st *pob)
{
  assert (is_valid_ptr_HDGJIRPS (pob));
  assert (pob->typenum == -comp_boxed_object);
  uint32_t hob = hash_obid_HDGJIRPS (pob->ob_idhi, pob->ob_idlo);
  pthread_mutex_lock (&object_mtx_HDGJIRPS);
  assert (object_arrptr_HDGJIRPS != NULL);
  assert (object_arrsize_HDGJIRPS != 0);
  assert (object_arrcnt_HDGJIRPS < object_arrsize_HDGJIRPS);
  unsigned startix = hob % object_arrsize_HDGJIRPS;
  assert (hob != 0);
  for (unsigned ix = startix; ix < object_arrsize_HDGJIRPS; ix++)
    {
      if (!object_arrptr_HDGJIRPS[ix])
	{
	  object_arrcnt_HDGJIRPS++;
	  object_arrptr_HDGJIRPS[ix] = pob;
	  goto end;
	};
    };
  for (unsigned ix = 0; ix < startix; ix++)
    {
      if (!object_arrptr_HDGJIRPS[ix])
	{
	  object_arrcnt_HDGJIRPS++;
	  object_arrptr_HDGJIRPS[ix] = pob;
	  goto end;
	};
    }
end:
  pthread_mutex_unlock (&object_mtx_HDGJIRPS);
}				/* end object_add_HDGJIRPS */


struct node_hdgjirps_st *
make_node_vect_HDGJIRPS (struct object_hdgjirps_st *ob, unsigned nbsons,
			 void **sontab)
{
  struct node_hdgjirps_st *res = NULL;
  if (!ob || !is_valid_ptr_HDGJIRPS (ob))
    return NULL;
  if (ob->typenum != comp_boxed_object)
    return NULL;
  res = mallocx (sizeof (struct node_hdgjirps_st) + nbsons * sizeof (void *),
		 MALLOCX_ZERO | MALLOCX_ALIGN (1 << 4));
  uint32_t h = hash_object_HDGJIRPS (ob) ^ (nbsons & 0xfff);
  for (unsigned ix = 0; ix < nbsons; ix++)
    {
      void *curson = sontab ? sontab[ix] : NULL;
      if (curson && is_valid_ptr_HDGJIRPS (curson))
	{
	  struct header_hdgjirps_st *hd =
	    (struct header_hdgjirps_st *) curson;
	  h = (h * 1307 + ix) ^ (hd->hash);
	  res->nod_sons[ix] = curson;
	}
      else
	h = h + ix;
    };
  res->typenum = -(int) comp_boxed_node;
  res->hash = h;
  res->nod_obj = ob;
  res->gcmark = 0;
  res->flag = 0;
  register_composite_value_HDGJIRPS (res, comp_boxed_node);
  return res;
} /* end make_node_vect_HDGJIRPS */

struct object_hdgjirps_st *
make_object_HDGJIRPS (void)
{
  struct object_hdgjirps_st *resob = NULL;
  pthread_mutex_lock (&object_mtx_HDGJIRPS);
  if (4 * object_arrcnt_HDGJIRPS + 5 > 3 * object_arrsize_HDGJIRPS)
    {
      struct object_hdgjirps_st **oldarr = object_arrptr_HDGJIRPS;
      unsigned oldcnt = object_arrcnt_HDGJIRPS;
      unsigned oldsiz = object_arrsize_HDGJIRPS;
      unsigned newsiz = 1 + ((4 * oldcnt / 3 + 20) | 0x3f);
      object_arrptr_HDGJIRPS =
	mallocx (newsiz * sizeof (void *),
		 MALLOCX_ZERO | MALLOCX_ALIGN (1 << 8));
      object_arrcnt_HDGJIRPS = 0;
      object_arrsize_HDGJIRPS = newsiz;
      for (unsigned ix = 0; ix < oldsiz; ix++)
	if (oldarr[ix] != NULL)
	  object_add_HDGJIRPS (oldarr[ix]);
    };
  while (resob == NULL)
    {
      uint32_t hi = randomi32_HDGJIRPS ();
      uint64_t lo = randomi64_HDGJIRPS ();
      if (hi < 0x100 || lo < 0x100)
	continue;
      if (find_object_HDGJIRPS (hi, lo))
	continue;
      resob = MALLOC_HDGJIRPS (sizeof (struct object_hdgjirps_st));
      if (!resob)
	goto end;
      resob->typenum = -(int) comp_boxed_object;
      resob->hash = hash_obid_HDGJIRPS (hi, lo);
      resob->gcmark = 0;
      resob->flag = 0;
      resob->ob_idhi = hi;
      resob->ob_idlo = lo;
      object_add_HDGJIRPS (resob);
    };
end:
  pthread_mutex_unlock (&object_mtx_HDGJIRPS);
  return resob;
}				/* end make_object_HDGJIRPS */

/* end of file gccjit-refpersys/composite.c */
