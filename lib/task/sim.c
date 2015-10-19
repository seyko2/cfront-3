/* @(#) sim.c 1.2 1/27/86 17:48:09 */
/*ident	"@(#)cfront:lib/task/sim.c	1.2"*/
/* @(#) sim.c 1.1 1/17/85 14:49:52 */
#include "task.h"

histogram.histogram(int nb, int ll, int rr)
{
DB(("x%x->histogram( %d, %d, %d )\n", this,nb,ll,rr));
	register int i;
	if (rr<=ll || nb<1) task_error(E_HISTO,0);
	if (nb%2) nb++;
	while ((rr-ll)%nb) rr++;
	binsize = (rr-ll)/nb;
	h = new int[nb];
	while (h == 0) task_error(E_STORE,0);
	for (i=0; i<nb; i++) h[i] = 0;
	l = ll;
	r = rr;
	nbin = nb;
	sum = 0;
	sqsum = 0;
}

void histogram.add(int a)
/* add a to one of the bins, adjusting histogram, if necessary */
{
DB(("x%x->histogram::add( %d )\n", this,a));
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

void histogram.print()
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
			printf("[%d:%d) : %d\n",ll,ll+d,x);
		}
	}
} 

/*
int erand.draw()
{
	int k;
	float a;

	for(k=0;;k++) {
		register float u1, u2;
		a = u1 = fdraw();
		do {
			u2 = fdraw();
			if (u1 < u2) return (int) k+a;
			u1 = fdraw();
		} while (u1<u2);
	}
	
}
*/
