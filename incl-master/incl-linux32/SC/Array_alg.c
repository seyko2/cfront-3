/*ident	"@(#)Array_alg:incl/Array_alg.c	3.1" */
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

#ifndef _ARRAY_ALG_C_
#define _ARRAY_ALG_C_

template <class T>
const T*
bin_loc(
	register const T& value,
	register const T* begin,
	register const T* end)
{
	while ( 5 < end - begin ) {
		register const T *index = begin + ((end - begin) >> 1);

		if (value < *index)
			end = index;
		else
			begin = index + 1;
	}

	while ( begin < end )
		if ( !(value < *--end) )
			return end + 1;

	return end;
}

template <class T>
const T*
bin_loc_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	register const T* begin,
	register const T* end)
{
	register const T *temp = &value;

	while ( 5 < end - begin ) {
		register const T *index = begin + ((end - begin) >> 1);

		if ( (*rel_ptr)(temp, index) < 0 )
			end = index;
		else
			begin = index + 1;
	}

	while ( begin < end )
		if ( !((*rel_ptr)(temp, --end) < 0) )
			return end + 1;

	return end;
}

template <class T>
const T* 
bin_search(
	const T &value,
	const T* begin,
	const T* end)
{
	const T *index = bin_loc(value, begin, end);

	if ( begin < index && !(*(index - 1) < value) )
		return index - 1;
	else
		return 0;
}

template <class T>
const T*
bin_search_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end)
{
	const T *index = bin_loc_r(rel_ptr, value, begin, end);

	if ( begin < index && !((*rel_ptr)(index - 1, &value) < 0) )
		return index - 1;
	
	return 0;
}

template <class T>
void
copy(
	register T* begin,
	register T* end,
	register T* result)
{
	if ( begin < result ) {
		result += end - begin;
		while ( begin < end )
			*--result = *--end;
	}
	else if ( result < begin )
		while (begin < end)
			*result++ = *begin++;
}

template <class T>
ptrdiff_t 
count(
	register const T& value,
	register const T* begin,
	register const T* end)
{
	register ptrdiff_t n = 0;

	while ( begin < end )
		if ( *begin++ == value )
			n++;

	return n;
}

template <class T>
ptrdiff_t 
count_p(
	register int (*pred_ptr)(const T*),
	register const T* begin,
	register const T* end)
{
	register ptrdiff_t n = 0;

	while ( begin < end )
		if ( (*pred_ptr)(begin++) )
			n++;

	return n;
}

template <class T>
ptrdiff_t 
count_r(
	register int (*rel_ptr)(const T*, const T*),
	const T& value,
	register const T* begin,
	register const T* end)
{
	register const T *temp = &value;
	register ptrdiff_t n = 0;

	while ( begin < end )
		if ( (*rel_ptr)(begin++, temp) == 0 )
			n++;

	return n;
}


template <class T>
void 
fill(
	register const T& value,
	register T* begin,
	register T* end)
{
	while ( begin < end )
		*begin++ = value;
}

template <class T>
void 
for_each(
	register void (*function)(T*),
	register T* begin,
	register T* end)
{
	while ( begin < end )
		(*function)(begin++);
}

template <class T>
void 
generate(
	void (*function)(ptrdiff_t, T*),
	register T* begin,
	register T* end)
{
	register T *index = begin;

	for ( ; begin < end ; begin++ )
		(*function)(begin - index, begin);
}

template <class T>
T* 
insert(
	const T &value,
	T* begin,
	T* end)
{
	begin = (T*)bin_loc(value, begin, end);
	copy(begin, end, begin + 1);
	*begin = value;
	return begin;
}

template <class T>
T* 
insert_r(
	int (*rel_ptr)(const T*, const T*),
	const T&	value,
	T*		begin,
	T*		end)
{
	begin = (T*)bin_loc_r(rel_ptr, value, begin, end);
	copy(begin, end, begin + 1);
	*begin = value;
	return begin;
}

template <class T>
const T* 
minimum(
	register const T* begin,
	register const T* end)
{
	register const T *index = begin;

	while ( ++begin < end )
		if ( *begin < *index )
			index = begin;

	return index;
}

template <class T>
void 
ins_sort(
	register T* begin,
	register T* end)
{
	if ( end - begin < 2 )		// size < 2
		return;
	
	T *r = (T*)minimum(begin, end);	// create a sentinel

	T temp = *begin;		// swap
	*begin = *r;
	*r = temp;

	begin++;			// there is no need to insert
					// the second element

	while ( ++begin < end ) {
		register T value = *begin;
		register T *index = begin;

		while ( value < *--index )
			*(index + 1) = *index;
		
		*(index + 1) = value;
	}
}

