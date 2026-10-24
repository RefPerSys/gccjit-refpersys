/****************************************************************
 * file gccjit-refpersys/hdgjirps.h
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System
 *      (the gccjit-refpersys variant which might be accepted in India)
 *      It is its header file
 *
 * Author(s):
 *      Basile Starynkevitch, France   <basile@starynkevitch.net>
 *
 *      © Copyright (C) 2026 Basile Starynkevitch
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

#define _GNU_SOURCE

#ifndef HDGJIRPS_INCLUDED
#define HDGJIRPS_INCLUDED

#ifndef SHORTGITID
#error compilation command without SHORTGITID
#endif

#include <stdlib.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <zlib.h>
#include <unistr.h>
#include <math.h>
#include <dlfcn.h>
#include <pthread.h>
#include <threads.h>
#include <backtrace.h>
#include <readline/readline.h>
#include <unistring/version.h>
#include <gnu/libc-version.h>
#include "unistr.h"		/* www.gnu.org/software/libunistring */
#include "jemalloc/jemalloc.h"	/* see jemalloc.net */
#include <bsd/string.h>		/* for strnstr(3) */
#include <string.h>

#include <libgccjit.h>		/* see gcc.gnu.org/onlinedocs/jit/ */

typedef struct _object_HDGJIRPS_st Obj_HDGJIRPS;

extern gcc_jit_context *jitctx_HDGJIRPS;
extern const char *progname_HDGJIRPS;
extern const char *loadpath_HDGJIRPS;
extern char **argv_HDGJIRPS;
extern int argc_HDGJIRPS;
extern char hostname_HDGJIRPS[64];
extern const char *zlibv_HDGJIRPS;
extern struct backtrace_state *backtrace_state_HDGJIRPS;
extern const char shortgitid_HDGJIRPS[];
extern const char sourcedir_HDGJIRPS[];
extern char full_source_main_HDGJIRPS[];
extern char executable_HDGJIRPS[128];
extern int verbose_HDGJIRPS;
extern pthread_mutex_t globmtx_HDGJIRPS;
struct load_data_HDGJIRPS_st;


extern double wallclock_real_time_HDGJIRPS (void);
extern double monotonic_real_time_HDGJIRPS (void);
extern double process_cpu_time_HDGJIRPS (void);
extern double thread_cpu_time_HDGJIRPS (void);

#pragma GCC poison malloc calloc free
#define MALLOC_HDGJIRPS(Siz) \
  mallocx((Siz),		    \
	  MALLOCX_ZERO|MALLOCX_LG_ALIGN(4));
extern void
emit_gplv3_notice_AT_HDGJIRPS (FILE *fout, const char *fil, int lin,
			       const char *fromfun, const char *path,
			       const char *linprefix, const char *linsuffix,
			       char *explain);

extern int32_t randomi32_HDGJIRPS (void);
extern int64_t randomi64_HDGJIRPS (void);

extern void *full_program_dlhandle_HDGJIRPS (void);

/// convert an unsigned 64 bits  to its string representation in base 36
/// the reverse is done by strtoll; return true if successful
extern bool uint64b36_HDGJIRPS (uint64_t i, char *buf, int bufsiz);

#define LOADMAGIC_HDGJIRPS 0x3eb03561	/* 1051735393 */

extern void load_data_HDGJIRPS (struct load_data_HDGJIRPS_st *ld);
extern void *loaded_value_HDGJIRPS (struct load_data_HDGJIRPS_st *ld);
extern void load_fill_object_HDGJIRPS (struct load_data_HDGJIRPS_st *ld,
				       Obj_HDGJIRPS *pob);
/// This loading state is called after a successful mmap of path
/// (start -> end)
extern void load_state_HDGJIRPS (const char *path, const char *start,
				 const char *last);

extern Obj_HDGJIRPS *make_loaded_object_HDGJIRPS (struct load_data_HDGJIRPS_st
						  *ld, uint32_t hi,
						  uint32_t lo);

extern void
write_state_HDGJIRPS (const char *path, void **tabptr, size_t siztab);

extern const uint32_t loadmagic_HDGJIRPS;
struct load_data_HDGJIRPS_st
{
  uint32_t lda_magic;		/* aload loadmagic_HDGJRPS */
  uint32_t lda_lineno;
  const char *lda_path;
  void *lda_start;
  void *lda_cur;
  void *lda_end;
  /* todo: we probably want a stack of values and a load protocol
     inspired by concepts from FORTH, PDF, PostScript, NeWS stack
     operations */
};

extern void load_skip_spaces_HDGJIRPS (struct load_data_HDGJIRPS_st *ld);

#define DUMPMAGIC_HDGJIRPS 0x3d4a10bf	/* 1028264127 */

