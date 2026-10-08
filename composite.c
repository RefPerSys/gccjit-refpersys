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
