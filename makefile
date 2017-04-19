all: test1 test2 test3 test4 coalesce

test1: test1.c libuserthread.so
	gcc -g -o test1 test1.c -L. -luserthread -lrt

test2: test2.c libuserthread.so
	gcc -g -o test2 test2.c -L. -luserthread -lrt

test3: test3.c libuserthread.so
	gcc -g -o test3 test3.c -L. -luserthread -lrt

test4: test4.c libuserthread.so
	gcc -g -o test4 test4.c -L. -luserthread -lrt

coalesce: coalesce.c libuserthread.so
	gcc -g -o coalesce coalesce.c -L. -luserthread -lrt

libuserthread.so: mem.o
	gcc -g -o libuserthread.so mem.o -shared -lrt

mem.o: mem.c
	gcc -g -Wall -fpic -c mem.c -lrt

clean:
	rm *.o
	rm test1 test2 test3 test4
