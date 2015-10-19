/* @(#) qtail.c 1.2 1/27/86 17:48:08 */
/*ident	"@(#)cfront:lib/task/qtail.c	1.2"*/
#include "task.h"

qtail.qtail(int mode, int max) : (QTAIL)
/*
	construct qtail <--> oqueue
*/
{
DB(("x%x->qtail( %d, %d )\n", this, mode, max));
	if (0 < max) {
		qt_queue = new class oqueue(max);
		qt_queue->q_tail = this;
	};
	qt_mode = mode;
}

qtail.~qtail()
/*
	destroy q if not also pointed to by a qhead
*/
{
DB(("x%x->~qtail()\n", this));
	oqueue* q = qt_queue;

	if (q->q_head)
		q->q_tail = 0;
	else
		delete q;
}


int
qtail.put(object* p)
/*
	insert object at rear of q (becoming new value of oqueue->q_ptr)
*/
{
DB(("x%x->qtail::put( x%x )\n", this, p));
	register oqueue* q = qt_queue;
ll:
	if (p->o_next) task_error(E_PUTOBJ,this);

	if (q->q_count < q->q_max) {
		if (q->q_count++) {
			register object* oo = q->q_ptr;
			p->o_next = oo->o_next;
			q->q_ptr = oo->o_next = p;
		}
		else {
			qhead* h = q->q_head;
			q->q_ptr = p->o_next = p;
			if (h) h->alert();
		}
		return 1;
	}

	switch (qt_mode) {
	case WMODE:
		remember(thistask);
		thistask->sleep();
		forget(thistask);
		goto ll;
	case EMODE:
		task_error(E_PUTFULL,this);
		goto ll;
	case ZMODE:
		return 0;
	}
}


qhead*
qtail.head()
/*
	create head for this q
*/
{
DB(("x%x->qtail::head()\n", this));
	oqueue* q = qt_queue;
	register qhead* h = q->q_head;

	if (h == 0) {
		h = new qhead(qt_mode,0);
		q->q_head = h;
		h->qh_queue = q;
	};

	return h;
}


qtail*
qtail.cut()
/*
	result:  ?qhead<-->? oldq<-->(new)qtail  newq<-->(this)qtail
*/
{
DB(("x%x->qtail::cut()\n", this));
	oqueue* oldq = qt_queue;
	qtail* t = new qtail(qt_mode,oldq->q_max);
	oqueue* newq = t->qt_queue;

	t->qt_queue = oldq;
	oldq->q_tail = t;

	newq->q_tail = this;
	qt_queue = newq;

	return t;
}


void
qtail.splice(qhead* h)
/*
	this qtail is supposed to be downstream from the qhead h
*/
{
DB(("x%x->qtail::splice( x%x )\n", this,h));
	h->splice(this);
}


void
qtail.print(int n)
{
DB(("x%x->qtail::print( %d )\n", this,n));
	int m = qt_queue->q_max;
	int c = qt_queue->q_count;
	class qhead * h = qt_queue->q_head;

	printf("qtail (%d): mode=%d, max=%d, space=%d, head=%d\n",
		this,qt_mode,m,m-c,h);

	if (n&VERBOSE) {
		int m = n & ~(CHAIN|VERBOSE);
		if (h) {
			printf("head of queue:\n");
			h->print(m);
		}

		qt_queue->print(m);
	}
}
