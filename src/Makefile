OBJECTS = mem-std.o mst-prim.o pq-bin-heap.o pq-fib-heap.o rng-mt.o \
rng-std.o set-rect.o uniformity.o

CFLAGS = -Wall -g -fPIC -O3

unf.so: $(OBJECTS)
	R CMD SHLIB unf.c $(OBJECTS)

clean:
	rm -f *.o
	rm -f *.so
