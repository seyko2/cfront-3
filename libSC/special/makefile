#ident	"@(#)makefile	1.1.2.3"
CC = CC
CCFLAGS = -O

OBJS = \
	set_String1.o set_String2.o set_String3.o \
	List_String1.o List_String2.o List_String3.o \
	List_int1.o List_int2.o List_int3.o \
	List_voidP1.o List_voidP2.o List_voidP3.o \
	List_Path1.o List_Path2.o List_Path3.o \
	Map_SS1.o \
	Map_UlUi1.o \
	Map_Si1.o Map_Sl1.o \
	Map_iS1.o Map_lS1.o \
	Map_ii1.o Map_ll1.o \
	Map_il1.o Map_li1.o \
	Map_SS2.o \
	Map_UlUi2.o \
	Map_Si2.o Map_Sl2.o \
	Map_iS2.o Map_lS2.o \
	Map_ii2.o Map_ll2.o \
	Map_il2.o Map_li2.o \
	Block_int1.o Block_int2.o \
	Block_long1.o Block_long2.o \
	Block_Str1.o Block_Str2.o \
	Block_ulong1.o Block_ulong2.o \
	setofp_int1.o setofp_int2.o \
	setofp_void1.o \
	setofp_Str1.o setofp_Str2.o

libspecial.a:	$(OBJS)
	@if [ ! -d Templates.DB ]; then \
		ar cr libspecial.a $(OBJS); \
	fi
	if test "$(SYS)" = BSD; then ranlib libspecial.a; fi

.c.o:	
	$(CC) $(CCFLAGS) -c $<

set_String1.c:	set.dummy1
	sed -e 's/TYPE/String/g' set.dummy1 >set_String1.c

set_String2.c:	set.dummy2
	sed -e 's/TYPE/String/g' set.dummy2 >set_String2.c

set_String3.c:	set.dummy3
	sed -e 's/TYPE/String/g' set.dummy3 >set_String3.c

List_String1.c:	List.dummy1
	sed -e 's/TYPE/String/g' List.dummy1 >List_String1.c

List_String2.c:	List.dummy2
	sed -e 's/TYPE/String/g' List.dummy2 >List_String2.c

List_String3.c:	List.dummy3
	sed -e 's/TYPE/String/g' List.dummy3 >List_String3.c

List_int1.c:	List.dummy1
	sed -e 's/TYPE/int/g' List.dummy1 | grep -v "^#include <int.h>" >List_int1.c

List_int2.c:	List.dummy2
	sed -e 's/TYPE/int/g' List.dummy2 | grep -v "^#include <int.h>" >List_int2.c

List_int3.c:	List.dummy3
	sed -e 's/TYPE/int/g' List.dummy3 | grep -v "^#include <int.h>" >List_int3.c

List_voidP1.c:	List.dummy1
	sed -e 's/TYPE/voidP/g' List.dummy1 | sed -e 's/^#include <voidP.h>/typedef void *voidP;/' >List_voidP1.c

List_voidP2.c:	List.dummy2
	sed -e 's/TYPE/voidP/g' List.dummy2 | sed -e 's/^#include <voidP.h>/typedef void *voidP;/' >List_voidP2.c

List_voidP3.c:	List.dummy3
	sed -e 's/TYPE/voidP/g' List.dummy3 | sed -e 's/^#include <voidP.h>/typedef void *voidP;/' >List_voidP3.c

List_Path1.c:	List.dummy1
	sed -e 's/TYPE/Path/g' List.dummy1 >List_Path1.c

List_Path2.c:	List.dummy2
	sed -e 's/TYPE/Path/g' List.dummy2 >List_Path2.c

List_Path3.c:	List.dummy3
	sed -e 's/TYPE/Path/g' List.dummy3 >List_Path3.c

Map_Si1.c:	Map.dummy1
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/int/g' Map.dummy1 |\
		grep -v "^#include <int.h>" >Map_Si1.c

Map_Sl1.c:	Map.dummy1
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/long/g' Map.dummy1 |\
		grep -v "^#include <long.h>" >Map_Sl1.c

Map_Ss1.c:	Map.dummy1
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/short/g' Map.dummy1 |\
		grep -v "^#include <short.h>" >Map_Ss1.c

