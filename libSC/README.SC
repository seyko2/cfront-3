C++ Standard Components, Release 3.0 
----------------------------------------------------------------

This file describes the contents of the C++ Standard Components
Release 3.0 source distribution.  It also gives an overview
of the compilation process and the major changes from the previous
release.  For more details, see the Release Notes.

Contents of the distribution
----------------------------
Library components:
   Args - UNIX command line arguments
   Array_alg - Operations on contiguously stored data
   Bits - Variable-length bit strings
   Block - Parameterized variable-size arrays
   Fsm - Simple deterministic finite state machines
   G2++ - Records for interprocess communication
   Graph - Entities and relationships
   Graph_alg - Operations on Graphs
   List - Parameterized variable-length sequences
   Map - Parameterized variable-size associative arrays
   Objection - Rudimentary error-handling 
   Path - UNIX pathnames and search paths
   Pool - Special-purpose memory allocators
   Regex - Regular expressions
   Set - Parameterized unordered collections 
   Stopwatch - Program execution time measurement
   String - Variable-length character strings
   Strstream - iostream specialized to String
   Symbol - Unique identifiers based on character strings
   Time - Absolute time, timezone, and duration

Tools components:
   aoutdem - A tool for using C debuggers on C++ programs
   fs - A tool and library to help find freestore bugs
   hier - A tool that displays the inheritance hierarchy in a program
   incl - A tool that displays the hierarchy include files used in a program
   publik - A tool that displays the public interface of a class

All of the source code for these components can be found in the
directories "libSC" and "SCincl".

The "man" directory contains UNIX-style manual pages for all of
the components.

The "aux" directory contains the programs that run the build
process.  The build process uses ordinary UNIX "sh" and "make".

Changes from the previous release 
---------------------------------
The following list contains the new Release 3.0 features which are different
from Release 2.0. To upgrade applications based on Release 2.0 to the
current release, please read the Porting Guide found in the "Getting Started"
manual.

    1.  Syntactically, the major difference between this release and
        the previous release is the replacement of simulated
        parameterized types with template classes. Most container
        components such as Block, List, Map, and Set have been
 	"templatized".

    2.  The List component has been revised in the following two
        ways: (1). there is no more built-in iterator within List, and
        (2). a pointer is now returned from the next() and its companions
        (i.e., prev(), peek_next(), peek_prev(), head(), and tail()).
	With the change, the behaviors of List iterators is more consistent
        with the Set iterators.

    3.  In order for applications which relied heavily on the old
        List to avoid a potential implementation revision, the
        old List component from Release 2.0 is also included in this
        release.  To use the old List instead of the new one, the
        only source change would be to change

               #include <List.h>
        to
               #include <List_old.h>

        Note, however, that the old List is also templatized.

    4.  In the previous release, a used-defined hash function for
        the Set and Bag classes can be specified as a parameterized
        type argument in Setimplement and Bagimplement statements.
        In the current template version, a hash function will be
        provided as an instantiated hash() member function.  If
        there is no user-provided hash() instance, a default hash
        function which always returns zero will be instantiated.

        The guideline for converting the hash function specification
        is:

          - Assume that Setimplement(T,MyHash) was used to specify
            the user-defined hash function MyHash. Change the hash
            function header for MyHash from

                Set_or_Bag_hashval MyHash(const T&) { ... }
            to
                Set_or_Bag_hashval Set<T>::hash(const T&) { ... }

    5.  The Graph and Graph_alg components are the only container
        classes which are not templatized in the current release.
        Simulated parameterized types using preprocessor macros
        are still used in the implementation.  However, the
        Graphimplement macro is no longer necessary in the new
        release and it is optional to delete it from the source
        code.

