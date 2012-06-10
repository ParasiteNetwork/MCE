C = $(CXX)
O = $(CXXFLAGS) -std=c++0x -pedantic -Wall
CC = $C $O

.PHONY: default
default:
	- echo "Targets: all clean"

.PHONY: clean
clean:
	- rm *.a;
	- rm *.o;
	- rm *~;
	- rm *.out;

.PHONY: all
all: libmce.a main.o
	$(CC) main.o libmce.a -o a.out

.PHONY: lib
libmce.a: debug.o mce-apt.o mce-stream_buffer.o mce-env.o mce-filepath.o mce-parser.o mce-scope.o mce-syntax.o mce-vars.o
	$(AR) $(ARFLAGS) $@ $^

debug.o: debug.cpp
	$(CC) -c $^

mce-apt.o: mce-apt.cpp
	$(CC) -c $^

mce-stream_buffer.o: mce-stream_buffer.cpp
	$(CC) -c $^

mce-env.o: mce-env.cpp
	$(CC) -c $^

mce-filepath.o: mce-filepath.cpp
	$(CC) -c $^

mce-parser.o: mce-parser.cpp
	$(CC) -c $^

mce-scope.o: mce-scope.cpp
	$(CC) -c $^

mce-syntax.o: mce-syntax.cpp
	$(CC) -c $^

mce-vars.o: mce-vars.cpp
	$(CC) -c $^

main.o: main.cpp
	$(CC) -c $^


