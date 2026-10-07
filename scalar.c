/****************************************************************
 * file gccjit-refpersys/global.c
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
#pragma GCC poison malloc calloc free

const char scalar_shortgit_HDGJIRPS[] = SHORTGITID;

#define SCALAR_MALLOC_HDGJIRPS(Siz) \
  mallocx((Siz),		    \
	  MALLOCX_ZERO|MALLOCX_LG_ALIGN(4));



static_assert (alignof (struct boxint_hdgjirps_st) == (1 << 4));
static_assert (sizeof (struct boxint_hdgjirps_st) < 4 * sizeof (intptr_t));

struct boxint_hdgjirps_st *
make_box_int_HDGJIRPS (intptr_t v)
{
  static_assert (alignof (struct boxint_hdgjirps_st) == 16);
  struct boxint_hdgjirps_st *p = SCALAR_MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    HDGJIRPS_FATAL ("out of memory when boxing int %ld", (long) v);
  p->typenum = sca_boxed_int;
  p->gcmark = 0;
  p->flag = 0;
  p->xtranum = 0;
  p->intval = v;
  return p;
}				/* end make_box_int_HDGJIRPS */


struct boxint_hdgjirps_st *
make_bxtra_int_HDGJIRPS (intptr_t v, int32_t xtra)
{
  struct boxint_hdgjirps_st *p = SCALAR_MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    HDGJIRPS_FATAL ("out of memory when boxing int %ld", (long) v);
  p->typenum = sca_boxed_int;
  p->gcmark = 0;
  p->flag = 0;
  p->xtranum = xtra;
  p->intval = v;
  return p;
}				/* end make_bxtra_int_HDGJIRPS */

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

bool
get_int_xtra_HDGJIRPS (const void *ptr, intptr_t *p, int32_t *x)
{
  if (!ptr || !is_valid_ptr_HDGJIRPS (ptr))
    return false;
  const struct boxint_hdgjirps_st *d = (struct boxint_hdgjirps_st *) ptr;
  if (d->typenum != sca_boxed_int)
    return false;
  if (p)
    *p = d->intval;
  if (x)
    *x = d->xtranum;
  return true;
}				/* end get_int_xtra_HDGJIRPS */




///// boxed pair of integers
static_assert (alignof (struct boxtwoints_hdgjirps_st) == (1 << 4));
static_assert (sizeof (struct boxtwoints_hdgjirps_st) <
	       8 * sizeof (intptr_t));

struct boxtwoints_hdgjirps_st *
make_boxtwoints_HDGJIRPS (intptr_t v0, intptr_t v1)
{
  static_assert (alignof (struct boxtwoints_hdgjirps_st) == 16);
  struct boxtwoints_hdgjirps_st *p = SCALAR_MALLOC_HDGJIRPS (sizeof (*p));
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


struct boxtwoints_hdgjirps_st *
make_bxtratwoints_HDGJIRPS (intptr_t v0, intptr_t v1, int32_t xtra)
{
  struct boxtwoints_hdgjirps_st *p = SCALAR_MALLOC_HDGJIRPS (sizeof (*p));
  if (!p)
    HDGJIRPS_FATAL ("out of memory when boxing two ints %ld & %ld", (long) v0,
		    (long) v1);
  p->typenum = sca_boxed_twoints;
  p->gcmark = 0;
  p->flag = 0;
  p->xtranum = xtra;
  p->intpair[0] = v0;
  p->intpair[1] = v1;
  return p;
}				/* end make_bxtratwoints_HDGJIRPS */

bool
gettwoints_HDGJIRPS (const void *ptr, intptr_t *p0, intptr_t *p1)
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
}				/* end gettwoints_HDGJIRPS */

bool
gettwoints_xtra_HDGJIRPS (const void *ptr, intptr_t *p0, intptr_t *p1,
			  int32_t *x)
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
  if (x)
    *x = d->xtranum;
  return true;
}				/* end gettwoints_xtra_HDGJIRPS */

/// end of file gccjit-refpersys/scalar.c
