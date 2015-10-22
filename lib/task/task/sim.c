/*ident	"@(#)cls4:lib/task/task/sim.c	1.3" */
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
#include <task.h>
#include "hw_stack.h"		/* Needed for MAXINT_AS_FLOAT */

histogram::histogram(int nb, int ll, int rr)
{
	register int i;
	if (rr<=ll || nb<1) object::task_error(E_HISTO, (object*)0);
	if (nb%2) nb++;
	while ((rr-ll)%nb) rr++;
	binsize = (rr-ll)/nb;
	h = new int[nb];
	while (h == 0) object::task_error(E_STORE, (object*)0);
	for (i=0; i<nb; i++) h[i] = 0;
	l = ll;
	r = rr;
	nbin = nb;
	sum = 0;
	sqsum = 0;
}

void
histogram::add(int a)
/* add a to one of the bins, adjusting histogram, if necessary */
{
	register int i, j;

	/* make l <= a < r, */
        /*      possibly expanding histogram by doubling binsize and range */
	while (a<l) {
		l -= r - l;
		for (i=nbin-1, j=nbin-2; 0<=j; i--, j-=2) h[i] = h[j] + h[j+1];
		while(i >= 0) h[i--] = 0;
		binsize += binsize;
	}
	while (r<=a) {
		r += r - l;
		for (i=0, j=0; i<nbin/2 ; i++, j+=2) h[i] = h[j] + h[j+1];
		while (i < nbin) h[i++] = 0;
		binsize += binsize;
	}
	sum += a;
	sqsum += a * a;
	h[(a-l)/binsize]++;
}

void
histogram::print()
/*
	printout non-empty ranges
*/
{
	register int i;
	register int x;
	int d = binsize;
	
	for (i=0; i<nbin; i++) {
		if (x=h[i]) {
			int ll = l+d*i;
			printf("[%d:%d] : %d\n",ll,ll+d,x);
		}
	}
}

float
randint::fdraw()
{
	return ABS(DRAW)/(MAXINT_AS_FLOAT + 1);
}

int
urand::draw()
{
	return int(low + (high+1-low) * (0+randint::draw()/(MAXINT_AS_FLOAT+1)));
}
