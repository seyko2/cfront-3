/*ident	"@(#)cls4:src/Bits.c	1.5" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

// Bits.c

// The numbering scheme in this library is consistently
// ``little-end''  -- bit 0 is the low-order bit of the word
// stored in the lowest location of a class.

#include "Bits.h"

Blockimplement(Bits_chunk)

Bits::Bits(register Bits_chunk val, register unsigned ct)
{
	if (ct < Bits_len_ATTLC) {
		register Bits_chunk mask = ~Bits_chunk(0) << ct;
		while (val & mask) {
			ct++;
			mask <<= 1;
		}
	}
	if (size(ct))
		*((Bits_chunk*)b) = val;
}

unsigned
Bits::size(unsigned x)
{
	int newsize = bound(x);
	if (b.size() != newsize)
		b.size(newsize);
	n = b.size()? x: 0;

	normalize();

	return n;
}

Bits&
Bits::operator&= (const Bits& x)
{
	if (size() < x.size())
		size(x.size());

	register Bits_chunk* p = b;
	register const Bits_chunk* q = x.b;
	register const Bits_chunk* lim = x.limit();

	while (q < lim)
		*p++ &= *q++;

	lim = limit();
	while (p < lim)
		*p++ = 0;

	return *this;
}

Bits&
Bits::operator|= (const Bits& x)
{
	if (size() < x.size())
		size(x.size());

	register Bits_chunk* p = b;
	register const Bits_chunk* q = x.b;
	register const Bits_chunk* lim = x.limit();

	while (q < lim)
		*p++ |= *q++;

	return *this;
}

Bits&
Bits::operator^= (const Bits& x)
{
	if (size() < x.size())
		size(x.size());

	register Bits_chunk* p = b;
	register const Bits_chunk* q = x.b;
	register const Bits_chunk* lim = x.limit();

	while (q < lim)
		*p++ ^= *q++;

	return *this;
}

Bits&
Bits::compl()
{
	register Bits_chunk* p = b;
	register const Bits_chunk* lim = limit();

	while (p < lim) {
		*p = ~*p;
		p++;
	}

	normalize();

	return *this;
}

unsigned
Bits::count() const
{
	register const Bits_chunk* p = b;
	register const Bits_chunk* lim = limit();
	register unsigned r = 0;

	while (p < lim) {
		register unsigned long x = *p++;
		register int i = Bits_len_ATTLC;

		while (--i >= 0) {
			if (x & 1)
				r++;
			x >>= 1;
		}
	}

	return r;
}

#if 0
Bits::operator Bits_chunk() const
{
	register const Bits_chunk* p = b;
	register const Bits_chunk* lim = limit();

	while (p < lim) {
		if (*p++)
			return p[-1];
	}

	return 0;
}

Bits
operator& (const Bits& x, const Bits& y)
{
	Bits r = x;
	r &= y;
	return r;
}

Bits
operator| (const Bits& x, const Bits& y)
{
	Bits r = x;
	r |= y;
	return r;
}

Bits
operator^ (const Bits& x, const Bits& y)
{
	Bits r = x;
	r ^= y;
	return r;
}
#endif

Bits
operator~ (const Bits& x)
{
	Bits r = x;
	r.compl();
	return r;
}

#if 0
Bits
operator<< (const Bits& x, int n)
{
	Bits r = x;
	r <<= n;
	return r;
}

Bits
operator>> (const Bits& x, int n)
{
	Bits r = x;
	r >>= n;
	return r;
}
#endif

int
Bits::compare(const Bits& x) const
{
	int w = bound(size());
	int xw = bound(x.size());
	int len, r;
	register const Bits_chunk* p;
	register const Bits_chunk* q;
	register const Bits_chunk* lim;

	// two null strings are equal
	if (w == 0 && xw == 0)
		return 0;

	// a null string is the smallest string
	if (w == 0)
		return -1;
	if (xw == 0)
		return 1;

	// if the lengths differ, check the high-order
	// part of the longer string; leave shorter length
	// in len if we get out of this
	if (w != xw) {
		if (w > xw) {
			len = xw;
			p = &b[len];
			q = &b[w];
			r = 1;
		} else {
			len = w;
			p = &x.b[len];
			q = &x.b[xw];
			r = -1;
		}

		do {
			if (*p++)
				return r;
		} while (p < q);
	} else
		len = w;

	// now compare low-order parts, going from high-order
	// end to low-order end (the direction is important!)
	p = (const Bits_chunk*)b + len;
	q = (const Bits_chunk*)x.b + len;
	lim = b;
	while (p > lim) {
		if (*--p != *--q)
			return *p < *q? -1: 1;
	}

	// the bits are equal -- length determines the result
	return int(size()) - int(x.size());
}

// Are two bit strings identical?
int
Bits::equal(const Bits& x) const
{
	// two strings of different sizes are not equal
	if (size() != x.size())
		return 0;

	// two null strings are equal
	if (size() == 0)
		return 1;

	// else go the long route
	return compare(x) == 0;
}


// The following routines can surely be made more efficient.
// I have not done so for two reasons:
//
//	1. The function call and memory allocation overhead
//	   will probably dominate for all but the largest of
//	   bit strings.
//
//	2. This code is tricky.  Further optimization would
//	   erode my confidence in getting it right.

Bits&
Bits::operator<<= (int k)
{
	// Quick test for shift by 0 or negative
	if (k <= 0) {
		if (k < 0)
			operator>>= (-k);
		return *this;
	}

	// Enlarge the structure to contain the result; return on error
	if (size(size() + k) == 0)
		return *this;

	register Bits_chunk* lim = b;

	// If needed, shift left by chunks
	int chunkoffset = k >> Bits_shift_ATTLC;
	if (chunkoffset) {
		register Bits_chunk* dest = limit();
		register Bits_chunk* src = dest - chunkoffset;

		do *--dest = *--src;
		while (src > lim);

		do *--dest = 0;
		while (dest > lim);
	}

	// If needed, shift left by bits
	register int bitoffset = k & Bits_mask_ATTLC;
	if (bitoffset) {
		register Bits_chunk* p = limit();
		register int compoffset = Bits_len_ATTLC - bitoffset;

		while (--p > lim)
			*p = (*p << bitoffset) | (*(p-1) >> compoffset);

		// Shift low-order chunk
		*lim <<= bitoffset;
	}

	normalize();

	return *this;
}

Bits&
Bits::operator>>= (int k)
{
	// Quick test for shift by 0 or negative
	if (k <= 0) {
		if (k < 0)
			operator<<= (-k);
		return *this;
	}

	// Check for shifting all significance out
	int newsize = size() - k;
	if (newsize <= 0) {
		size(0);
		return *this;
	}

	// If needed, shift right by chunks, leaving high-order
	// garbage words that will be sized out later
	int chunkoffset = k >> Bits_shift_ATTLC;
	if (chunkoffset) {
		register Bits_chunk* dest = b;
		register Bits_chunk* src = dest + chunkoffset;
		register const Bits_chunk* lim = limit();

		do *dest++ = *src++;
		while (src < lim);
	}

	// If needed, shift right by bits
	register int bitoffset = k & Bits_mask_ATTLC;
	if (bitoffset) {
		register Bits_chunk* p = b;
		register Bits_chunk* lim = p + chunk(newsize-1);
		register int compoffset = Bits_len_ATTLC - bitoffset;

		while (p < lim) {
			*p = (*p >> bitoffset) | (*(p+1) << compoffset);
			p++;
		}

		// Shift high-order chunk right
		*lim >>= bitoffset;
		if (lim+1 < limit())
			*lim |= *(lim+1) << compoffset;
	}

	// Finally, make the size right, discarding junk bits
	size(newsize);

	return *this;
}

// How many significant bits?
unsigned
Bits::signif() const
{
	if (size() == 0)
		return 0;

	register const Bits_chunk* p = limit();
	register const Bits_chunk* lim = b;

	while (--p >= lim) {
		if (*p) {
			register Bits_chunk x = *p;
			register int k = Bits_len_ATTLC;

			while (--k >= 0) {
				if (x & (Bits_chunk(1) << k))
					return k + 1 + Bits_len_ATTLC * (p - lim);
			}
		}
	}

	return 0;
}

Bits&
Bits::concat(const Bits& x)
{
	operator<<= (x.size());
	operator|= (x);
	return *this;
}

#if 0
Bits
concat(const Bits& x, const Bits& y)
{
	Bits r = x;
	r.concat(y);
	return r;
}
#endif