Map_iS1.c:	Map.dummy1
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/String/g' Map.dummy1 |\
		grep -v "^#include <int.h>" >Map_iS1.c

Map_lS1.c:	Map.dummy1
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/String/g' Map.dummy1 |\
		grep -v "^#include <long.h>" >Map_lS1.c

Map_sS1.c:	Map.dummy1
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/String/g' Map.dummy1 |\
		grep -v "^#include <short.h>" >Map_sS1.c

Map_SS1.c:	Map.dummy1
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/String/g' Map.dummy1 >Map_SS1.c

Map_UlUi1.c:	Map.dummy1
	sed -e 's/TYPE1/unsigned long/g' -e 's/TYPE2/unsigned int/g' Map.dummy1 |\
		grep -v "^#include <unsigned long.h>" |\
		grep -v "^#include <unsigned int.h>" >Map_UlUi1.c

Map_ii1.c:	Map.dummy1
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/int/g' Map.dummy1 |\
		grep -v "^#include <int.h>" >Map_ii1.c

Map_ll1.c:	Map.dummy1
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/long/g' Map.dummy1 |\
		grep -v "^#include <long.h>" >Map_ll1.c

Map_ss1.c:	Map.dummy1
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/short/g' Map.dummy1 |\
		grep -v "^#include <short.h>" >Map_ss1.c

Map_is1.c:	Map.dummy1
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/short/g' Map.dummy1 |\
		grep -v "^#include <int.h>" |\
		grep -v "^#include <short.h>" >Map_is1.c

Map_si1.c:	Map.dummy1
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/int/g' Map.dummy1 |\
		grep -v "^#include <short.h>" |\
		grep -v "^#include <int.h>" >Map_si1.c

Map_il1.c:	Map.dummy1
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/long/g' Map.dummy1 |\
		grep -v "^#include <int.h>" |\
		grep -v "^#include <long.h>" >Map_il1.c

Map_li1.c:	Map.dummy1
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/int/g' Map.dummy1 |\
		grep -v "^#include <long.h>" |\
		grep -v "^#include <int.h>" >Map_li1.c

Map_sl1.c:	Map.dummy1
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/long/g' Map.dummy1 |\
		grep -v "^#include <short.h>" |\
		grep -v "^#include <long.h>" >Map_sl1.c

Map_ls1.c:	Map.dummy1
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/short/g' Map.dummy1 |\
		grep -v "^#include <long.h>" |\
		grep -v "^#include <short.h>" >Map_ls1.c

Map_Si2.c:	Map.dummy2
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/int/g' Map.dummy2 |\
		grep -v "^#include <int.h>" >Map_Si2.c

Map_Sl2.c:	Map.dummy2
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/long/g' Map.dummy2 |\
		grep -v "^#include <long.h>" >Map_Sl2.c

Map_Ss2.c:	Map.dummy2
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/short/g' Map.dummy2 |\
		grep -v "^#include <short.h>" >Map_Ss2.c

Map_iS2.c:	Map.dummy2
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/String/g' Map.dummy2 |\
		grep -v "^#include <int.h>" >Map_iS2.c

Map_lS2.c:	Map.dummy2
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/String/g' Map.dummy2 |\
		grep -v "^#include <long.h>" >Map_lS2.c

Map_sS2.c:	Map.dummy2
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/String/g' Map.dummy2 |\
		grep -v "^#include <short.h>" >Map_sS2.c

Map_SS2.c:	Map.dummy2
	sed -e 's/TYPE1/String/g' -e 's/TYPE2/String/g' Map.dummy2 >Map_SS2.c

Map_UlUi2.c:	Map.dummy2
	sed -e 's/TYPE1/unsigned long/g' -e 's/TYPE2/unsigned int/g' Map.dummy2 |\
		grep -v "^#include <unsigned long.h>" |\
		grep -v "^#include <unsigned int.h>" >Map_UlUi2.c

Map_ii2.c:	Map.dummy2
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/int/g' Map.dummy2 |\
		grep -v "^#include <int.h>" >Map_ii2.c

