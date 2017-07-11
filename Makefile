emacs-module-helpers.o: emacs-module-helpers.c emacs-module-helpers.h
	gcc -Wall -I/usr/local/include -fPIC -c emacs-module-helpers.c

libemacs-module-helpers.so: emacs-module-helpers.o
	gcc -shared -o libemacs-module-helpers.so emacs-module-helpers.o

gsl-constants.so: gsl-constants.o libemacs-module-helpers.so
	gcc -shared -L/usr/local/include -lgsl -L. -lemacs-module-helpers -o gsl-constants.so gsl-constants.o

gsl-constants.o: gsl-constants.c
	gcc -Wall -I/usr/local/include -fPIC -c gsl-constants.c

gsl-linalg.so: gsl-linalg.o libemacs-module-helpers.so
	gcc -shared -L/usr/local/include -lgsl -L. -lemacs-module-helpers -o gsl-linalg.so gsl-linalg.o

gsl-linalg.o: gsl-linalg.c
	gcc -Wall -I/usr/local/include -fPIC -c gsl-linalg.c

test-linalg: gsl-linalg.so tests.el
	emacs -batch -q -l tests.el -f test-linalg

test-constants: gsl-constants.so tests.el
	emacs -batch -q -l tests.el -f test-constants


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

##################################################################
# Special cases with nested functions
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


all: libemacs-module-helpers.so gsl-constants.so gsl-linalg.so

clean:
	rm *.o *.so
