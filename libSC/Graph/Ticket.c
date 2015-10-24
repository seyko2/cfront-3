/*ident	"@(#)Graph:Ticket.c	3.1" */
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

#include <Ticket.h>
#include <Bits.h>

const int Ticket_ATTLC::bits_increment = sizeof(unsigned long); 
Bits Vis_v_ticket::b = Bits(0, sizeof(unsigned long));
Bits Vis_e_ticket::b = Bits(0, sizeof(unsigned long));
Bits Val_v_ticket::b = Bits(0, sizeof(unsigned long));
Bits Val_e_ticket::b = Bits(0, sizeof(unsigned long));

void
Ticket_ATTLC::Validate(Bits& b)
{
	unsigned int count = 0;

	while (b[count])
		count++;

	while (count >= b.size())
		b.size(b.size() + bits_increment);

	num = count;
	b.set(num);
}

void
Ticket_ATTLC::Invalidate(Bits& b)
{
	b.reset(num);
}

void
Vis_v_ticket::validate()
{
	Validate(b);
}

void
Vis_v_ticket::invalidate()
{
	Invalidate(b);
}

void
Vis_e_ticket::validate()
{
	Validate(b);
}

void
Vis_e_ticket::invalidate()
{
	Invalidate(b);
}

void
Val_v_ticket::validate()
{
	Validate(b);
}

void
Val_v_ticket::invalidate()
{
	Invalidate(b);
}

void
Val_e_ticket::validate()
{
	Validate(b);
}

void
Val_e_ticket::invalidate()
{
	Invalidate(b);
}
