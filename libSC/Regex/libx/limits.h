/*ident	"@(#)Regex:libx/limits.h	3.1" */

/*
 * C language implementation integral type sizes
 */

#define CHAR_BIT	8
#if __STDC__
#define UCHAR_MAX	255U
#else
#define UCHAR_MAX	255
#endif
#define SCHAR_MIN	-128
#define SCHAR_MAX	127
#define CHAR_MIN	SCHAR_MIN
#define CHAR_MAX	SCHAR_MAX
#if __STDC__
#define USHRT_MAX	65535U
#else
#define USHRT_MAX	65535
#endif
#define SHRT_MIN	-32768
#define SHRT_MAX	32767
#if __STDC__
#define UINT_MAX	4294967295U
#else
#define UINT_MAX	4294967295
#endif
#define INT_MIN		-2147483648
#define INT_MAX		2147483647
#define ULONG_MAX	UINT_MAX
#define LONG_MIN	INT_MIN
#define LONG_MAX	INT_MAX

/*
 * system implementation limits
 */

#define ARG_MAX		1048576
#define CLK_TCK		60
#define MAX_CANON	256
#define LINK_MAX	32767
#define NGROUPS_MAX	16
#define PATH_MAX	1024
#define OPEN_MAX	64
#define OPEN_MAX_CEIL	OPEN_MAX