template <class T>
void 
ins_sort_r(
	register int (*rel_ptr)(const T*, const T*),
	register T* begin,
	T* end)
{
	register T *index = begin;

	while ( ++index < end ) {
		T value = *index;
		register T *temp = &value;
		register T *current = index;

		for ( ; begin < current && (*rel_ptr)(temp , current - 1) < 0 ; current-- )
			*current = *(current - 1);

		*current = value;
	}
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
ins_sort_chunks(
	ptrdiff_t number,
	T* begin,
	T* end)
{
	for ( ; begin + number < end ; begin += number )
		ins_sort(begin, begin + number);

	ins_sort(begin, end);
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
ins_sort_chunks_r(
	int (*rel_ptr)(const T*, const T*),
	ptrdiff_t number,
	T* begin,
	T* end)
{
	for ( ;  begin + number < end ; begin += number )
		ins_sort_r(rel_ptr, begin, begin + number);

	ins_sort_r(rel_ptr, begin, end);
}

template <class T>
T* 
set_inter(
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 )
		if ( *begin2 < *begin1 )
			begin2++;
		else if ( *begin1 < *begin2 )
			begin1++;
		else {
			*(result++) = *(begin1++);
			begin2++;
		}

	return result;
}

template <class T>
T* 
set_inter_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 ) {
		register int c = (*rel_ptr)(begin2, begin1);

		if (c < 0)
			begin2++;
		else if (0 < c)
			begin1++;
		else {
			*(result++) = *(begin1++);
			begin2++;
		}
	}

	return result;
}

template <class T>
void 
merge(
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 )
		*result++ = (*begin2 < *begin1 ? *begin2++ : *begin1++);

	while ( begin1 < end1 )
		*result++ = *begin1++;

	while ( begin2 < end2 )
		*result++ = *begin2++;
}

template <class T>
void 
merge_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 )
		*result++ = ((*rel_ptr)(begin2, begin1) < 0 ? *begin2++ : *begin1++);

	while ( begin1 < end1 )
		*result++ = *begin1++;

	while ( begin2 < end2 )
		*result++ = *begin2++;
}

template <class T>
T* 
merge_sort(
	T* begin,
	T* end,
	T* result)
{
	ptrdiff_t number = 8, length = end - begin;

	ins_sort_chunks(number, begin, end);

	for ( ; number < length ; number += number, end = begin, begin = result, result = end )
		merge_sort_step(number, begin, begin + length, result);

	return begin;
}

template <class T>
T* 
merge_sort_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end,
	T* result)
{
	T* tmp;
	ptrdiff_t number = 8, length = end - begin;

	ins_sort_chunks_r(rel_ptr, number, begin, end);

	for ( ; number < length ; number += number, tmp = begin, begin = result, result = tmp )
		merge_sort_step_r(rel_ptr, number, begin, begin + length, result);

	return begin;
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
merge_sort_step(
	ptrdiff_t number,
	T* begin,
	T* end,
	T* result)
{
	ptrdiff_t m = 2 * number;

	while (begin + m < end) {
		merge(begin, begin + number, begin + number, begin + m, result);
		begin += m;
		result += m;
	}

	if ( begin + number + 1 < end )
		merge(begin, begin + number, begin + number, end, result);
	else
		while ( begin < end )
			*result++ = *begin++;
}

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
	T* result)
{
	ptrdiff_t m = 2 * number;

	while ( begin + m < end ) {
		merge_r(rel_ptr, begin, begin + number, begin + number, begin + m, result);
		begin += m;
		result += m;
	}

	if ( begin + number < end )
		merge_r(rel_ptr, begin, begin + number, begin + number, end, result);
	else
		while ( begin < end )
			*result++ = *begin++;
}

template <class T>
const T* 
minimum_r(
	int (*rel_ptr)(const T*, const T*),
	const T* begin,
	const T* end)
{
	register const T *index = begin;

	while ( ++begin < end )
		if ( (*rel_ptr)(begin,index) < 0 )
			index = begin;

	return index;
}

template <class T>
const T* 
mismatch(
	const T* begin1,
	const T* end1,
	const T* begin2,
	const T* end2)
{
	register ptrdiff_t len = end1 - begin1 < end2 - begin2 ? end1 - begin1 : end2 - begin2;

	while ( 0 < len-- ) {
		if ( !(*begin1 == *begin2++) )
			return begin1;

		begin1++;
	}

	return 0;
}