extern const uint32_t dump_magic_HDGJIRPS;
struct dump_data_HDGJIRPS_st
{
  uint32_t dump_magic;		/* always dump_magic_HDGJRPS */
  uint16_t dump_indent;
  const char *dump_path;
  FILE *dump_file;
  long dump_bol;		/* offset of last line */
  void *dump_data;
};				/* end struct dump_data_HDGJIRPS_st */

extern void breakpoint_at_HDGJIRPS (const char *fil, int lin, char kind);

#define HDGJIRPS_FATAL_AT_BIS(Fil,Lin,Func,Fmt,...) do {	\
    char thrname##Lin[32];					\
    memset(thrname##Lin, 0, sizeof(thrname##Lin));		\
    pthread_getname_np(pthread_self(), thrname##Lin,		\
                       sizeof(thrname##Lin));			\
    fprintf (stderr, "%s:%d:%s [%s]", (Fil), (Lin),		\
             (Func), thrname##Lin);				\
    fprintf (stderr, "FATAL ERROR\n");				\
    fprintf (stderr, Fmt "\n", ##__VA_ARGS__);			\
    fprintf (stderr, "%s: shortgit %s pid %d\n",		\
             progname_HDGJIRPS, shortgitid_HDGJIRPS,		\
             (int)getpid());					\
    fflush (stderr);						\
    if (backtrace_state_HDGJIRPS)				\
      backtrace_print (backtrace_state_HDGJIRPS, 1,		\
		       stderr);					\
    fflush(NULL);						\
    breakpoint_at_HDGJIRPS((Fil),(Lin), 'F');                   \
    abort(); } while(0)

#define HDGJIRPS_FATAL_AT(Fil,Lin,Func,Fmt,...) \
   HDGJIRPS_FATAL_AT_BIS(Fil,Lin,Func,Fmt,##__VA_ARGS__)

#define HDGJIRPS_FATAL(Fmt,...)  HDGJIRPS_FATAL_AT(__FILE__,__LINE__,__FUNCTION__,Fmt,##__VA_ARGS__)


#define HDGJIRPS_WARNING_AT_BIS(Fil,Lin,Func,Fmt,...) do {	\
    char thrname##Lin[32];					\
    memset(thrname##Lin, 0, sizeof(thrname##Lin));		\
    pthread_getname_np(pthread_self(), thrname##Lin,		\
                       sizeof(thrname##Lin));			\
    fprintf (stderr, "%s:%d:%s [%s]", (Fil), (Lin),		\
             (Func), thrname##Lin);				\
    fprintf (stderr, "WARNING\n");				\
    fprintf (stderr, Fmt "\n", ##__VA_ARGS__);			\
    fprintf (stderr, "%s: shortgit %s pid %d\n",		\
             progname_HDGJIRPS, shortgitid_HDGJIRPS,		\
             (int)getpid());					\
    fflush (stderr);						\
    if (backtrace_state_HDGJIRPS)				\
      backtrace_print (backtrace_state_HDGJIRPS, 1,		\
		       stderr);					\
    fflush(NULL);						\
    breakpoint_at_HDGJIRPS((Fil),(Lin), 'W');                   \
    } while(0)

#define HDGJIRPS_WARNING_AT(Fil,Lin,Func,Fmt,...) \
   HDGJIRPS_WARNING_AT_BIS(Fil,Lin,Func,Fmt,##__VA_ARGS__)

#define HDGJIRPS_WARNING(Fmt,...) \
  HDGJIRPS_WARNING_AT(__FILE__,__LINE__,__FUNCTION__,Fmt,##__VA_ARGS__)

extern int get_escape_sequence_HDGHIRPS (const char **ps, bool instring,
					 bool *badseq);

#define HDGJIRPS_TWO_WORDS_ALIGNED __attribute__((aligned(2*sizeof(void*))))
// by convention positive typenums are for scalar values and negative
// ones are for composite values.
#define HDGJIRPS_HEADER_FIELDS			\
  int16_t HDGJIRPS_TWO_WORDS_ALIGNED typenum;	\
  uint8_t  gcmark;				\
  uint8_t  flag;				\
  uint32_t length;				\
  uint32_t hash;                                \
  uint32_t xtranum

struct header_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;
};


//// scalar types values for typenum
enum scalar_typid_HDGJIRPS_en
{
  sca__none,
  sca_boxed_int,
  sca_boxed_twoints,
  sca_boxed_double,
  sca_boxed_twodoubles,
  sca_boxed_string,
  sca_boxed_namedrout,
  sca__lasttypid
};

extern void register_scalar_value_HDGJIRPS (void *ptr,
					    enum scalar_typid_HDGJIRPS_en
					    typcod);


struct boxint_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;	//
  intptr_t intval;
};
struct boxint_hdgjirps_st *make_box_int_HDGJIRPS (intptr_t v);
bool get_int_HDGJIRPS (const void *ptr, intptr_t * p);
struct boxtwoints_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;	//
  intptr_t intpair[2];
};

struct boxtwoints_hdgjirps_st *make_boxtwoints_HDGJIRPS (intptr_t v0,
							 intptr_t v1);
bool get_twoints_HDGJIRPS (const void *ptr, intptr_t * p0, intptr_t * p1);




struct boxdouble_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;	//
  double dblval;
};

unsigned hashstr_HDGJIRPS (const char *s);

struct boxdouble_hdgjirps_st *make_box_double_HDGJIRPS (double v);
bool get_double_HDGJIRPS (const void *ptr, double *p);


//-  struct boxtwodbls_hdgjirps_st
//-  {
//-    HDGJIRPS_HEADER_FIELDS;  //
//-    double dblpair[2];
//-  };

struct string_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;	//
  char cstr[];
};
struct string_hdgjirps_st *make_string_HDGJIRPS (const char *str);
struct string_hdgjirps_st *make_sized_string_HDGJIRPS (const char *str,
						       int bytesize);
bool get_string_length_HDGJIRPS (void *ptr, const char **pstr,
				 unsigned *pslen);

bool get_string_HDGJIRPS (void *ptr, const char **pstr);

//- struct intvect_hdgjirps_st
//- {
//-   HDGJIRPS_HEADER_FIELDS;   //
//-   intptr_t intarr[];
//- };
//-
//- struct dblvect_hdgjirps_st
//- {
//-   HDGJIRPS_HEADER_FIELDS;   //
//-   double dblarr[];
//- };

#define NAMEDROUT_LENGTH_HDGJIRPS 48
struct namedrout_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;	//
  void *routad;
  const char routnam[NAMEDROUT_LENGTH_HDGJIRPS];
};
struct namedrout_hdgjirps_st *make_namedrout_HDGJIRPS (const char *str);
bool get_namedrout_HDGJIRPS (const void *, void **pad, const char **pnam);


////////////////////////////////////////////////////////////////
/////// composite values

//// composite types values for typenum
enum composite_typid_HDGJIRPS_en
{
  comp__none,
  comp_boxed_node,
  comp_boxed_set,
  comp_boxed_object,
  comp__lasttypid
};

extern void register_composite_value_HDGJIRPS (void *ptr,
					       enum
					       composite_typid_HDGJIRPS_en
					       typcod);

extern const uint32_t minhi_obid_HDGJIRPS;
extern const uint64_t minlo_obid_HDGJIRPS;

struct set_hdgjirps_st;
struct node_hdgjirps_st;

struct node_hdgjirps_st
{
  HDGJIRPS_HEADER_FIELDS;
  Obj_HDGJIRPS *nod_obj;
  void *nod_sons[];
};

struct node_hdgjirps_st *make_node_vect_HDGJIRPS (Obj_HDGJIRPS
						  *ob, unsigned nbsons,
						  void **sontab);
struct node_hdgjirps_st *make_node_args_HDGJIRPS (Obj_HDGJIRPS *ob, ...);
#define MAKE_VNODE_HDGJIRPS(Ob,...) make_node_args_HDGJIRPS((Ob), #__VA_ARGS__, NULL);
struct compvect_hdgjirps_st;	/// in composite.c
struct attrvect_hdgjirps_st;	/// in composite.c
struct _object_HDGJIRPS_st	// aliased as Obj_HDGJIRPS
{
  HDGJIRPS_HEADER_FIELDS;
  pthread_mutex_t ob_mtx;
  uint32_t ob_idhi;
  uint64_t ob_idlo;
  struct attrvect_hdgjirps_st *ob_attrv;
  struct compvect_hdgjirps_st *ob_compv;
};				/* end struct _object_hdgjirps_st a.k.a Obj_HDGJIRPS */

inline bool
is_valid_ptr_HDGJIRPS (const void *ptr)
{
  if (((intptr_t) ptr & 0xf) != 0)
    return false;
  if (((intptr_t) ptr & ~0xf) == 0)
    return false;
  const struct header_hdgjirps_st *had = ptr;
  switch (had->typenum)
    {
    case (int) sca_boxed_int:
    case (int) sca_boxed_twoints:
    case (int) sca_boxed_double:
    case (int) sca_boxed_twodoubles:
    case (int) sca_boxed_string:
    case (int) sca_boxed_namedrout:
    case (int) -comp_boxed_node:
    case (int) -comp_boxed_set:
    case (int) -comp_boxed_object:
      return true;
    };
  return false;
}				/* end is_valid_ptr_HDGJIRPS */

extern Obj_HDGJIRPS *load_object_HDGJIRPS (struct load_data_HDGJIRPS_st *ld);
extern Obj_HDGJIRPS *find_object_HDGJIRPS (uint32_t hi, uint64_t lo);
extern Obj_HDGJIRPS *make_object_HDGJIRPS (void);
extern uint32_t hash_object_HDGJIRPS (Obj_HDGJIRPS *ob);

#endif /*HDGJIRPS_INCLUDED */
/*end of hdgjirps.h file */
