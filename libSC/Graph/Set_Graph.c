/*ident	"@(#)Graph:Set_Graph.c	3.1" */
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

#ifdef __GNUG__
#pragma implementation "set_of_p.h"
#include <set_of_p.h>
#include <Graph.h>
#include <iostream.h>
static ostream& operator<<(ostream& os, const Graph &g) { return os; }
static ostream& operator<<(ostream& os, const Vertex &v) { return os; }
static ostream& operator<<(ostream& os, const Edge &e) { return os; }
template class Set_of_p<Graph>;
#endif
