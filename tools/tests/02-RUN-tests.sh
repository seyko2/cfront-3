#!/bin/sh

source ./00-COMMON.sh

[ ! -d $td ] && {
    echo "Error: run 01-PREPARE-tests.sh"
    exit 1
}

for i in CC cfront libC.a munch ptcomp ptlink; do
    [ ! -f ../../$i ] && {
	echo "Error: not a file ../../$i"
	exit 1
    }
done
test_CC=$(readlink -f ../../CC)

function printmap
{
    sed '/@tab/,/@etab/d' $1 |
    awk '
	$1 == "@dec" {
	    tn = $2
	}
	$1 != "@dec" {
	    printf "%s %s\n", tn, $1
	}
    ' | sort
}

function printmap2
{
    awk '
	BEGIN {
	    intab = 0
	}
	$1 == "@tab" {
	    intab = 1
	    tn = 0;
	    next
	}
	$1 == "@etab" {
	    intab = 0
	    next
	}
	intab == 1 {
	    tab[tn++] = $0
	    next
	}
	{
	    if ($1 == "@dec") {
		printf "\n"
		flag = 1 
		flag2 = 0
	    }
	    else {
		flag = 0
	    }
	    for (i = 1; i <= NF; i++) {
		if ($i == "\\") {
		    flag2 = 1
		    continue
		}
		if (i > 1 && substr($i, 1, 1) == "@")
		    s = tab[substr($i, 2) + 0]
		else
		    s = $i
		if (i > 1)
		    printf " %s", s
		else
		    printf "%s", s
		# if (i > 2 && !flag || i > 1 && flag)
		#	printf " \\\n"
	    }
	    printf "\n"
	}
    ' $repm
}

count=0

function cleanup
{
    count=`expr $count + 1`
    count_str=`printf "%03d" $count`
    t2=${tstdir}/report-${count_str}.txt
    t2x=${tstdir}/report-${count_str}-2.txt

    rm -rf ./$rep ./$t1o 2> /dev/null
    # echo -n "."
}


cd $td
echo "$sep dynamic extensions"

    cleanup
    PTHDR= $test_CC -ptn $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    egrep 'assuming INC_EXT' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    PTSRC= $test_CC -ptn $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    egrep 'assuming SRC_EXT' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    PTSRC="." $test_CC -ptn $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    egrep 'illegal extension' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    cleanup
    PTHDR="xxx" $test_CC -ptn $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    egrep 'illegal extension' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    cleanup
    PTSRC=".xxxx" $test_CC -ptn $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
    egrep 'illegal extension' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }

    cleanup
    PTHDR=".xxx,.yyy,   ,,,,   .h  " PTSRC=".zzz,,,	.C" $test_CC -ptn $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
    egrep 'illegal extension' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #12 ***'; exit; }

echo "$sep pass through cfront options"

    cleanup
    $test_CC -ptn -ptv -gdump $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    fgrep 'gdump' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    $test_CC -ptn -ptv -gdem $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    fgrep 'gdem' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    $test_CC -ptn -ptv -ispeed $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    fgrep 'ispeed' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    cleanup
    $test_CC -ptn -ptv -ispace $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    fgrep 'ispace' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    cleanup
    $test_CC -ptn -ptv +d $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
    fgrep '+d' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }

echo "$sep -pth"

    cleanup
    $test_CC -ptn -pth $t37 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    x=`ls $rep/pt[0-9]* | wc -l`
    [ "$x" -lt 5 ] && { echo '*** error #2 ***'; exit; }

echo "$sep -I optimization"

    cleanup
    $test_CC -ptn -ptv $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    $test_CC -Ixxxx -ptn -ptv $t37 >/dev/null 2>$t2
    #[ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    $test_CC -I. -ptn -ptv $t51 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    $test_CC -Ixxx -I. -ptn -ptv $t51 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    $test_CC -Ixxx -I. -ptn -ptv $t51 >/dev/null 2>$t2
    #[ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #10 ***'; exit; }

    $test_CC -I. -ptn -ptv $t51 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }

echo "$sep -ptm"

    cleanup
    $test_CC -ptn -ptm./xmap $t37 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    egrep 'unconditional' xmap >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    egrep 'symbol.*needed' xmap >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }


echo "$sep -ptk"

    cleanup
    $test_CC -ptn -ptk $t50 >/dev/null 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    fgrep 'fatal error' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }
    fgrep 'instantiation errors' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }


echo "$sep countdown list"

    cleanup
    $test_CC -ptn -ptv $t37 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    fgrep 'left to do' $t2 1>/dev/null 2>/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep @tab in middle"

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec X fff
ggg.h
@tab
zzz
@etab
EOF
    $test_CC -ptn $t37 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep spacing in map file"

    cleanup
    $test_CC -ptn -c $t37 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    sed 's/\(@de[cf] && \) /\1     /g' $repm >x

    cp x $repm
    $test_CC -ptn $t37o >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep link order"

    cleanup
    $test_CC -c -ptn $t491 $t492 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    rm -f xlib.a
    ar cr xlib.a $t491o
    [ -x /bin/ranlib -o -x /usr/bin/ranlib ] && ranlib xlib.a
    $test_CC $t492o xlib.a >/dev/null 2>&1
    [ $? -ne 0 ] &&  { echo '*** error #2 ***'; exit; }