template <class T>
const T* 
mismatch_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2)
{
	register ptrdiff_t len = end1 - begin1 < end2 - begin2 ? end1 - begin1 : end2 - begin2;

	while ( 0 < len-- ) {
		if ( (*rel_ptr)(begin1, begin2++) != 0 )
			return begin1;

		begin1++;
	}

	return 0;
}


template <class T>
T* 
part(
	register const T &value,
	register T* begin,
	register T* end)
{
	for ( ; ; ) {
		for ( ; ; begin++ )
			if ( begin >= end )
				return end;
			else if ( !(*begin == value) )
				break;

		for ( ; ; )
			if ( begin >= --end )
				return end;
			else if ( *end == value )
				break;

		{
			register T temp = *begin;
			*begin++ = *end;
			*end = temp;
		}
	}
}

template <class T>
T* 
part_p(
	register int (*pred_ptr)(const T*),
	register T* begin,
	register T* end)
{
	for ( ; ; ) {
		for ( ; ; begin++ )
			if ( begin >= end )
				return end;
			else if ( !(*pred_ptr)(begin) )
				break;

		for ( ; ; )
			if ( begin >= --end )
				return end;
			else if ( (*pred_ptr)(end) )
				break;

		{
			register T temp = *begin;
			*begin++ = *end;
			*end = temp;
		}
	}
}

template <class T>
T* 
part_r(
	register int (*rel_ptr)(const T*, const T*),
	const T &value,
	register T* begin,
	register T* end)
{
	register const T *temp = &value;

	for ( ; ; ) {
		for ( ; ; begin++ )
			if ( begin >= end )
				return end;
			else if ( (*rel_ptr)(begin, temp) != 0 )
				break;

		for ( ; ; )
			if ( begin >= --end )
				return end;
			else if ( (*rel_ptr)(end, temp) == 0 )
				break;

		{
			register T temp = *begin;
			*begin++ = *end;
			*end = temp;
		}
	}
}

template <class T>
T* 
part_c(
	register const T& value,
	const T* begin,
	register const T* end,
	register T* result)
{
	register T *last = result + (end - begin);

	for ( ; begin < end ; begin++ )
		if ( *begin == value )
			*(result++) = *begin;
		else
			*(--last) = *begin;

	return last;
}

template <class T>
T* 
part_pc(
	register int (*pred_ptr)(const T*),
	register const T* begin,
        register const T* end,
	register T* result)
{
	register T *last = result + (end - begin);

	for ( ; begin < end ; begin++ )
		if ( (*pred_ptr)(begin) )
			*(result++) = *begin;
		else
			*--last = *begin;

	return last;
}

template <class T>
T* 
part_rc(
	register int (*rel_ptr)(const T*, const T*),
	const T& value,
        register const T* begin,
	register const T* end,
	register T* result)
{
	register T *last = result + (end - begin);
	register const T *temp = &value;

	for ( ; begin < end ; begin++ )
		if ( (*rel_ptr)(begin, temp) == 0 )
			*(result++) = *begin;
		else
			*(--last) = *begin;

	return last;
}

template <class T>
const T* 
pos(
	register const T& value,
	register const T* begin,
	register const T* end)
{
	while ( begin < end )
		if ( *begin++ == value )
			return begin - 1;

	return 0;
}

template <class T>
const T* 
pos_p(
	register int (*pred_ptr)(const T*),
	register const T* begin,
	register const T* end)
{
	while ( begin < end )
		if ( (*pred_ptr)(begin++) )
			return begin - 1;

	return 0;
}

template <class T>
const T* 
pos_r(
	register int (*rel_ptr)(const T*, const T*),
	const T &value,
	register const T* begin,
	register const T* end)
{
	register const T *temp = &value;

	while ( begin < end )
		if ( (*rel_ptr)(begin++, temp) == 0 )
			return begin - 1;

	return 0;
}

template <class T>
const T* 
random(
	const T* begin,
	const T* end)
{
	if ( begin < end )
		return begin + (ptrdiff_t)(drand48() * (end - begin));
	else
		return 0;
}

template <class T>
T* 
rem(
	register const T& value,
	register T* begin,
	register T* end)
{
	for ( ; ; ) {
		for ( ; ; begin++ )
			if ( begin >= end )
				return end;
			else if ( *begin == value )
				break;

		for ( ; ; )
			if ( begin >= --end )
				return end;
			else if ( !(*end == value) )
				break;

		*begin++ = *end;
	}
}

