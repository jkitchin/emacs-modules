


server: hwserver.c
	gcc -lzmq hwserver.c -o hwserver



client: hwclient.c
	gcc -lzmq hwclient.c -o hwclient


mod-zmq.so: mod-zmq.o
	gcc -shared -lzmq -L.. -lemacs-module-helpers -o mod-zmq.so mod-zmq.o

mod-zmq.o: mod-zmq.c
	gcc -fPIC -c mod-zmq.c


test: mod-zmq.so
	# ./hwserver &
	emacs -batch -q -l test.el -f test-1

all: client server mod-zmq.so

clean:
	rm *.o mod-zmq.so hwclient hwserver
