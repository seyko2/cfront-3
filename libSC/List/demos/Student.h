/*ident	"@(#)List:demos/Student.h	3.1" */
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

#include <String.h>
#include <stream.h>

class Student {
    String name;
    String grade;
public:
    Student(const String& n,const String& g) : name(n), grade(g) {}
    Student(const Student& s) : name(s.name), grade(s.grade) {}
    int operator==(const Student& s) { return name == s.name && grade == s.grade; }
    friend inline ostream& operator<<(ostream&,const Student&);
    friend inline int name_compare(const Student&,const Student&);
    friend inline int grade_compare(const Student&,const Student&);
}; 

inline ostream&
operator<<(ostream& os,const Student& s)
{
    os << s.name << ": " << s.grade;
    return os;
}

inline int
name_compare(const Student& s1,const Student& s2)
{
    if(s1.name + s1.grade < s2.name + s2.grade)
        return 1;
    else return 0;
}

inline int
grade_compare(const Student& s1,const Student& s2)
{
    if(s1.grade < s2.grade)
        return 1;
    else return 0;
}