template <class T>
T* 
rem_p(
	register int (*pred_ptr)(const T*),
	register T* begin,
	register T* end)
{
	for ( ; ; ) {
		for ( ; ; begin++ )
			if ( begin >= end )
				return end;
			else if ( (*pred_ptr)(begin) )
				break;

		for ( ; ; )
			if ( begin >= --end )
				return end;
			else if ( !(*pred_ptr)(end) )
				break;

		*begin++ = *end;
	}
}

template <class T>
T* 
rem_r(
	register int (*rel_ptr)(const T*, const T*),
	const T& value,
	register T* begin,
	register T* end)
{
	register const T *temp = &value;

	for ( ; ; ) {
		for ( ; ; begin++ )
			if ( begin >= end )
				return end;
			else if ( (*rel_ptr)(begin, temp) == 0 )
				break;

		for ( ; ; )
			if ( begin >= --end )
				return end;
			else if ( (*rel_ptr)(end, temp) != 0 )
				break;

		*begin++ = *end;
	}
}

template <class T>
T* 
rem_c(
	register const T &value,
	register const T* begin,
	register const T* end,
	register T* result)
{
	for ( ; begin < end ; begin++ )
		if ( !(*begin == value) )
			*result++ = *begin;

	return result;
}

template <class T>
T* 
rem_pc(
	register int (*pred_ptr)(const T*),
	register const T* begin,
	register const T* end,
	register T* result)
{
	for ( ; begin < end ; begin++ )
		if ( !(*pred_ptr)(begin) )
			*result++ = *begin;

	return result;
}

template <class T>
T* 
rem_rc(
	register int (*rel_ptr)(const T*, const T*),
	const T& value,
	register const T* begin,
	register const T* end,
	register T* result)
{
	register const T *temp = &value;

	for ( ; begin < end ; begin++ )
		if ( (*rel_ptr)(begin, temp) != 0 )
			*result++ = *begin;

	return result;
}

template <class T>
T* 
rem_dup(
	register T* begin,
	register T* end)
{
	register T *index = begin;
	register T *m;

	for( ; index < end ; index++ )
		if ( pos(*index, begin, index) != 0 )
			break;

	m = index;

	while ( ++index < end )
		if ( pos(*index, begin, m) == 0 )
			*m++ = *index;

	return m;
}

template <class T>
T* 
rem_dup_r(
	int (*rel_ptr)(const T*, const T*),
	register T* begin,
	register T* end)
{
	register T *index = begin;
	register T *m;

	for( ; index < end ; index++ )
		if ( pos_r(rel_ptr, *index, begin, index) != 0 )
			break;

	m = index;

	while ( ++index < end )
		if ( pos_r(rel_ptr, *index, begin, m) == 0 )
			*m++ = *index;

	return m;
}

template <class T>
T* 
rem_dup_c(
	register const T* begin,
	register const T* end,
	register T* result)
{
	register T *m = result;

	for ( ; begin < end ; begin++ )
		if ( !pos(*begin, result, m) )
			*m++ = *begin;

	return m;
}

template <class T>
T* 
rem_dup_rc(
	int (*rel_ptr)(const T*, const T*),
	register const T* begin,
	register const T* end,
	register T* result)
{
	register T *m = result;

	for ( ; begin < end ; begin++ )
		if ( pos_r(rel_ptr, *begin, result, m) == 0 )
			*m++ = *begin;

	return m;
}

template <class T>
void 
reverse(
	register T* begin,
	register T* end)
{
	while ( begin < --end ) {
		register T temp = *begin;
		*begin++ = *end;
		*end = temp;
	}
}

template <class T>
void 
reverse_c(
	register const T* begin,
	register const T* end,
	register T* result)
{
	while ( begin < end-- )
		*result++ = *end;
}

template <class T>
const T* 
rt_pos(
	register const T& value,
	register const T* begin,
	register const T* end)
{
	while ( begin < end )
		if ( *--end == value )
			return end;

	return 0;
}

template <class T>
const T* 
rt_pos_p(
	register int (*pred_ptr)(const T*),
	register const T* begin,
	register const T* end)
{
	while ( begin < end )
		if ( (*pred_ptr)(--end) )
			return end;

	return 0;
}

template <class T>
const T* 
rt_pos_r(
	register int (*rel_ptr)(const T*, const T*),
	const T& value,
	register const T* begin,
	register const T* end)
{
	register const T *temp = &value;

	while ( begin < end )
		if ( (*rel_ptr)(--end, temp) == 0 )
			return end;

	return 0;
}

