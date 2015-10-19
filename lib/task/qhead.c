/* @(#) qhead.c 1.2 1/27/86 17:48:07 */
/*ident	"@(#)cfront:lib/task/qhead.c	1.2"*/
#include "task.h"

/*	a qhead's qh_queue has its pointer q_ptr pointing the last
	element of a circular list, so that q_ptr->o_next is the
	first element of that list.

   STRUCTURE:
	qhead <--> oqueue <--> qtail	(qhead and qtail are independent)
	oqueue --> circular queue of objects

	"pen and paper is recommended when trying to understand
	the list manipulations."
*/

qhead.qhead(int mode, int max) : (QHEAD)
/*
	construct qhead <--> (possible)oqueue --> 0
*/
{
DB(("x%x->qhead( %d, %d )\n", this, mode, max));
	if (0 < max) {
		qh_queue = new oqueue(max);
		qh_queue->q_head = this;
	};
	qh_mode = mode;
}


qhead.~qhead()
/*
	destroy q if not pointed to by a qtail
*/
{
DB(("x%x->~qhead()\n", this));
	oqueue* q = qh_queue;

	if (q->q_tail)
		q->q_head = 0;
	else 
		delete q;
	
}


object*
qhead.get()
/*
	remove and return object from head of q
	alert tasks blocked on full queue
	sleep if queue is empty
*/
{
DB(("x%x->qhead::get()\n", this));
	register oqueue* q = qh_queue;
ll:
	if (q->q_count) {
		register object* oo = q->q_ptr;
		register object* p = oo->o_next;
		oo->o_next = p->o_next;
		p->o_next = 0;
		if (q->q_count-- == q->q_max) {
			qtail* t = q->q_tail;
			if (t) t->alert();
		};
		return p;
	}

	switch (qh_mode) {
	case WMODE:
		remember(thistask);
		thistask->sleep();
		forget(thistask);
		goto ll;
	case EMODE:
		task_error(E_GETEMPTY,this);
		goto ll;
	case ZMODE:
		return 0;
	}
}

qtail*
qhead.tail()
/*
	create a tail for this queue
*/
{
DB(("x%x->qhead::tail()\n", this));
	oqueue* q = qh_queue;
	register qtail* t = q->q_tail;

	if (t == 0) {
		t = new qtail(qh_mode,0);
		q->q_tail = t;
		t->qt_queue = q;
	}

	return t;
}


qhead*
qhead.cut()
/*
	make room for a filter upstream from this qhead
	result:  (this)qhead<-->newq    (new)qhead<-->oldq ?<-->qtail?
*/
{
DB(("x%x->qhead::cut()\n", this));
	oqueue* oldq = qh_queue;
	qhead* h = new qhead(qh_mode,oldq->q_max);
	oqueue* newq = h->qh_queue;

	oldq->q_head = h;
	h->qh_queue = oldq;

	qh_queue = newq;
	newq->q_head = this;

	return h;
}


void
qhead.splice(qtail* t)
/*
	this qhead is supposed to be upstream to the qtail t
	add the contents of this's queue to t's queue
	destroy this, t, and this's queue
	alert the spliced qhead and qtail if a significant state change happened
*/
{
DB(("x%x->qhead::splice( x%x )\n", this,t));
	oqueue* qt = t->qt_queue;
	oqueue* qh = qh_queue;

	int qtcount = qt->q_count;
	int qhcount = qh->q_count;
	int halert = (qtcount==0 && qhcount);	/* becomes non-empty */
	int talert = (qh->q_max <= qhcount && qhcount+qtcount<qt->q_max);
						/* becomes non-full */	
	if (qhcount) {
		object* ooh = qh->q_ptr;
		object* oot = qt->q_ptr;
		qt->q_ptr = ooh;
		if (qtcount) {		/* add the contents of qh to qt */
			object* tf = oot->o_next;
			oot->o_next = ooh->o_next;
			ooh->o_next = tf;
		}
		qt->q_count = qhcount + qtcount;
		qh->q_count = 0;
	}

	(qh->q_tail)->qt_queue = qt;
	qt->q_tail = qh->q_tail;
	qh->q_tail = 0;

	delete t;
	delete this;

	if (halert) qt->q_head->alert();
	if (talert) qt->q_tail->alert();
}


int
qhead.putback(object* p)
/*
	insert new object at head of queue (after queue->q_ptr)
*/
{
DB(("x%x->qhead::putback( x%x )\n", this,p));
	oqueue* q = qh_queue;

	if (p->o_next) task_error(E_BACKOBJ,this);
ll:
	if (q->q_count++ < q->q_max) {
		if (q->q_count == 1) {
			q->q_ptr = p;
			p->o_next = p;
		}
		else {
			object* oo = q->q_ptr;
			p->o_next = oo->o_next;
			oo->o_next = p;
		}
		return 1;
	}

	switch (qh_mode) {
	case WMODE:
	case EMODE:
		task_error(E_BACKFULL,this);
		goto ll;
	case ZMODE:
		return 0;
	}
}


void
qhead.print(int n)
{
DB(("x%x->qhead::print( %d )\n", this, n));
	oqueue* q = qh_queue;

	printf("qhead (%d): mode=%d, max=%d, count=%d, tail=%d\n",
		this,qh_mode,q->q_max,q->q_count,q->q_tail);

	if (n&VERBOSE) {
		int m = n & ~(CHAIN|VERBOSE);
		if (q->q_tail) {
			printf("\ttail of queue:\n");
			q->q_tail->print(m);
		}
		q->print(m);
	}
}


void
oqueue.print(int n)
{
DB(("x%x->wqueue::print( %d )\n", this, n));
	object* p = q_ptr;

	if (q_count == 0) return;

	printf("\tobjectects on queue:\n");

	do {
		p->print(n);
		p = p->o_next;
	} while (p != q_ptr);

	printf("\n");
}