echo "$sep -ptt with header cache"

    cleanup
    $test_CC -ptn -ptt -ptv $t37 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit;}

    $test_CC -Ixxx -ptn -ptt -ptv $t37 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }

    $test_CC -Dxxx -ptn -ptt -ptv $t37 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    rm -f $rep/*.he
    $test_CC -Dxxx -ptn -ptt -ptv $t37 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    for i in $rep/*.he ; do echo > $i; done
    $test_CC -Dxxx -ptn -ptt -ptv $t37 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }

    touch $rep/*.C
    $test_CC -Dxxx -ptn -ptt -ptv $t37 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }
    fgrep '.C' $rep/*.he | fgrep -v 't37' # ???????????????????????

    cleanup
    $test_CC -ptt -ptv $t23 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #13 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #14 ***'; exit; }

    $test_CC -ptt -ptv $t23 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #15 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    #[ $? -eq 0 ] && { echo '*** error #16 ***'; exit; }

    $test_CC -ptt -ptv $t23 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #17 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    #[ $? -eq 0 ] && { echo '*** error #18 ***'; exit; }

    touch $t23c
    $test_CC -ptt -ptv $t23 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #19 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #20 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #21 ***'; exit; }

    touch $rep/*.C
    $test_CC -ptt -ptv $t23 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #22 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #23 ***'; exit; }
    #fgrep 'compiled' $t2 >/dev/null
    #[ $? -ne 0 ] && { echo '*** error #24 ***'; exit; }

    $test_CC -DZZZ -ptt -ptv $t23 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #25 ***'; exit; }
    fgrep 'rebuilt header file cache' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #26 ***'; exit; }
    fgrep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #27 ***'; exit; }
    sort $rep/*.he | uniq -c | awk '$1 > 1{echo}'
    awk 'NR > 1 && NF == 1 && $1 + 0 == 0{echo}' $rep/*.he >$t2
    sort $t2 | cmp - $t2


echo "$sep pointer to function as actual"

    cleanup
    $test_CC -ptn $t48 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep -I prefixes"

    cleanup
    mv $t8h $td/xxx/xxx
    $test_CC -c -I$td/xxx -I$td/xxx/xxx $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    mv $td/xxx/xxx/t8.h .
    grep 'xxx' $repm >/dev/null 2>/dev/null
    #[ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep -pta and @alltc"

    cleanup
    $test_CC -ptn -pta $t36 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    $test_CC -ptn -ptv $t36 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }

    echo 'int zzz = 57;' >> $t36
    $test_CC -ptn -ptv $t36 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    #[ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }


echo "$sep check defmap in multiple repositories"

    cleanup
    $test_CC -c $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    mkdir zzz
    $test_CC -ptrzzz -ptr$rep -ptn $t37o 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    rm -rf zzz


echo "$sep -ptt and local files"

    cleanup
    cp $t8h $t8c $td2
    $test_CC $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    touch $t8h
    $test_CC -ptt -ptv $t8 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    #[ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }

    touch $td2/t8.h
    $test_CC -ptt -ptv $t8 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    #[ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }

    cleanup
    rm -f t8.h
    $test_CC -ptt -ptv -I$td2 $t8 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    #[ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }

    cp $td2/t8.C . 2> /dev/null
    $test_CC -ptt -ptv -I$td2 $t8 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    #[ $? -eq 0 ] && { echo '*** error #9 ***'; exit; }
    cp $td2/t8.h . 2> /dev/null


echo "$sep -pts and dependency analysis"

    cleanup
    $test_CC -pts $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    $test_CC -pts -ptv $t8 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }

    $test_CC -pts -ptt -ptv $t8 2>$t2 1>/dev/null
    #[ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }


echo "$sep definitions found down -I path"

    cleanup
    mv $t8c $td2
    $test_CC -I$td2 -ptv $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    mv $td2/t8.C .


echo "$sep basenames and archives"

    cleanup
    $test_CC -c $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    #rm -f xlib.a
    #ar cr xlib.a $t8o
    #[ -x /bin/ranlib -o -x /usr/bin/ranlib ] && ranlib xlib.a
    #echo 'void f() {main();}' >x.C
    #cc -c x.C
    #$test_CC x.o xlib.a 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    #rm -f xlib.a


echo "$sep CC path"

    cleanup
    PATH="`dirname $test_CC`":$PATH CC -ptn $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep inlines and vtbls"

    cleanup
    $test_CC -ptn $t47 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep preserving quotes or brackets"

    cleanup
    $test_CC -c -I. $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec A
<t8.h>
@dec f
<t8.h>
@dec Lurch
"t8.$$.C"
EOF
    cat <<EOF >$repm
@dec ZZZ
zzz.h
EOF
    $test_CC -I. $t8o 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    grep '"t8\.[hc] && "' $rep/*.C >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }
    grep "<t8\.$$\..C>" $rep/*.C >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    cat <<EOF >$rep/nmap001
@dec A
"t8.h"
@dec f
"t8.h"
@dec Lurch
<t8.$$.C>
EOF
    $test_CC -I. $t8o 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    grep '<t8\.[hc] && >' $rep/*.C >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #6 ***'; exit; }
    grep "\"t8\.$$\.C\"" $rep/*.C >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #7 ***'; exit; }


echo "$sep numeric literals"

    cleanup
    $test_CC -ptn $t29 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep literals and pointers"

    cleanup
    $test_CC -ptn $t42_1 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    cleanup
    $test_CC -ptn $t42_2 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    $test_CC -ptn $t42_3 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }

    cleanup
    $test_CC -ptn $t42_4 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    $test_CC -ptn $t42_5 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }

    cleanup
    $test_CC -ptn $t42_6 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }


echo "$sep type checking of unused"

    cleanup
    $test_CC -ptn $t35 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    $test_CC -ptn -DBAD $t35 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep deleting old entries"

    cleanup
    cat <<EOF >$t15h
template <class T> struct A {};
enum E {};
EOF
    $test_CC -c $t15 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    cat <<EOF >$t15h
template <class T> struct A {};
EOF
    $test_CC -c $t15 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    printmap2 | grep "@dec E .*t15" >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    cat <<EOF >$t15h
template <class T> struct A {};
enum E {};
EOF
    $test_CC -c $t15 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    cat <<EOF >$t16h
template <class T> struct A {};
enum E {};
EOF
    $test_CC -c $t16 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    cat <<EOF >$t16h
template <class T> struct A {};
EOF
    $test_CC -c $t16 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }

    cleanup
    cat <<EOF >$t15h
template <class T> struct A {};
enum E {};
EOF
    $test_CC -c $t15 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }

    cat <<EOF >$repm
@dec A t15
t15.h
@dec E t15
t15.h
xxx.h
EOF
    cat <<EOF >$t15h
template <class T> struct A {};
EOF
    $test_CC -c $t15 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #13 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #14 ***'; exit; }

    cat <<EOF >$repm
@dec A t15
t15.h
@dec E t15
xxx.h
EOF
    cat <<EOF >$t15h
template <class T> struct A {};
EOF
    $test_CC -c $t15 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #15 ***'; exit; }
    printmap2 | grep "@dec E t15" >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #16 ***'; exit; }


echo "$sep basenames carried along"

    cleanup
    $test_CC $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    #printmap2 | grep " t8.$$" >/dev/null 2>&1
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    $test_CC $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    #printmap2 | grep "t8.$$ t8.$$" >/dev/null 2>&1
    #[ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    $test_CC $t10 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    #printmap2 | grep "@dec A .*t8.$$" >/dev/null 2>&1
    #[ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }
    #printmap2 | grep "@dec A t10.$$" >/dev/null 2>&1
    #[ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }


echo "$sep override quotes with brackets"

    cleanup
    echo 'template <class T> struct A {};' >xxx5.h
    echo '#include "xxx5.h"' >xxx5.C
    $test_CC -c xxx5.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    echo 'template <class T> struct A {};' >$td2/xxx5.h
    echo '#include <xxx5.h>' >xxx5.C
    $test_CC -c -I$td2 xxx5.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    x=`grep 'xxx5.h' $repm | wc -l`
    [ $x -ne 1 ] && { echo '*** error #3 ***'; exit; }


echo "$sep quotes vs brackets"

    cleanup
    echo 'foobar' >$td2/t46_1.h
    $test_CC -I$td2 $t46a 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    cleanup
    cp t46_1.h t46_1.bak
    cp t46_1.h t46_1.C $td2
    echo 'foobar' >$td/t46_1.h
    $test_CC -I$td2 $t46b 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    cp t46_1.bak t46_1.h

#echo
#echo "$sep cross device repositories"
#
#    cleanup
#    cp $t37 $HOME
#    cp $t37 t37.bak
#    cd $TMPDIR/..
#    rm -f $TMPDIR/*.Cs 2>/dev/null
#    $test_CC -ptn -ptr$TMPDIR t37.C 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    rm -f t37.C
#    cd $td
#    cp t37.bak t37.C


echo "$sep +a0"

    cleanup
    $test_CC +a0 -ptn -ptv $t37 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep '\+a0' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep vtbl and specializations"

    cleanup
    $test_CC -c $t453 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    $test_CC $t45 $t453o 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

#echo
#echo "$sep -pto"
#
#    cleanup
#    $test_CC -ptn $t37 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    $test_CC -ptn -ptv $t37 2>$t2 1>/dev/null
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }
#
#    cd $rep
#    ls *.o | sed -n '1,1p' > ../x
#    cd ..
#    echo '@delete' >>x
#    $test_CC -ptn -ptv -ptox $t37 2>$t2 1>/dev/null
#    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
#    [ -f x ] && { echo '*** error #6 ***'; exit; }
#
#    echo '@delete' >x
#    $test_CC -ptn -ptv -ptox $t37 2>$t2 1>/dev/null
#    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#     [ $? -eq 0 ] && { echo '*** error #8 ***'; exit; }
#    [ -f x ] && { echo '*** error #9 ***'; exit; }


echo "$sep .C not overwritten"

    cleanup
    $test_CC -ptn $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    [ -f $rep/_instfile ] && { echo '*** error #2 ***'; exit; }

    #$test_CC -ptn $t37 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    #[ ! -f $rep/_instfile ] && { echo '*** error #4 ***'; exit; }


echo "$sep .H/.C"

    cleanup
    cp $t8h t8.H	2>/dev/null 1>&2
    cp $t8c t8.C	2>/dev/null 1>&2
    $test_CC $t44 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep long lists of basenames"
#
#    cleanup
#    for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
#    do
#	cp $t8 xx${i}.C
#	$test_CC -c -ptn xx${i}.C 2>/dev/null 1>&2
#	[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    done
#
#    $test_CC xx25.o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#    rm -f xx[0-9] && *


echo "$sep hard path with -ptt"

    cleanup
    mkdir -p $rep
    rm -rf xdir
    mkdir xdir
    cp $t37 xdir
    cat <<EOF >$rep/nmap001
@dec A t37
xdir/$t37
EOF
    $test_CC -ptt -Ixdir $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    rm -rf xdir


echo "$sep function templates and pointers to members"

    cleanup
    $test_CC -ptn $t43 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep U then C when fakelinking"
#
#    cleanup
#    echo 'extern int x; void f() {x++;}' > x1.C
#    echo 'int x;' > x2.C
#    echo 'int x = 59;' > x3.C
#    cc -c x1.C x2.C x3.C >/dev/null 2>&1
#    ar cr xlib.a x2.o >/dev/null 2>&1
#    [ -x /bin/ranlib -o -x /usr/bin/ranlib ] && ranlib xlib.a
#    $test_CC -ptn x1.o xlib.a x3.o $t37 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    rm -f xlib.a


echo "$sep use of forwards"

    cleanup
    $test_CC $t22 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    #$test_CC -ptt $t22 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep PTOPTS"

    cleanup
    PTOPTS="-ptv -ptz" $test_CC -C -ptn $t37 2>$t2 1>/dev/null
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'ptlink' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

#echo
#echo "$sep basenames for x/y"
#
#    cleanup
#    mkdir x
#    mv $t37 x
#    $test_CC -ptn x/t37.C 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    mv x/t37.C .
#    rm -rf x


echo "$sep demangle works on ld output"

    cleanup
    $test_CC $t30 1>$t2 2>/dev/null
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
#    grep "class 'A':" $t2 >/dev/null
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#    grep "class 'A::B':" $t2 >/dev/null
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }

#echo
#echo "$sep use of literals"
#
#    cleanup
#    $test_CC $t21 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep -pts after -pta"
#
#    cleanup
#    $test_CC -pta $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#
#    cleanup
#    $test_CC -pta $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#
#    $test_CC -pts $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
#
#    cleanup
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
#
#    $test_CC -pta $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }
#
#    cleanup
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
#
#    $test_CC -pts $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }
#
#    cleanup
#    $test_CC -pts $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
#
#    $test_CC -pta $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }
#
#    cleanup
#    $test_CC -pts $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
#
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }


echo "$sep archive searched repeatedly"

    cleanup
    $test_CC -c $t19a1 $t19a2 1>/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    rm -f $t19a
    ar cr $t19a $t19o1 $t19o2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    [ -x /bin/ranlib -o -x /usr/bin/ranlib ] && ranlib $t19a
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }

    cleanup
    mkdir $rep
    cat <<EOF >$rep/nmap999
@dec A
t19.h
EOF
    cat <<EOF >$repm
@dec ZZZ
zzz.h
@def ZZZ
zzz.C
EOF
    $test_CC $t19 $t19a 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }


echo "$sep cppC options"

    cleanup
    #export cppC="/lib/cpp -Ifoo"
    cppC="${cppC-/lib/cpp} -Ifoo" $test_CC -ptn $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    #unset cppC


echo "$sep map file formatting"

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@tab
@etab
@dec A
zzz.h
EOF
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@tab
xxx
@etab
@dec A @1
zzz.h
EOF
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@dex ZZZ
zzz.h
EOF
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@dec ZZZ
@def ZZZZ
zzz.h
EOF
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@tab
@etab
@dec A
zzz.h
EOF
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@tab
xxx
@etab
@dec A @1
zzz.h
EOF
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #6 ***'; exit; }

    cleanup
    mkdir $rep
    cat <<EOF >$rep/nmap001
@dex ZZZ
zzz.h
EOF
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #7 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec ZZZ
@def ZZZZ
zzz.h
EOF
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #8 ***'; exit; }


echo "$sep -ptr."

    cleanup
    $test_CC -ptr. $t8 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep two +T"

    cleanup
    $test_CC +Tx +Ty zzz.C 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'warning: second' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep -ptt"

    cleanup
    $test_CC -ptn $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    $test_CC -ptn -ptt -ptv $t37 2>$t2
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }

    touch $t37
    $test_CC -ptn -ptt -ptv $t37 2>$t2
    #[ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
    grep 'compiled' $t2 >/dev/null
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }

#    rm -rf x1
#    mkdir x1
#    cp $rep/* x1
#    touch $rep/*.o
#    touch $t37
#    sleep 1
#    touch x1/*.o
#    $test_CC -ptn -ptt -ptv -ptr$rep -ptrx1 $t37 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#    [ $? -eq 0 ] && { echo '*** error #7 ***'; exit; }
#    rm -rf x1

#    cleanup
#    mkdir x1
#    mkdir -p $rep
#    cat <<EOF >x1/nmap001
#@dec A
#$td/t8.h
#EOF
#    $test_CC -ptr$rep -ptrx1 $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }
#
#    touch $td/t8.h
#    $test_CC -ptr$rep -ptrx1 -ptt -ptv $t8 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }
#    rm -rf x1


echo "$sep -lX"

    cleanup
    $test_CC -ptn $t33 -lcomplex 2>/dev/null 1>&2
    #[ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep +T and -ptn"

    cleanup
    echo 'void foo() {}' >x.C
    $test_CC -c $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    nm $NMFLAGS $t37o | egrep ' T [ ] && *.*f2.*A__pt|.*f2.*A__pt.*text' >/dev/null
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    $test_CC $t37 x.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    nm $NMFLAGS $t37o | egrep ' T [ ] && *.*f2.*A__pt|.*f2.*A__pt.*text' >/dev/null
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    $test_CC -ptn $t37 x.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    nm $NMFLAGS $t37o | egrep ' T [ ] && *.*f2.*A__pt|.*f2.*A__pt.*text' >/dev/null
    [ $? -eq 0 ] && { echo '*** error #6 ***'; exit; }

    cleanup
    echo '@none' >x
    $test_CC +Tx $t37 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    nm $NMFLAGS $t37o | egrep ' T [ ] && *.*f2.*A__pt|.*f2.*A__pt.*text' >/dev/null
    [ $? -eq 0 ] && { echo '*** error #8 ***'; exit; }


echo "$sep excessive iteration when unresolved"

    cleanup
    $test_CC -ptv $t41 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    x=`grep 'compiled' $t2 | wc -l`
    [ $x -ne 1 ] && { echo '*** error #2 ***'; exit; }


echo "$sep demangler on filenames"

    cleanup
    $test_CC -ptn $t40 >$t2 2>&1
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    egrep 'f\(int|long\)' $t2 >/dev/null
    #[ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep ft laydown"

    cleanup
    $test_CC -ptn $t39 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep nmapNNN wins"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    cat <<EOF >$rep/nmap001
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    cat <<EOF >$repm
#@dec A t8.$$
#t8xxx.h
#@dec f t8.$$
#t8xxx.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#
#    cat <<EOF >$rep/nmap001
#@dec f t8.$$
#t8.h
#EOF
#    cat <<EOF >$repm
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8xxx.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#
#echo
#echo "$sep use of -I / -D / hard paths"
#
#    cleanup
#    mkdir $rep
#    cat <<EOF >$rep/nmap001
#@dec AA t12.$$
#xxx/t12.h
#@dec ff t12.$$
#xxx/t12.h
#EOF
#    $test_CC -DN=23 $t12 -DZZZ 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    cleanup
#    $test_CC -Ixxx -DN=23 -Iyyy -DXXX $t12 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep AFAIL"

    cleanup
    $test_CC $t38 $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'ptlink' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

#echo
#echo "$sep inlines"
#
#    cleanup
#    $test_CC -ptn -I. $t32 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    nm $NMFLAGS $rep/*.o | egrep ' T [ ] && *.*f__10A|.*f__10A.*text' >/dev/null
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#
#    cleanup
#    $test_CC -ptn -DINLINE $t32 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#    nm $NMFLAGS $rep/*.o 2>/dev/null | egrep ' T [ ] && *.*f__10A|.*f__10A.*text' >/dev/null
#    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }


echo "$sep +T"

    cleanup
    cat <<EOF >xx.C
#include "t8.h"
#include "t8.C"
typedef A<double> _dummy;
EOF
    echo '@alltc' >instfile
    $test_CC +Tinstfile -c xx.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    nm $NMFLAGS xx.o | egrep ' T [ ] && *[_] && *f__10A__pt__2_dFv|f__10A__pt__2_dFv.*text' >/dev/null
    #[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    echo 'f' >instfile
    $test_CC +Tinstfile -c xx.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    nm $NMFLAGS xx.o | egrep ' T [ ] && *[_] && *f__10A__pt__2_dFv|f__10A__pt__2_dFv.*text' >/dev/null
    #[ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
    nm $NMFLAGS xx.o | egrep ' T [ ] && *[_] && *g__10A__pt__2_dFv|g__10A__pt__2_dFv.*text' >/dev/null
    #[ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }

#echo
#echo "$sep objects in archive"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    rm -f xlib.a
#    cp $t8o x.o
#    ar cr xlib.a $t8o x.o
#    [ -x /bin/ranlib -o -x /usr/bin/ranlib ] && {
#	ranlib xlib.a >/dev/null 2>&1
#    }
#    $test_CC -ptv $t7 xlib.a 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }
#
#    echo 'static void f() {main();}' > xx.C
#    cc -c xx.C
#    $test_CC -ptv xx.o xlib.a 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
#    grep 'compiled' $t2 >/dev/null
#    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }

#echo
#echo "$sep repository permissions"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    ls -l $repm | egrep -v 'rw-r--r--'
#    set `ls -ld .`
#    x=$1
#    set `ls -ld $rep`
#    y=$1
#    [ "$x" != "$y" ] && { echo '*** error #2 ***'; exit; }
#
#    cleanup
#    chmod 777 .
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#    ls -l $repm | egrep -v 'rw-rw-rw-'
#    set `ls -ld .`
#    x=$1
#    set `ls -ld $rep`
#    y=$1
#    [ "$x" != "$y" ] && { echo '*** error #4 ***'; exit; }
#
#    chmod 755 .
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
#    ls -l $repm | egrep -v 'rw-rw-rw-'
#    set `ls -ld .`
#    x=$1
#    set `ls -ld $rep`
#    y=$1
#    [ "$x" = "$y" ] && { echo '*** error #6 ***'; exit; }
#    chmod 755 .
#
#    cleanup
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
#    ls -l $rep/* | egrep -v 'rw-r--r--'
#    chmod 777 .
#
#    cleanup
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }
#    ls -l $rep/* | egrep -v 'rw-rw-rw-'
#    chmod 755 .
#
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
#    ls -l $rep/* | egrep -v 'rw-rw-rw-'

#echo
#echo "$sep order of inclusion of arguments"
#
#    cleanup
#    $test_CC $t10 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    ln1=`grep -n t10.h < $rep/*.C | awk -F: '{echo $1}'`
#    ln2=`grep -n t10_2.h < $rep/*.C | awk -F: '{echo $1}'`
#    ln3=`grep -n t10_3.h < $rep/*.C | awk -F: '{echo $1}'`
#    [ $ln1 -ge $ln2 ] && { echo '*** error #2 ***'; exit; }
#    [ $ln2 -ge $ln3 ] && { echo '*** error #3 ***'; exit; }

#echo
#echo "$sep order of archives and objects"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#
#    rm -f xlib.a
#    ar cr xlib.a $rep/*.o >/dev/null 2>&1
#    [ -x /bin/ranlib -o -x /usr/bin/ranlib ] && ranlib xlib.a
#    $test_CC -ptv $t8o xlib.a 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#    grep 'dependency' $t2 >/dev/null 2>&1
#    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }
#
#    cleanup
#    mkdir $rep
#    cat <<EOF >$repm
#@dec ZZZ
#zzz.h
#EOF
#    $test_CC xlib.a $t8o 2>/dev/null 1>&2
#    [ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }

#echo
#echo "$sep defined then undefined in objects"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#
#    $test_CC -ptv $rep/*.o $t8o 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#    grep 'dependency check' $t2 >/dev/null 2>&1
#    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }


echo "$sep ptcomp fails"

    cleanup
    mkdir $rep
    echo '@dec' > $repm
    $test_CC $t7 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep missing template.C"
#
#    cleanup
#    $test_CC -ptn $t26 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    grep 'warning: ##### template definition file' $t2 >/dev/null 2>&1
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep specialization of virtuals"

    cleanup
    $test_CC -ptn $t37 1>/dev/null 2>/dev/null
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep race condition on vtbls"
#
#    cleanup
#    $test_CC -ptn $t36 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    $test_CC -ptn -ptv $t36 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#    grep 'compiled' $t2>/dev/null 1>&2
#    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }


echo "$sep static data initializers"

    cleanup
    $test_CC -ptn $t34 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

#echo
#echo "$sep duplicate -Is"
#
#    cleanup
#    $test_CC -Ixxyyzz $t8 2>$t2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#    grep 'xxyyzz.*xxyyzz' $t2 >/dev/null
#    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

#echo
#echo "$sep type lookup"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap001
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap001
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    cat <<EOF >$repm
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap001
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >xxxx/nmap001
#@def A t8.$$
#t8.C
#@def f t8.$$
#t8.C
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >xxxx/nmap001
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >xxxx/nmap001
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >xxxx/nmap001
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap001
#@def A t8.$$
#t8.C
#@def f t8.$$
#t8.C
#EOF
#    cat <<EOF >$rep/nmap2
#@def A t8.$$
#zzz.h
#@def f t8.$$
#zzz.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap001
#@dec A
#zzz.h
#@dec f
#zzz.h
#EOF
#    cat <<EOF >$rep/nmap2
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap002
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    cat <<EOF >$rep/nmap001
#@dec A
#zzz.h
#@dec f
#zzz.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    cat <<EOF >$repm
#@dec A t8.$$
#zzz.h
#@dec f t8.$$
#zzz.h
#EOF
#    cat <<EOF >$rep/nmap001
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #13 ***'; exit; }
#
#    cleanup
#    mkdir -p $rep
#    cat <<EOF >$repm
#@def A
#zzz.h
#@def f
#zzz.h
#@def A t8.$$
#t8.C
#@def f t8.$$
#t8.C
#EOF
#    $test_CC $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #14 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    cat <<EOF >$repm
#@dec A t8.$$
#t8.h
#@dec f t8.$$
#t8.h
#EOF
#    cat <<EOF >xxxx/nmap001
#@dec A
#zzz.h
#@dec f
#zzz.h
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #15 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    cat <<EOF >$repm
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    cat <<EOF >xxxx/nmap001
#@dec A t8.$$
#zzz.h
#@dec f t8.$$
#zzz.h
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #16 ***'; exit; }
#
#    cleanup
#    rm -rf xxxx
#    mkdir -p $rep xxxx
#    echo '@dec ZZZ\nZZZ.H' >$repm
#    cat <<EOF >$rep/nmap002
#@dec A
#t8.h
#@dec f
#t8.h
#EOF
#    cat <<EOF >xxxx/nmap001
#@dec A t8.$$
#zzz.h
#@dec f t8.$$
#zzz.h
#EOF
#    $test_CC -ptr$rep -ptrxxxx $t8o 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #17 ***'; exit; }

#echo
#echo "$sep defmap.old written"
#
#    cleanup
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
#
#    $test_CC -c $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
#    [ ! -f ${repm}.old ] && { echo '*** error #3 ***'; exit; }

#echo
#echo "$sep flookup() fails"
#
#    cleanup
#    mkdir -p $rep
#    cat <<EOF >$rep/nmap001
#@dec A sfdjasdlkj
#zzz.h
#EOF
#    $test_CC $t8 2>/dev/null 1>&2
#    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


##############################################################################################


echo Done. Many tests are commented out. Look inside.
exit


echo "$sep map files wrongly formatted"

    cleanup
    mkdir -p $rep
    echo 'xxx' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    mkdir -p $rep
    echo '@xxx' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    mkdir -p $rep
    echo '@ xxx' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    cleanup
    mkdir -p $rep
    echo '@' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #7 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    cleanup
    mkdir $rep
    echo '@dec' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #9 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #10 ***'; exit; }

    cleanup
    mkdir -p $rep
    echo '@decxxx' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #11 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }

    cleanup
    mkdir -p $rep
    echo 'xxx.h' >$rep/nmap001
    $test_CC $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #13 ***'; exit; }
    grep 'error.*incorrectly formatted' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #14 ***'; exit; }


echo "$sep same file twice in a row"

    cleanup
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    awk '{for (i = 1; i <= NF; i++) printf "%s\n", $i}' $repm | sort > $td/x99
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    awk '{for (i = 1; i <= NF; i++) printf "%s\n", $i}' $repm | sort | df - $td/x99


echo "$sep ft expansion"

    cleanup
    $test_CC -ptv $t31 1>/dev/null 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'wrote instantiation file' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep map file formatting"

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@dec     T1  
       t.h

@def   T2   a1 a2	a3 a4
t2.h
EOF
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@dec
EOF
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@decx     T1
t.h
EOF
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@decx T1
t.h
EOF
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@dec 37
t.h
EOF
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$repm
@dec T1
EOF
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #6 ***'; exit; }

    cleanup
    mkdir $rep
    cat <<EOF >$repm
t.h
EOF
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #7 ***'; exit; }

    cleanup
    mkdir -p $rep
    echo '@dec T1' >$repm
    echo -n 't1.h' >> $repm
    $test_CC $t8 2>/dev/null 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #8 ***'; exit; }


echo "$sep only data needed"

    cleanup
    $test_CC -DINIT -DVAL=37 $t28 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    ./a.out
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep global common"

    cleanup
    $test_CC -DVAL=0 $t28 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep support for +i"

    cleanup
    $test_CC +i $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    ls $rep/*..[ci] &&  >$t2
    [ ! -s $t2 ] && { echo '*** error #2 ***'; exit; }


echo "$sep static data"

    cleanup
    $test_CC -DVAL=37 $t27 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    ./a.out
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    $test_CC -DVAL=47 -DOVER $t27 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    ./a.out
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }


echo "$sep specializations"

    cleanup
    $test_CC -c $t25s1 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    $test_CC -c $t25s2 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    $test_CC -c $t25s3 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }

    $test_CC -c $t25s4 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }

    $test_CC $t25s1o $t25s2o $t25s3o $t25s4o 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    ./a.out
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }


echo "$sep type defined twice in @def"

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec A t17
t17x.h
@def A t17
t17xx.C
@def A t17
t17xx.C
EOF
    $test_CC $t17 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep bad map file formatting"

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec 2A t17
t17x.h
@def A t17
t17xx.C
EOF
    $test_CC $t17 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec A t17
@def A t17
t17xx.C
EOF
    $test_CC $t17 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec A t17
t17x.h
@def A t17
EOF
    echo -n 't17xx.C' >>$rep/nmap001
    $test_CC $t17 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }


echo "$sep handling whitespace"

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001

@dec A      t17

	t17x.h

 @def A		t17

	 t17xx.C

EOF
    $test_CC $t17 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep -E / -F"

    cleanup
    $test_CC -ptv -E t8 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    egrep 'ptcomp|ptlink' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

    $test_CC -ptv -F t8 >/dev/null 2>$t2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    egrep 'ptcomp|ptlink' $t2 >/dev/null
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }
    [ -d $rep ] && { echo '*** error #5 ***'; exit; }


echo "$sep multiple repositories"

    cleanup
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    mv $rep x1
    $test_CC -Ixxx -Iyyy -DXXX -DN=23 $t12 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    mv $rep x2
    $test_CC -ptrx2 -ptrx1 $t8o 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    [ -d $rep ] && { echo '*** error #4 ***'; exit; }
    rm -rf x1 x2


echo "$sep replaying .C files"

    cleanup
    $test_CC $t24 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    x=`ls $rep/*.o 2>/dev/null | wc -l`
    [ $x -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep -ptn"

    cleanup
    $test_CC -ptn $t24 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    x=`ls $rep/*.o 2>/dev/null | wc -l`
    [ $x -eq 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep multiple -I"

    cleanup
    $test_CC -Ixxx -DN=23 -Iyyy -DXXX $t12 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep -pts"

    cleanup
    $test_CC -pts $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep -pts with -pta"

    cleanup
    $test_CC -pta -pts $t8 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'pta and -pts cannot both be specified' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep use of I/O library"

    cleanup
    $test_CC $t23 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    ./a.out >/dev/null
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep use of -o"

    cleanup
    $test_CC $t8 -o xxxt8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    ./xxxt8
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep use of -pta"

    cleanup
    $test_CC -pta $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    ./a.out
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep passing options to instantiation"

    cleanup
    $test_CC -ptv -g $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep ' -c *-g' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep bad/missing object files"

    cleanup
    echo 'asdflkjasdlfjkad' >$t20o
    $test_CC $t8 $t20o 1>/dev/null 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'fatal' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }

    cleanup
    $test_CC $t8 dflkadsjfasd.o 1>/dev/null 2>$t2
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }
    grep 'fatal' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }


echo "$sep long lists of object files"

    cleanup
    for i in 1 2 3 4 5 6 7 8 9 10
    do
	for j in 1 2 3 4 5 6 7 8 9 10
	do
	    echo $t20o
	done
    done >$t2
    rm -f $t20o
    cc -c $t20c
    $test_CC -ptv $t8 `<$t2` 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep support for locking"

    cleanup
    ($test_CC $t8 2>/dev/null 1>&2; echo $? >$t2) &
    ($test_CC $t8 2>/dev/null 1>&2; echo $? >$t2x) &
    wait
    read x <$t2
    [ "$x" -ne 0 ] && { echo '*** error #1 ***'; exit; }
    read x <$t2x
    [ "$x" -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep no symbols needed from archive"

    cleanup
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    rm -f x.a
    ar cr x.a $t8o >/dev/null 2>&1
    [ -x /bin/ranlib -o -x /usr/bin/ranlib ] && {
	ranlib x.a
	[ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }
    }

    cleanup
    $test_CC $t7 x.a 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }


echo "$sep another iteration required"

    cleanup
    $test_CC  $t18 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep empty object for nm"

    cleanup
    echo 'struct A {int x;};' > x.C
    cc -c x.C 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    $test_CC $t8 x.o 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep handling @def"

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@dec A t17
t17x.h
@def A t17
t17xx.C
EOF
    $test_CC $t17 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep echo list of unresolved"

    cleanup
    $test_CC $t9 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep "^	A<int>::g(void)$" $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep type defined twice"

    cleanup
    $test_CC $t11 $t11_2 2>$t2 1>/dev/null
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'error.*type.*defined twice' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep merging fts"

    cleanup
    $test_CC $t14 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep mapping .C to .h"

    cleanup
    mkdir -p $rep
    cat <<EOF >$rep/nmap001
@def f t13.$$
t13.C
@def A t13.$$
t13.C
EOF
    $test_CC -c $t13 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }

    echo '@dec ZZZ' >$repm
    echo 'lurch.h' >>$repm
    $test_CC $t13o 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep multiple types used as template args"

    cleanup
    $test_CC $t10 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }


echo "$sep type not found in map files"

    cleanup
    $test_CC -c $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    echo '@dec ZZZ' > $repm
    echo 'ZZZ.h' >> $repm
    $test_CC $t8o 2>$t2
    [ $? -eq 0 ] && { echo '*** error #2 ***'; exit; }
    # grep 'warning: ##### assuming template type' $t2 >/dev/null 2>&1
    # [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }


echo "$sep named repositories at link time"

    cleanup
    rm -rf xxxx
    $test_CC -ptrxxxx $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    x=`ls xxxx/*.C xxxx/*.o xxxx/*.Cs | wc -l`
    [ $x -ne 12 ] && { echo '*** error #2 ***'; exit; }
    rm -rf xxxx


echo "$sep loop detected in ptlink"

    cleanup
    $test_CC $t9 2>$t2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'error.*resolved' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep non-PT case works optimally"

    cleanup
    $test_CC -ptv $t7 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    [ -d $rep ] && { echo '*** error #2 ***'; exit; }
    grep 'CC\[pt' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #3 ***'; exit; }

    cleanup
    mkdir -p $rep
    $test_CC -ptv $t7 2>$t2
    [ $? -ne 0 ] && { echo '*** error #4 ***'; exit; }
    grep 'CC\[ptlink' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #5 ***'; exit; }


echo "$sep partitioning into files"

    cleanup
    $test_CC $t8 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    x=`ls $rep/*.C $rep/*.o $rep/*.Cs | wc -l`
    [ $x -ne 12 ] && { echo '*** error #2 ***'; exit; }


echo "$sep dependency management and checksums"

    cleanup
    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }

    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }

    rm -f $rep/*.o
    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #5 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #6 ***'; exit; }

    rm -f $rep/*.Cs
    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #7 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #8 ***'; exit; }

    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #9 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #10 ***'; exit; }

    echo ' ' >> $t8h
    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #11 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #12 ***'; exit; }

    echo ' ' >> $t8c
    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #13 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #14 ***'; exit; }

    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #15 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -eq 0 ] && { echo '*** error #16 ***'; exit; }

    echo 'int zzz = 89;' >> $t8c
    $test_CC -ptv $t8 2>$t2
    [ $? -ne 0 ] && { echo '*** error #17 ***'; exit; }
    grep 'compiled' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #18 ***'; exit; }


echo "$sep duplicate information elided"

    cleanup
    $test_CC -c $t6 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    printmap $repm | uniq -c | awk '$1 != 1{echo}'


echo "$sep right information extracted"

    cleanup
    $test_CC -c -I$td $t5 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    printmap $repm | df - $t5x


echo "$sep named repositories work"

    cleanup
    rm -rf x999
    $test_CC -c -ptr./x999 $t1 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    [ ! -s x999/$nmap ] && { echo '*** error #2 ***'; exit; }
    rm -rf x999


echo "$sep -ptr followed by nothing"

    cleanup
    $test_CC -c -ptr $t1 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'warning.*no repository' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep repository is a file"

    cleanup
    > $rep
    $test_CC -c $t1 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'warning: could not create' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep cannot create repository"

    cleanup
    cd /				# ?????????????
    $test_CC -c $t1 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    cd $OLDPWD
    grep 'warning: could not create' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep repository not writeable"

    cleanup
    mkdir $rep
    chmod 555 $rep
    $test_CC -c $t1 2>$t2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    grep 'warning: repository.*is not writable' $t2 >/dev/null 2>&1
    [ $? -ne 0 ] && { echo '*** error #2 ***'; exit; }


echo "$sep rep not created  not needed"

    cleanup
    $test_CC -c $t3 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #1 ***'; exit; }
    [ -d $rep ] && { echo '*** error #2 ***'; exit; }


echo "$sep no update  compile fails"

    cleanup
    $test_CC -c $t4 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #1 ***'; exit; }
    [ -d $rep ] && { echo '*** error #2 ***'; exit; }

    cleanup
    $test_CC -c $t1 2>/dev/null 1>&2
    [ $? -ne 0 ] && { echo '*** error #3 ***'; exit; }

    rm -f $repm
    $test_CC -c $t4 2>/dev/null 1>&2
    [ $? -eq 0 ] && { echo '*** error #4 ***'; exit; }
    [ -d $repm ] && { echo '*** error #5 ***'; exit; }

cleanup
