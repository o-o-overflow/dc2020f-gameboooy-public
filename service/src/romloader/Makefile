all:
	gcc rom.c -I ../gb/gb.h -c
	g++ -std=c++14 romloader.cc -I ../rpclib/include/ ../rpclib/build/librpc.a rom.o -o romloader_server $(CFLAGS)

clean:
	rm -f romloader_server rom.o
