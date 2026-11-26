/****************************************************************
 * file gccjit-refpersys/loader.c
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *   This file is part of the Reflective Persistent System.
 *  the variant aiming to use libgccjit from gcc.gnu.org It is
 *  implementing the loading routines (called by
 *  RUN_LOADING_ROUTINE_HDGJIRPS) for persistence (in a single textual
 *  file)
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

#include "hdgjirps.h"

extern load_routine_sig_HDGJIRPS loader_echo_HDGJIRPS;

/// for @echo '<MESSAGE> 
void
loader_echo_HDGJIRPS (struct load_data_HDGJIRPS_st *ldp, long num)
{
  assert (ldp && ldp->lda_magic == LOADMAGIC_HDGJIRPS);
  const char *msg = ldp->lda_cur + num;
  assert ((void *) msg < ldp->lda_end);
  printf ("%s:%d: ° %s\n", ldp->lda_path, ldp->lda_lineno, msg);
  fflush (NULL);
}                               // end loader_echo_HDGJIRPS  


/****************
 **                           for Emacs...
 ** Local Variables: ;;
 ** compile-command: "make -j2 gccjit-refpersys" ;;
 ** End: ;;
 ****************/
/// end of file gccjit-refpersys/loader.c