template <class T>
void 
rotate(
	ptrdiff_t number,
	T* begin,
	T* end)
{
	if ( begin >= end )
		return;

	number %= end - begin;

	if ( number == 0 )
		return;

	if ( number < 0 )
		number += (end - begin);

	reverse(begin, end);
	reverse(begin, begin + number);
	reverse(begin + number, end);
}

template <class T>
void 
rotate_c(
	ptrdiff_t number,
	const T* begin,
	const T* end,
	T* result)
{
	if ( begin >= end )
		return;

	number %= end - begin;

	if ( number == 0 ) {
		copy((T*)begin, (T*)end, result);
		return;
	}

	if ( number < 0 )
		number += (end - begin);

	copy((T*)end - number, (T*)end, result);
	copy((T*)begin, (T*)end - number, result + number);
}

template <class T>
const T* 
search(
	register const T* begin1,
	const T* end1,
	register const T* begin2,
	const T* end2)
{
	register ptrdiff_t d1, d2, k;

	if ( begin2 >= end2 )
		return begin1;

	d1 = end1 - begin1;
	d2 = end2 - begin2;

	if ( d1 < d2 )
		return 0;

	k = 0;

	while ( k < d2 ) {
		if ( begin1[k] == begin2[k] )
			k++;
		else if ( d1 == d2 )
			return 0;
		else {
			k = 0;
			begin1++;
			d1--;
		}
	}

	return begin1;
}

template <class T>
const T* 
search_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	const T* end1,
	register const T* begin2,
	const T* end2)
{
	register ptrdiff_t d1, d2, k;

	if ( begin2 >= end2 )
		return begin1;

	d1 = end1 - begin1;
	d2 = end2 - begin2;

	if ( d1 < d2 )
		return 0;

	k = 0;

	while ( k < d2 ) {
		if ( (*rel_ptr)(begin1 + k, begin2 + k) == 0 )
			k++;
		else if ( d1 == d2 )
			return 0;
		else {
			k = 0;
			begin1++;
			d1--;
		}
	}
	return begin1;
}

template <class T>
void 
select(
	ptrdiff_t nth,
	T* begin,
	T* end)
{
	if ( begin >= end || nth <= 0 || end - begin < nth )
		return;

	for ( ; ; ) {
		if ( end - begin < 6 ) {
			ins_sort(begin, end);
			return;
		}
		else if ( nth < 4 ) {
			while (nth--) {
				register T *r = (T*)minimum(begin, end);
				register T temp = *begin;
				*begin = *r;
				*r = temp;
			}
			return;
		}
		else {
			T *index = ordered_part_ATTLC(begin, end);

			if ( index - begin >= nth )
				end = index;
			else {
				nth -= index - begin;
				begin = index;
			}
		}
	}
}

template <class T>
void 
select_r(
	int (*rel_ptr)(const T*, const T*),
	ptrdiff_t nth,
	T* begin,
	T* end)
{
	if ( begin >= end || nth <= 0 || end - begin < nth )
		return;

	for ( ; ; ) {
		if ( end - begin < 6 ) {
			ins_sort_r(rel_ptr, begin, end);
			return;
		}
		else if ( nth < 4 ) {
			while ( nth-- ) {
				register T *r = (T*)minimum_r(rel_ptr, begin, end);
				register T temp = *begin;
				*begin++ = *r;
				*r = temp;
			}
			return;
		}
		else {
			T *index = ordered_part_r_ATTLC(rel_ptr, begin, end);

			if ( index - begin >= nth )
				end = index;
			else {
				nth -= index - begin;
				begin = index;
			}
		}
	}
}

template <class T>
T* 
set_diff(
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 )
		if ( *begin1 < *begin2 )
			*result++ = *begin1++;
		else if ( !(*begin2++ < *begin1) )
			begin1++;

	while ( begin1 < end1 )
		*result++ = *begin1++;

	return result;
}

template <class T>
T* 
set_diff_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 ) {
		register int c = (*rel_ptr)(begin2, begin1);

		if ( 0 < c )
			*result++ = *begin1++;
		else {
			if ( c == 0 )
				begin1++;

			begin2++;
		}
	}

	while ( begin1 < end1 )
		*result++ = *begin1++;

	return result;
}

template <class T>
T* 
set_insert(
	const T& value,
	T* begin,
	T* end)
{
	T *index = (T*)bin_loc(value, begin, end);

	if ( begin < index && !(*(index - 1) < value) )
		return 0;
	else {
		copy(index, end, index + 1);
		*index = value;
		return index;
	}
}

