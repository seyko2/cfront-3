example.E -align symbol -I incl1 foo.c -I incl2 -D FOO
align(symbol)
add_includedir(incl1)
add_includedir(incl2)
add_define(FOO, )
compile(foo.c)
compile_only = 0
optimize = 0
allow_anachronisms = 1
profiling = 0
output_filename = a.out
dryrun = 0
example.E -o blech -O -o -O -DFOO=BAR,BAZ foo.c -dryrun
add_define(FOO, BAR)
add_define(BAZ, )
compile(foo.c)
compile_only = 0
optimize = 1
allow_anachronisms = 1
profiling = 0
output_filename = -O
dryrun = 1
