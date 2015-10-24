/*ident	"@(#)Stopwatch:incl/Stopwatch.h	3.1" */
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

#ifndef STOPWATCHH
#define STOPWATCHH

#include <sys/param.h> 
#include <unistd.h>

//  On pure BSD systems, the value HZ is missing from sys/param.h.
//  The following makes Stopwatch more portable at the expense of
//  not giving correct results on systems for which HZ is other
//  than 60.

#ifndef HZ
#define HZ 60
#endif

class Stopwatch{
public:

    Stopwatch();
//    Purpose:
//        Create a new Stopwatch.
//        The Stopwatch is initially stopped (status() is 0) 
//        and real(), user(), and system() all read zero.

    inline int status()const;
//    Purpose:
//        Return 0 iff stopwatch is not moving.

    void reset();
//    Purpose:
//        Reset the Stopwatch to zero.  It is not necessary 
//        for the Stopwatch to be stopped in order to reset it.  
//        If the Stopwatch is running, it will resume running 
//        (from zero) immediately after resetting.

    void start();
//    Purpose:
//        Start the Stopwatch moving from its present 
//        reading.  If status() is initially 0, it
//        becomes nonzero.  If status() is initially
//        non-zero, the call has no effect.

    void stop();
//    Purpose:
//        Stop the Stopwatch.  If status() is initially
//        non-zero, it will become zero.  If status() is
//        initially zero, the call will have no effect.

    double system()const;
//    Purpose:
//        Return the elapsed system time in seconds.

    double user()const;
//    Purpose:
//        Return the elapsed user time in seconds.

    double real()const;
//    Purpose:
//        Return the elapsed real time in seconds.
//    Note:                                                           ?????
//        Uses times(2).  According to SVID, times(2) should
//        return the elapsed real time since some arbitrary
//        point prior to process initiation.  However Sun
//        Unix version of times(3) does not claim to do this.

    inline static double resolution();
//    Purpose:
//        Return the resolution of the Stopwatch in seconds.

private:
    long       old_r_ticks;    
    long       old_u_ticks;    
    long       old_s_ticks;    
    long       r_ticks; 
    long       u_ticks; 
    long       s_ticks; 
    int        move;
}; 

// Inline Function definitions

inline int Stopwatch::status()const{
    return move;
}
inline double Stopwatch::resolution(){
    return 1.0/double(HZ);
}
#endif