template <class T>
T* 
set_insert_r(
	int (*rel_ptr)(const T*, const T*),
	const T &value,
	T* begin,
	T* end)
{
	T *index = (T*)bin_loc_r(rel_ptr, value, begin, end);

	if ( begin < index && !((*rel_ptr)(index - 1, &value) < 0) )
		return 0;
	else {
		copy(index, end, index + 1);
		*index = value;
		return index;
	}
}

template <class T>
T* 
set_remove(
	const T& value,
	T* begin,
	T* end)
{
	T *index = (T*)bin_loc(value, begin, end);

	if ( begin < index && !(*(index - 1) < value) ) {
		copy(index, end, index - 1);
		return end-1;
	}
	else
		return end;
}

template <class T>
T* 
set_remove_r(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	T* begin,
	T* end)
{
	T *index = (T*)bin_loc_r(rel_ptr, value, begin, end);

	if ( begin < index && !((*rel_ptr)(index - 1, &value) < 0) ) {
		copy(index, end, index - 1);
		return end-1;
	}
	else
		return end;
}

template <class T>
T* 
set_union(
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 ) {
		if ( *begin2 < *begin1 )
			*result++ = *begin2++;
		else {
			if ( *begin1 < *begin2 )
				*result++ = *begin1++;
			else {
				*result++ = *begin2++;
				begin1++;
			}
		}
	}

	while ( begin1 < end1 )
		*result++ = *begin1++;

	while ( begin2 < end2 )
		*result++ = *begin2++;

	return result;
}

template <class T>
T*
set_union_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 ) {
		register int c = (*rel_ptr)(begin2, begin1);

		if ( c < 0 )
			*result++ = *begin2++;
		else {
			*result++ = *begin1++;
			if ( c == 0 )
				begin2++;
		}
	}

	while ( begin1 < end1 )
		*result++ = *begin1++;

	while ( begin2 < end2 )
		*result++ = *begin2++;

	return result;
}

template <class T>
void 
shuffle(
	register T* begin,
	register T* end)
{
	register T *index = begin + 1;

	while ( index < end ) {
		register T *r = (T*)random(begin, index + 1);
		register T temp = *index;
		*index++ = *r;
		*r = temp;
	}
}

