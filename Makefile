all: libemacs-module-helpers.so gsl-constants.so gsl-linalg.so

clean:
	rm *.o *.so

##################
# Module helpers #
##################

emacs-module-helpers.o: emacs-module-helpers.c emacs-module-helpers.h
	gcc -Wall -I/usr/local/include -fPIC -c emacs-module-helpers.c

libemacs-module-helpers.so: emacs-module-helpers.o
	gcc -shared -o libemacs-module-helpers.so emacs-module-helpers.o

##################
# GSL constants  #
##################

gsl-constants.so: gsl-constants.o libemacs-module-helpers.so
	gcc -shared -L/usr/local/include -lgsl -L. -lemacs-module-helpers -o gsl-constants.so gsl-constants.o

gsl-constants.o: gsl-constants.c
	gcc -Wall -I/usr/local/include -fPIC -c gsl-constants.c

test-constants: gsl-constants.so tests.el
	emacs -batch -q -l tests.el -f test-constants

##############
# GSL linalg #
##############

gsl-linalg.so: gsl-linalg.o libemacs-module-helpers.so
	gcc -shared -L/usr/local/include -lgsl -L. -lemacs-module-helpers -o gsl-linalg.so gsl-linalg.o

gsl-linalg.o: gsl-linalg.c
	gcc -Wall -I/usr/local/include -fPIC -c gsl-linalg.c

test-linalg: gsl-linalg.so tests.el
	emacs -batch -q -l tests.el -f test-linalg


##############
# MOD-MKL module
##############
mod-mkl.so: mod-mkl.o libemacs-module-helpers.so
	sh /opt/intel/mkl/bin/mklvars.sh intel64
	gcc -shared -L${MKLROOT}/lib -Wl,-rpath,${MKLROOT}/lib -lmkl_rt -lpthread -lm -ldl -L. -lemacs-module-helpers -o mod-mkl.so mod-mkl.o

mod-mkl.o: mod-mkl.c
	sh /opt/intel/mkl/bin/mklvars.sh intel64
	gcc -Wall -m64 -I${MKLROOT}/include -fPIC -c mod-mkl.c

test-mod-mkl: mod-mkl.so tests.el
	sh /opt/intel/mkl/bin/mklvars.sh intel64
	emacs -batch -q -l tests.el -f test-mkl


#########################
# Miscellaneous modules #
#########################

mod-types.so: mod-types.o libemacs-module-helpers.so
	gcc -shared  -L. -lemacs-module-helpers -o mod-types.so mod-types.o

mod-types.o: mod-types.c
	gcc -Wall -fPIC -c mod-types.c

test-types: mod-types.so tests.el
	emacs -batch -q -l tests.el -f test-types

##################################################################

mod-list-vec.so: mod-list-vec.o libemacs-module-helpers.so
	gcc -shared  -L. -lemacs-module-helpers -o mod-list-vec.so mod-list-vec.o

mod-list-vec.o: mod-list-vec.c
	gcc -Wall -fPIC -c mod-list-vec.c

test-list-vec: mod-list-vec.so tests.el
	emacs -batch -q -l tests.el -f test-list-vec

test: test-constants test-linalg test-types test-list-vec



##########################################################################
# Special cases with nested functions #										       
# These require a special gcc since nested functions are not a standard c
# feature. In particular, on Mac OSX, the default gcc seems to use clang which
# does not support this. The path below is to a gcc that does support it. 
##########################################################################

gsl-integration.so: gsl-integration.o libemacs-module-helpers.so
	/usr/local/Cellar/gcc/6.1.0/bin/gcc-6  -shared -L/usr/local/include -lgsl -L. -lemacs-module-helpers -o gsl-integration.so gsl-integration.o

gsl-integration.o: gsl-integration.c
	/usr/local/Cellar/gcc/6.1.0/bin/gcc-6 -Wall -I/usr/local/include -fPIC -c gsl-integration.c

test-integration: gsl-integration.so tests.el
	emacs -batch -q -l tests.el -f test-integration

##################################################################

gsl-roots.so: gsl-roots.o libemacs-module-helpers.so
	/usr/local/Cellar/gcc/6.1.0/bin/gcc-6  -shared -L/usr/local/include -lgsl -L. -lemacs-module-helpers -o gsl-roots.so gsl-roots.o

gsl-roots.o: gsl-roots.c
	/usr/local/Cellar/gcc/6.1.0/bin/gcc-6 -Wall -I/usr/local/include -fPIC -c gsl-roots.c

test-roots: gsl-roots.so tests.el
	emacs -batch -q -l tests.el -f test-roots


