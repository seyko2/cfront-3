/*ident	"@(#)Array_alg:incl/Array_alg.h	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#ifndef ARRAY_ALGH
#define ARRAY_ALGH

#include <generic.h>
#if defined(__SUNPRO_CC)
extern "C" double drand48();
#else
#include <rand48.h>
#endif
#include <stddef.h>

template <class T>
const T* 
bin_loc(
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
const T*
bin_loc_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
const T* 
bin_search(
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
const T*
bin_search_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
void 
copy(
	T* begin,
	T* end,
	T* result
);

template <class T>
ptrdiff_t 
count(
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
ptrdiff_t 
count_p(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end
);

template <class T>
ptrdiff_t 
count_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
T*
set_diff(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
T* 
set_diff_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
void 
fill(
	const T& value,
	T* begin,
	T* end
);

template <class T>
void 
for_each(
	void (*function)(T*),
	T* begin,
	T* end
);

template <class T>
void 
generate(
	void (*function)(ptrdiff_t, T*),
	T* begin,
	T* end
);

template <class T>
T* 
insert(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
insert_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
void 
ins_sort(
	T* begin,
	T* end
);

template <class T>
void 
ins_sort_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
ins_sort_chunks(
	ptrdiff_t number,
	T* begin,
	T* end
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
ins_sort_chunks_r(
	int (*rel_ptr)(const T*, const T*),
	ptrdiff_t number,
	T* begin,
	T* end
);

template <class T>
T* 
set_inter(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
T* 
set_inter_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
void 
merge(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
void 
merge_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
T* 
merge_sort(
	T* begin,
	T* end,
	T* result
);

template <class T>
T* 
merge_sort_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end,
	T* result
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
merge_sort_step(
	ptrdiff_t number,
	T* begin,
	T* end,
	T* result
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
merge_sort_step_r(
	int (*rel_ptr)(const T*, const T*),
	ptrdiff_t number,
	T* begin,
	T* end,
	T* result
);

template <class T>
const T* 
minimum(
	const T* begin,
	const T* end
);

template <class T>
const T* 
minimum_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin,
	const T* end
);

template <class T>
const T* 
mismatch(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2
);

template <class T>
const T* 
mismatch_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
T*
ordered_part_ATTLC(
	register T *begin,
	register T *end
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
T* 
ordered_part_r_ATTLC(
	register int (*rel_ptr)(const T*, const T*),
	register T* begin,
	register T* end
);

template <class T>
T* 
part(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
part_p(
	int (*pred_ptr)(const T*),
	T* begin,
	T* end
);

template <class T>
T* 
part_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
part_c(
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
part_pc(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
part_rc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
const T* 
pos(
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
const T* 
pos_p(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end
);

template <class T>
const T* 
pos_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
quicksort_loop_ATTLC(
	register T *begin,
	register T *end
);

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
quicksort_loop_r_ATTLC(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end
);

template <class T>
const T* 
random(
	const T* begin,
	const T* end
);

template <class T>
T* 
rem(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
rem_p(
	int (*pred_ptr)(const T*),
	T* begin,
	T* end
);

template <class T>
T* 
rem_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
rem_c(
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
rem_pc(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
rem_rc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
rem_dup(
	T* begin,
	T* end
);

template <class T>
T*
rem_dup_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end
);

template <class T>
T* 
rem_dup_c(
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T*
rem_dup_rc(
	int (*rel_ptr)(const T*, const T*),
	const T* begin,
	const T* end,
	T* result
);

template <class T>
void 
reverse(
	T* begin,
	T* end
);

template <class T>
void 
reverse_c(
	const T* begin,
	const T* end,
	T* result
);

template <class T>
const T* 
rt_pos(
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
const T* 
rt_pos_p(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end
);

template <class T>
const T* 
rt_pos_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end
);

template <class T>
void 
rotate(
	ptrdiff_t number,
	T* begin,
	T* end
);

template <class T>
void 
rotate_c(
	ptrdiff_t number,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
const T* 
search(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2
);

template <class T>
const T* 
search_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2
);

template <class T>
void 
select(
	ptrdiff_t nth,
	T* begin,
	T* end
);

template <class T>
void 
select_r(
	int (*rel_ptr)(const T*, const T*),
	ptrdiff_t nth,
	T* begin,
	T* end
);

template <class T>
T* 
set_insert(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
set_insert_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
set_remove(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
set_remove_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
set_union(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
T* 
set_union_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
void 
shuffle(
	T* begin,
	T* end
);

template <class T>
void 
shuffle_c(
	const T* begin,
	const T* end,
	T* result
);

template <class T>
void 
sort(
	T* begin,
	T* end
);

template <class T>
void 
sort_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end
);

template <class T>
T* 
part_s(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
part_ps(
	int (*pred_ptr)(const T*),
	T* begin,
	T* end
);

template <class T>
T* 
part_rs(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
part_sc(
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
part_psc(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
part_rsc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
rem_s(
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
rem_ps(
	int (*pred_ptr)(const T*),
	T* begin,
	T* end
);

template <class T>
T* 
rem_rs(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end
);

template <class T>
T* 
rem_sc(
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
rem_psc(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
rem_rsc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
void 
sort_s(
	T* begin,
	T* end
);

template <class T>
void 
sort_rs(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end
);

template <class T>
void 
subs(
	const T& value,
	T new_value,
	T* begin,
	T* end
);

template <class T>
void 
subs_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T new_value,
	T* begin,
	T* end
);

template <class T>
void 
subs_c(
	const T& value,
	T new_value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
void 
subs_rc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T new_value,
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
set_sdiff(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
T* 
set_sdiff_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2,
	T* result
);

template <class T>
T* 
unique(
	T* begin,
	T* end
);

template <class T>
T* 
unique_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end
);

template <class T>
T* 
unique_c(
	const T* begin,
	const T* end,
	T* result
);

template <class T>
T* 
unique_rc(
	int (*rel_ptr)(const T*, const T*),
	const T* begin,
	const T* end,
	T* result
);

#if defined(__edg_att_40) && !defined(__IMPLICIT_INCLUDE)
#include <Array_alg.c>
#endif
#endif
