/*ident	"@(#) @(#)strmap.c	1.1.1.2" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#include <string>
#include <iostream.h>
#include "compiler_type.h"

//#if !defined(__edg_lucent_40) && !defined(__edg_lucent_41)
#ifdef CFRONT_COMPATIBLE_LUCENT
#include <Map.h>
#else
#include <map.h>
#endif

#ifdef NAMESPACES_LUCENT
using namespace std;
#endif

#ifdef CFRONT_COMPATIBLE_LUCENT
typedef Map<string,string> mapss;
//typedef Map<string,mapss> twodmap;
#else
typedef map<string,string,less<string> > mapss;
//typedef map<string,mapss,less<string> > twodmap;
#endif

main() {
	mapss m1;
	m1["Adams"] = "Abagail";
	m1["Madison"] = "Dolly";
	m1["Reagan"] = "Nancy";
	m1["Carter"] = "Rosalyn";
	m1["Ford"] = "Betty";
	m1["Bush"] = "Barbara";
	m1["Clinton"] = "Hillary";

#ifdef CFRONT_COMPATIBLE_LUCENT
	Mapiter<string,string> m_iter(m1);
	m_iter = m1.element("Madison");
	cout << "key=" << m_iter.curr()->key
	     << ", value=" << m_iter.curr()->value
	     << "\n";

	Mapiter<string,string> m_iter1(m1);
	while (m_iter1.next()) {
		cout << "key=" << m_iter1.curr()->key
		     << ", value=" << m_iter1.curr()->value
		     << "\n";
	}
	
#else
	mapss::iterator m_iter;
	m_iter = m1.find("Madison");
	cout << "key=" << (*m_iter).first
	     << ", value=" << (*m_iter).second
	     << "\n";
	
	m_iter = m1.begin();
	while (m_iter != m1.end()) {
		cout << "key=" << (*m_iter).first
		     << ", value=" << (*m_iter).second
		     << "\n";
		m_iter++;
	}
#endif


#if 0
	twodmap pres_info;
	
	pres_info["Reagan"]["spouse"] = "Nancy";
	pres_info["Reagan"]["VP"] = "Bush";
	pres_info["Reagan"]["term"] = "1981-1989";
	pres_info["Bush"]["spouse"] = "Barbara";
	pres_info["Bush"]["VP"] = "Quayle";
	pres_info["Bush"]["term"] = "1989-1993";

#ifdef CFRONT_COMPATIBLE_LUCENT
	Mapiter<string,string> mx(m1);
	mx = pres_info[(pres_info["Reagan"]["VP"])].element("term");
	cout << "Return value of " <<
	  "pres_info[(pres_info[\"Reagan\"][\"VP\"])].find(\"term\")\n";
	cout << "key=" << mx.curr()->key
	     << ", value=" << mx.curr()->value
	     << "\n";
#else
	mapss::iterator mx;
	mx = pres_info[(pres_info["Reagan"]["VP"])].find("term");
	cout << "Return value of " <<
	  "pres_info[(pres_info[\"Reagan\"][\"VP\"])].find(\"term\")\n";
	cout << "key=" << (*mx).first
	     << ", value=" << (*mx).second
	     << "\n";
#endif
#endif
	return (0);
}
