#ident	"@(#)cfront:scratch/bsd.sed	1.6"
echo "Fixing _iobuf structures:"
for f in */*..c
do
	echo $f
        sed -e '/__iobuf__base/s//&; int __iobuf__bufsiz/'  \
	-e '/_ctype/s//_ctype_/g' $f > temp$$
	mv temp$$ $f
done
echo "Before running \"make scratch\", you should set the"
echo "CCFLAGS and BSD macros in the makefile.  See the README for"
echo "details."
