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

test-linalg: gsl-linalg.so
	emacs -batch -q -l tests.el -f test-linalg


all: libemacs-module-helpers.so gsl-constants.so gsl-linalg.so

clean:
	rm *.o *.so