template <class T>
void 
shuffle_c(
	register const T* begin,
	register const T* end,
	register T* result)
{
	register T *result_end = result;

	while ( begin < end ) {
		register T *r = (T*)random(result, result_end + 1);
		*result_end++ = *r;
		*r = *begin++;
	}
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
T*
ordered_part_ATTLC(
	register T *begin,
	register T *end)
{
	T *old_begin = begin;
	register T value = *random(begin, end);

	begin--;

	for ( ; ; ) {
		while ( *++begin < value );

		while ( value < *--end );
		
		if ( begin < end ) {
			register T temp = *begin;
			*begin = *end;
			*end = temp;
		}
		else
			return (begin == old_begin) ? begin + 1 : begin;
	}
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
quicksort_loop_ATTLC(
	register T *begin,
	register T *end)
{
	while ( 10 < end - begin ) {
		register T *index = ordered_part_ATTLC(begin, end);

		if ( end - index < index - begin ) {
			quicksort_loop_ATTLC(index, end);
			end = index;
		}
		else {
			quicksort_loop_ATTLC(begin, index);
			begin = index;
		}
	}
}

template <class T>
void 
sort(
	T* begin,
	T* end)
{
	quicksort_loop_ATTLC(begin, end);
	ins_sort(begin, end);
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
T* 
ordered_part_r_ATTLC(
	register int (*rel_ptr)(const T*, const T*),
        register T* begin, register T* end)
{
	T* old_begin = begin;
	T value = *random(begin, end);
	register T *temp = &value;

	begin--;

	for ( ; ; ) {
		while ( (*rel_ptr)(++begin, temp) < 0 );

		while ( (*rel_ptr)(temp, --end) < 0 );

		if ( begin < end ) {
			register T temp = *begin;
			*begin = *end;
			*end = temp;
		}
		else
			return (begin==old_begin) ? begin + 1 : begin;
	}
}

template <class T>
#if !defined(__SUNPRO_CC)
static
#endif
void 
quicksort_loop_r_ATTLC(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end)
{
	while ( 10 < end - begin ) {
		T *index = ordered_part_r_ATTLC(rel_ptr, begin, end);

		if ( end - index < index - begin ) {
			quicksort_loop_r_ATTLC(rel_ptr, index, end);
			end = index;
		}
		else {
			quicksort_loop_r_ATTLC(rel_ptr, begin, index);
			begin = index;
		}
	}
}

template <class T>
void 
sort_r(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end)
{
	quicksort_loop_r_ATTLC(rel_ptr, begin, end);
	ins_sort_r(rel_ptr, begin, end);
}

template <class T>
T* 
part_s(
	const T& value,
	T* begin, T* end)
{
	if ( end - begin > 1 ) {
		T *middle = begin + ((end - begin) >> 1);
		T *first_half = part_s(value, begin, middle);
		T *second_half = part_s(value, middle, end);

		rotate(first_half - middle, first_half, second_half);
		return first_half + (second_half - middle);

	}
	else if ( begin < end )
		return *begin == value ? begin + 1 : begin;
	else
		return end;
}

template <class T>
T* 
part_ps(
	int (*pred_ptr)(const T*),
	T* begin,
	T* end)
{
	if ( end - begin > 1 ) {
		T *middle = begin + ((end - begin) >> 1);
		T *first_half = part_ps(pred_ptr, begin, middle);
		T *second_half = part_ps(pred_ptr, middle, end);

		rotate(first_half - middle, first_half, second_half);
		return first_half + (second_half - middle);

	}
	else if ( begin < end )
		return (*pred_ptr)(begin) ? begin + 1 : begin;
	else
		return end;
}

template <class T>
T* 
part_rs(
	int (*rel_ptr)(const T*, const T*),
	const T &value,
	T* begin,
	T* end)
{
	if ( end - begin > 1 ) {
		T *middle = begin + ((end - begin) >> 1);
		T *first_half = part_rs(rel_ptr, value, begin, middle);
		T *second_half = part_rs(rel_ptr, value, middle, end);

		rotate(first_half - middle, first_half, second_half);
		return first_half + (second_half - middle);

	}
	else if ( begin < end )
		return ((*rel_ptr)(begin, &value) == 0) ? begin + 1 : begin;
	else
		return end;
}

template <class T>
T* 
part_sc(
	const T& value,
	const T* begin,
	const T* end,
	T* result)
{
	T *m = part_c(value, begin, end, result);
	reverse(m, result + (end - begin));
	return m;
}

template <class T>
T* 
part_psc(
	int (*pred_ptr)(const T*), 
	const T* begin, 
	const T* end,
	T* result)
{
	T *m = part_pc(pred_ptr, begin, end, result);
	reverse(m, result + (end - begin));
	return m;
}

template <class T>
T* 
part_rsc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end,
	T* result)
{
	T *m = part_rc(rel_ptr, value, begin, end, result);
	reverse(m, result + (end - begin));
	return m;
}

template <class T>
T* 
rem_s(
	register const T& value,
	register T* begin,
	register T* end)
{
	register T *m;

	while ( begin < end && !(*begin == value) )
		begin++;

	if ( begin >= end )
		return end;

	m = begin;

	while ( ++begin < end )
		if ( !(*begin == value) )
			*m++ = *begin;

	return m;
}

template <class T>
T* 
rem_ps(
	register int (*pred_ptr)(const T*),
	register T* begin,
	register T* end)
{
	register T *m;

	while ( begin < end && !(*pred_ptr)(begin) )
		begin++;

	if ( begin >= end )
		return end;

	m = begin;

	while ( ++begin < end )
		if ( !(*pred_ptr)(begin) )
			*m++ = *begin;

	return m;
}

template <class T>
T* 
rem_rs(
	register int (*rel_ptr)(const T*, const T*),
	const T &value,
	register T* begin,
	register T* end)
{
	register const T *temp = &value;
	register T *m;

	while ( begin < end && (*rel_ptr)(begin, temp) != 0 )
		begin++;

	if ( begin >= end )
		return end;

	m = begin;

	while ( ++begin < end )
		if ( (*rel_ptr)(begin, temp) != 0 )
			*m++ = *begin;

	return m;
}

template <class T>
T* 
rem_sc(
	const T& value,
	const T* begin,
	const T* end,
	T* result)
{
	return rem_c(value, begin, end, result);
}

template <class T>
T* 
rem_psc(
	int (*pred_ptr)(const T*),
	const T* begin,
	const T* end,
	T* result)
{
	return rem_pc(pred_ptr, begin, end, result);
}

template <class T>
T* 
rem_rsc(
	int (*rel_ptr)(const T*, const T*),
	const T& value,
	const T* begin,
	const T* end,
	T* result)
{
	return rem_rc(rel_ptr, value, begin, end, result);
}

template <class T>
void 
sort_s(
	register T* begin,
	register T* end)
{
	T *index = new T[end-begin];

	if ( index == 0 ) {
		ins_sort(begin, end);
		return;
	}

	if ( merge_sort(begin, end, index) == index ) {
		T *i = index;

		while ( begin < end )
			*begin++ = *i++;
	}

	delete [/*end-begin*/]index;
}

template <class T>
void 
sort_rs(
	int (*rel_ptr)(const T*, const T*),
	T* begin,
	T* end)
{
	T* index = new T[end-begin];

	if ( index == 0 ) {
		ins_sort_r(rel_ptr, begin, end);
		return;
	}

	if ( merge_sort_r(rel_ptr, begin, end, index) == index ) {
		T *i = index;

		while ( begin < end )
			*begin++ = *i++;
	}

	delete [/*end-begin*/]index;
}

template <class T>
void 
subs(
	register const T &value,
	register T new_value,
	register T* begin,
	register T* end)
{
	for ( ; begin < end; begin++ )
		if ( *begin == value )
			*begin = new_value;

}

template <class T>
void 
subs_r(
	register int (*rel_ptr)(const T*, const T*),
	const T &value,
	register T new_value,
	register T* begin,
	register T* end)
{
	register const T *temp = &value;

	for ( ; begin < end; begin++ )
		if ( (*rel_ptr)(begin, temp) == 0 )
			*begin = new_value;

}

template <class T>
void 
subs_c(
	register const T& value,
	register T new_value,
	register const T* begin,
	register const T* end,
	register T* result)
{
	for ( ; begin < end; begin++ )
		*result++ = (*begin == value ? new_value : *begin);
}

template <class T>
void 
subs_rc(
	register int (*rel_ptr)(const T*, const T*),
	const T &value,
	register T new_value,
	register const T* begin,
	register const T* end,
	register T* result)
{
	register const T *temp = &value;

	for ( ; begin < end; begin++ )
		*result++ = ((*rel_ptr)(begin, temp) == 0 ? new_value : *begin);
}

template <class T>
T* 
set_sdiff(
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 )
		if ( *begin1 < *begin2 )
			*result++ = *begin1++;
		else if ( *begin2 < *begin1 )
			*result++ = *begin2++;
		else {
			begin1++;
			begin2++;
		}

	while ( begin1 < end1 )
		*result++ = *begin1++;

	while ( begin2 < end2 )
		*result++ = *begin2++;

	return result;
}

template <class T>
T* 
set_sdiff_r(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin1,
	register const T* end1,
	register const T* begin2,
	register const T* end2,
	register T* result)
{
	while ( begin1 < end1 && begin2 < end2 ) {
		register int c = (*rel_ptr)(begin2, begin1);

		if ( 0 < c )
			*result++ = *begin1++;
		else if ( c < 0 )
			*result++ = *begin2++;
		else
			begin1++, begin2++;
	}

	while ( begin1 < end1 )
		*result++ = *begin1++;

	while ( begin2 < end2 )
		*result++ = *begin2++;

	return result;
}

template <class T>
T* 
unique(
	register T* begin,
	register T* end)
{
	register T *m;

	if ( begin >= end )
		return begin;

	while ( ++begin < end )
		if ( *(begin - 1) == *begin )
			break;
	
	if ( begin == end )
		return begin;

	m = begin - 1;

	while ( ++begin < end )
		if ( !(*m == *begin) )
			*++m = *begin;

	return m + 1;
}

template <class T>
T* 
unique_r(
	register int (*rel_ptr)(const T*, const T*),
	register T* begin,
	register T* end)
{
	register T *m;

	if ( begin >= end )
		return begin;

	while ( ++begin < end )
		if ( !(*rel_ptr)(begin - 1, begin) )
			break;

	if ( begin == end )
		return begin;

	m = begin - 1;

	while ( ++begin < end )
		if ( (*rel_ptr)(m, begin) )
			*++m = *begin;

	return m + 1;
}

template <class T>
T* 
unique_c(
	register const T* begin,
	register const T* end,
	register T* result)
{
	if ( begin >= end )
		return result;

	*result = *begin;

	while ( ++begin < end )
		if ( !(*result == *begin) )
			*++result = *begin;

	return result + 1;
}

template <class T>
T* 
unique_rc(
	register int (*rel_ptr)(const T*, const T*),
	register const T* begin,
	register const T* end,
	register T* result)
{
	if ( begin >= end )
		return result;

	*result = *begin;

	while ( ++begin < end )
		if ( (*rel_ptr)(result, begin) )
			*++result = *begin;

	return result + 1;
}

#endif