Map_ll2.c:	Map.dummy2
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/long/g' Map.dummy2 |\
		grep -v "^#include <long.h>" >Map_ll2.c

Map_ss2.c:	Map.dummy2
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/short/g' Map.dummy2 |\
		grep -v "^#include <short.h>" >Map_ss2.c

Map_is2.c:	Map.dummy2
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/short/g' Map.dummy2 |\
		grep -v "^#include <int.h>" |\
		grep -v "^#include <short.h>" >Map_is2.c

Map_si2.c:	Map.dummy2
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/int/g' Map.dummy2 |\
		grep -v "^#include <short.h>" |\
		grep -v "^#include <int.h>" >Map_si2.c

Map_il2.c:	Map.dummy2
	sed -e 's/TYPE1/int/g' -e 's/TYPE2/long/g' Map.dummy2 |\
		grep -v "^#include <int.h>" |\
		grep -v "^#include <long.h>" >Map_il2.c

Map_li2.c:	Map.dummy2
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/int/g' Map.dummy2 |\
		grep -v "^#include <long.h>" |\
		grep -v "^#include <int.h>" >Map_li2.c

Map_sl2.c:	Map.dummy2
	sed -e 's/TYPE1/short/g' -e 's/TYPE2/long/g' Map.dummy2 |\
		grep -v "^#include <short.h>" |\
		grep -v "^#include <long.h>" >Map_sl2.c

Map_ls2.c:	Map.dummy2
	sed -e 's/TYPE1/long/g' -e 's/TYPE2/short/g' Map.dummy2 |\
		grep -v "^#include <long.h>" |\
		grep -v "^#include <short.h>" >Map_ls2.c

Block_int1.c:	Block.dummy1
	sed -e 's/TYPE/int/g' Block.dummy1 | grep -v "^#include <int.h>" >Block_int1.c

Block_int2.c:	Block.dummy2
	sed -e 's/TYPE/int/g' Block.dummy2 | grep -v "^#include <int.h>" >Block_int2.c

Block_long1.c:	Block.dummy1
	sed -e 's/TYPE/long/g' Block.dummy1 | grep -v "^#include <long.h>" >Block_long1.c

Block_long2.c:	Block.dummy2
	sed -e 's/TYPE/long/g' Block.dummy2 | grep -v "^#include <long.h>" >Block_long2.c

Block_short1.c:	Block.dummy1
	sed -e 's/TYPE/short/g' Block.dummy1 | grep -v "^#include <short.h>" >Block_short1.c

Block_short2.c:	Block.dummy2
	sed -e 's/TYPE/short/g' Block.dummy2 | grep -v "^#include <short.h>" >Block_short2.c

Block_Str1.c:	Block.dummy1
	sed -e 's/TYPE/String/g' Block.dummy1 >Block_Str1.c

Block_Str2.c:	Block.dummy2
	sed -e 's/TYPE/String/g' Block.dummy2 >Block_Str2.c

Block_ulong1.c:	Block.dummy1
	sed -e 's/TYPE/unsigned long/g' Block.dummy1 | grep -v "^#include <unsigned long.h>" >Block_ulong1.c

Block_ulong2.c:	Block.dummy2
	sed -e 's/TYPE/unsigned long/g' Block.dummy2 | grep -v "^#include <unsigned long.h>" >Block_ulong2.c

setofp_int1.c:	setofp.dummy1
	sed -e 's/TYPE/int/g' setofp.dummy1 | grep -v "^#include <int.h>" >setofp_int1.c

setofp_int2.c:	setofp.dummy2
	sed -e 's/TYPE/int/g' setofp.dummy2 | grep -v "^#include <int.h>" >setofp_int2.c

setofp_void1.c:	setofp.dummy1
	sed -e 's/TYPE/void/g' setofp.dummy1 | grep -v "^#include <void.h>" >setofp_void1.c

setofp_Str1.c:	setofp.dummy1
	sed -e 's/TYPE/String/g' setofp.dummy1 >setofp_Str1.c

setofp_Str2.c:	setofp.dummy2
	sed -e 's/TYPE/String/g' setofp.dummy2 >setofp_Str2.c

clean:
	rm -f *.a
	rm -f *.o
	rm -f *.c
	rm -rf ptrepository
