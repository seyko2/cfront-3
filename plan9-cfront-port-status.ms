.TL
Cfront re-port for the 4th edition of plan9
.AU
Steve Simon
.R "steve at quintile dot net"
.AB
The first and seccond version of plan9 where supplied with the cfront
c++ compilation environment; Later releases where not. With the release
of the Cfront sourcecode by AT&T.¹
.FS
[1] http://www.softwarepreservation.org/projects/c_plus_plus
.FE
The Plan9 cfront package (V3.01) has been updated to the 
most last available (V3.03). This document details the current status
of this project.
.AE
.NH
Introduction
.LP
Though
.I Cfront
cannot be regarded as a replacement for a modern c++ compiler for
plan9, howver its recent release has provided an opertunity for the
author to learn somthing of the evolution of c++ and to attempt to
provide a useful platform to compile a few historic c++ packages \-
most notable the troff pm macro package.²
.FS
[2] http://lists.cse.psu.edu/archives/9fans/2002-February/015773.html
.FE
.NH
Status
.LP
cfront is self hosting and will compile the pm troff macro post-processor.
.LP
.NH
Differences
.LP
The major differences between the AT&T release and the plan9 implementation
are in the support of multiple hetrogenious architectures,
and the implementation of the compiler driver as a C program rather
than a Bourne shell script.
.NH
Cfront
.LP
The changes made to 2nd edition release of cfront when compared
to he AT&T releae is outlined in a \FINOTES\fR file, this
is reproduced as Appendix A.
.NH 2
64bit, long long
.LP
A first attempt at adding support for the
.I "long long"
datatype to cfront was made, sadly this work is not complete,
and is currently commented out, it can be re-enabled byremoving the
``FIXME long long'' comments.
.NH 1
The Task library
.LP
The task library remains un-ported and both the 3.01 and 3.03
versions of the source code are included in the release package.
The structure of the library was changed from the AT&T for plan9
which makes the port a little more complex but with some care
this should be easy to finish off.
.NH
Templates
.LP
All the tools nescessary to support cfront template support have been
compiled, however these have not been tested; Templates where not supported
in the older plan9 c++ releases.
.NH
Standard Component library
.LP
It was decided not to attempt to port the Standard Component library as it
is now very rarely used. If there is any interested in this package then
the Citibank changes³
.FS
[3] http://www.opengroup.org/dce/mall/objtran.htm
.FE
should be seriously considered.
.NH
Standard Template Library
.LP
A version of the standard template library which is claimed to work
with cfront (though it is not clear which AT&T release was involved)
is availale for RiscOS⁴
.FS
[4] http://homepage.ntlworld.com/thouky/software/cathlibcpp/index.html
.FE
This would doubtless require some modification but could probably be persuaded
to work on plan9
.nh
mkfile changes
.LP
Plan9 seems to have used the .c extension for C++ sourcecode, this makes generating
rules for mkfiles difficult so it was decided to change the extension on all the C++ source
C (capital c) rules have been added to /sys/src/cmd/mkone /sys/src/cmd/mklib /sys/src/cmd/mkmany
to suit. At present C++ object files are still named after the source file and the target object
file. Unfortunately c++ compilation requires a postprocessing of executables (to link
the chain of static initialisers together) so a seperate linker must be specified. This
can be achieved simply by adding the following to c++ centric mkfiles:
.DS
.CW
LD=c++/$LD
.DE
.bp
.SH
Appendix A
.LP
.nj
.nf
.CW
0.
	most changes are commented with
	plan 9 somewhere in the comment

1. modified cfront.h to comment out the size in

	#define MAXCONT	100
	extern dcl_context ccvec[/*MAXCONT*/];
1'.
	moved about decl to after definition of struct dcl_context

2. commented out #include <memory.h> in template.c;  it's in string.h.
that appears to have fixed this problem:

	2'. the template structure class templ_compilation {...}
	appears to have only static members and functions, and thus
	becomes a size zero structure in c, and this is illegal on
	all c compilers i have.  how did this ever work?
	there are several others as well...

3. in tree_copy.c, replaced <memory.h> with <string.h>.



4. in expr3.c, commented out
	// extern int miFlag;
since it's declared static at the top of the file, and ken can't cope.

5. running v.out with input "int x;" yields
 error: no size/alignment values - use +x of provide #ifdef in size.h

	spelling mistake in message

6. turns out that size.h is pretty near useless, so i replaced it
with sean's previous version;  the -Dkens$CC line in the mkfile controls
the ifdef's in his size.h.

	was this necessary?  the c++/vc command seems to use +x to set the sizes.

7. new version appears to be have the type of size_t wired in much too
firmly:
	typedef unsigned long size_t;
	
    "./z.c", line 6: error:  operator delete()'s 2nd argument must be a size_t

changing long to int "fixes" this. 

bjarne says the problem is this code:
simpl.c: size_t_type = Pvoid_type->tsizeof()>uint_type->tsizeof()?
	ulong_type:uint_type;

8.
	malloc => (char *)malloc in some places
	the compiler checks this.
	this happens because plan 9 defines void *malloc(size_t)
	as ansi C says, instead of char *malloc(size_t)

